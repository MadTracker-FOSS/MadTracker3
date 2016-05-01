//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTColumns.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTColumns.h"
#include "MTObjects1.h"
#include "MTXMCommands.h"
#include <MTXAPI/MTXSkin.h>
#include <MTXAPI/MTXInput.h>
#include <MTXAPI/MTXSystem2.h>
#include <MTXAPI/RES/MTObjectsRES.h>
//---------------------------------------------------------------------------
NoteColumn notecolumn;
DrumsColumn drumscolumn;
VolumeColumn volumecolumn;
PanningColumn panningcolumn;
EffectColumn effectcolumn;
PatternType *pt;
//---------------------------------------------------------------------------
void initColumns()
{
	pt = (PatternType*)oi->getobjecttype(MTO_MTPATTERN);
	if (pt){
		pt->registercolumn(&notecolumn);
		pt->registercolumn(&drumscolumn);
		pt->registercolumn(&volumecolumn);
		pt->registercolumn(&panningcolumn);
		pt->registercolumn(&effectcolumn);
	};
}

void uninitColumns()
{
	if (pt){
		pt->unregistercolumn(&notecolumn);
		pt->unregistercolumn(&drumscolumn);
		pt->unregistercolumn(&volumecolumn);
		pt->unregistercolumn(&panningcolumn);
		pt->unregistercolumn(&effectcolumn);
	};
}
//---------------------------------------------------------------------------
static MTINoteEvent event = {0,0,0.0,0,sizeof(MTINoteEvent)};
static MTINoteEvent *cevent = &event;
unsigned char keymap[2][28] = {{0,0,0,0,0,0,0,0,44,31,45,32,46,47,34,48,35,49,36,50,51,38,52,39,53,0,0,0},{0,0,0,0,0,0,0,0,16,3,17,4,18,19,6,20,7,21,8,22,23,10,24,11,25,26,13,27}};
//---------------------------------------------------------------------------
NoteColumn::NoteColumn()
{
	description = "Note";
	type = CT_NOTE;
	nbytes = 2;
	ndata = sizeof(NoteData);
	ncpos = 4;
}

void NoteColumn::init(MTPatternInstance*,ColumnStatus &status)
{
}

void NoteColumn::firstpass(MTPatternInstance*,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	if ((*celldata>0) && (*celldata<97)) pass.flags |= MTFP_ISNOTE;
	if (*(celldata+1)) pass.flags |= MTFP_ISINS;
}

void NoteColumn::columnhandle(MTPatternInstance *pi,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	int x;
	unsigned char note = *celldata;
	unsigned char ins = *(celldata+1);

/*
	if (((note) && (pass.delay>-1.0)) || (ins)){
		if (pass.delay>0.0){
			status.nextevent += pass.delay;
			return;
		};
*/
	if (((note) && (pass.delay>-1.0)) || (ins)){
		NoteData &nd = *(NoteData*)status.data;
		InstrumentInstance *li = nd.lastinstance[0];
		event.source = pi->parent;
		event.flags = 0;
		if (note){
			nd.lastnote = note;
			if (pass.delay>-1.0) event.flags |= MTIEF_ISNOTE;
		}
		else note = nd.lastnote;
		if (ins){
			nd.lastins = ins;
			event.flags |= MTIEF_ISINS;
		}
		else ins = nd.lastins;
		event.type = MTIE_NOTE;
		switch (note){
		case 97:
			for (x=0;x<nd.ninstances;x++){
				InstrumentInstance *ci = nd.lastinstance[x];
				if ((!ci) || (ci->flags & IIF_BACKGROUND)) continue;
				event.note = 0;
				event.notelength = 0.0;
				event.noteoffset = 0.0;
				ci->sendevents(1,(MTIEvent**)&cevent);
			};
			break;
		case 98:
			break;
		case 99:
			break;
		default:
			event.notelength = 0.0;
			event.noteoffset = 0.0;
			event.note = nd.lastnote+11;
			event.gvolume = pass.gvolume;
			event.volume = pass.volume;
			event.gpanx = pass.gpanx;
			event.gpany = pass.gpany;
			event.gpanz = pass.gpanz;
			event.panx = pass.panx;
			event.pany = pass.pany;
			event.panz = pass.panz;
			nd.dnote = event.note;
			if ((pass.delay>-1.0) || (nd.cnote==0.0)) nd.cnote = nd.onote = event.note;
			else{
				event.note = nd.onote = nd.cnote;
			};
			if (event.flags!=(MTIEF_ISNOTE|MTIEF_ISINS)){
				if (pi->sendevents(1,(MTIEvent**)&cevent)){
					break;
				};
			};
			if ((nd.lastnote) && (nd.lastins) && (pi->module->instr->a[nd.lastins])){
				for (x=0;x<nd.ninstances;x++){
					InstrumentInstance *ci = nd.lastinstance[x];
					if ((!ci) || (ci->flags & IIF_BACKGROUND)) continue;
					event.type = MTIE_KILL;
					ci->sendevents(1,(MTIEvent**)&cevent);
				};
				event.type = MTIE_NOTE;
				Track *ctrk = (Track*)pi->module->trk->a[((MTPattern*)pi->parent)->tracks[pi->ctrack].id];
				if (ctrk){
					InstrumentInstance *ci = ((Instrument*)pi->module->instr->a[nd.lastins])->createinstance(ctrk,pi,nd.lastinstance[0]);
					if (ci){
						if (!pi->module->addchannel(ci)) delete ci;
						else{
							if (nd.lastinstance[MAX_POLYPHONY-1]){
								nd.lastinstance[MAX_POLYPHONY-1]->nextevent = -1.0;
								nd.ninstances--;
							};
							for (x=MAX_POLYPHONY-1;x>0;x--) nd.lastinstance[x] = nd.lastinstance[x-1];
							nd.lastinstance[0] = ci;
							nd.ninstances++;
							ci->sendevents(1,(MTIEvent**)&cevent);
						};
					};
				};
			};
		};
	};
	status.nextevent += 1.0;
}

int NoteColumn::getwidth(int charwidth)
{
	return (charwidth*11)/2;
}

void NoteColumn::drawcolumn(MTBitmap* b,MTRect &r,unsigned char *celldata,ColumnDrawState &state)
{
	unsigned char note = *(unsigned char*)celldata;
	int x,w,color;
	static unsigned char nt[12][3] = {"C.","C#","D.","D#","E.","F.","F#","G.","G#","A.","A#","B."};

	if ((state.line%state.lpb)==0) color = SC_PATT_TEXT3;
	else color = SC_PATT_TEXT1;
	if (state.flags & CDS_SELECTED) color++;
	color = skin->getcolor(color);
	x = r.left;
	switch (note){
	case 0:
		skin->drawtext((unsigned char*)"...",b,x,r.top,color);
		break;
	case 97:
		skin->drawtext((unsigned char*)"\x80\x81\x82",b,x,r.top,color);
		break;
	case 98:
		skin->drawtext((unsigned char*)"\x84\x85\x86",b,x,r.top,color);
		break;
	case 99:
		skin->drawtext((unsigned char*)"\x88\x89\x8A",b,x,r.top,color);
		break;
	default:
		skin->drawtext(nt[(note-1)%12],b,x,r.top,color);
		skin->drawchar('0'+((note-1)/12),b,x,r.top,color);
	};
	x += 4;
	if (*(celldata+1)) skin->drawhex(*(celldata+1),false,2,b,x,r.top,color);
	if (state.cursor>=0){
		w = skin->fontwidth;
		x = r.left;
		switch (state.cursor){
		case 0:
			w *= 3;
			break;
		case 1:
			x += 2*w;
			break;
		case 2:
			x += 3*w+4;
			break;
		case 3:
			x += 4*w+4;
			break;
		};
		b->fill(x,r.bottom-2,w,2,skin->getcolor(SC_CURSOR));
	};
}

void NoteColumn::onmessage(MTPattManager *pm,MTCMessage &msg,int cursor,unsigned char *celldata)
{
	int x;
	int offset = -1;
	unsigned char data = 0;
	unsigned char *oct1 = keymap[0];
	unsigned char *oct2 = keymap[1];

	switch (msg.msg){
	case MTCM_KEYDOWN:
		if ((msg.s) && (msg.s->group==pt->getksgroup())){
			switch (msg.s->user){
			case MTT_delete:
				if (cursor<2) offset = 0;
				else offset = 1;
				break;
			};
		};
		if (offset<0){
			if (cursor<2){
				if ((cursor==1) && (*celldata) && (msg.key>='0') && (msg.key<'8')){
					data = (*celldata-1)%12+(msg.key-'0')*12+1;
					offset = 0;
				};
				if (offset<0){
					for (x=0;x<28;x++,oct1++,oct2++){
						if (msg.scancode==*oct1){
							data = 37+x-8;
							offset = 0;
							break;
						};
						if (msg.scancode==*oct2){
							data = 49+x-8;
							offset = 0;
							break;
						};
					};
				};
			}
			else{
				if (((msg.key>='0') && (msg.key<='9')) || ((msg.key>='A') && (msg.key<='F'))){
					offset = 1;
					if (msg.key<='9') data = msg.key-'0';
					else data = msg.key-'A'+10;
					if (cursor==2){
						data = (*(celldata+1) & 0xF)|(data<<4);
					}
					else{
						data = (*(celldata+1) & 0xF0)|(data);
					};
				};
			};
		};
		if (offset>=0){
			*(celldata+offset) = data;
			pm->stepit();
		};
	};
}
//---------------------------------------------------------------------------
DrumsColumn::DrumsColumn()
{
	description = "Drums";
	type = CT_NOTE;
	nbytes = 2;
	ndata = sizeof(NoteData);
	ncpos = 3;
}

void DrumsColumn::init(MTPatternInstance*,ColumnStatus &status)
{
}

void DrumsColumn::firstpass(MTPatternInstance*,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	unsigned char note = *celldata;
	if (note){
		pass.flags |= MTFP_ISNOTE;
		pass.delay = (double)(note & 0x1F)/nticks-status.cpos;
	};
	if (*(celldata+1)) pass.flags |= MTFP_ISINS;
}

void DrumsColumn::columnhandle(MTPatternInstance *pi,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	int x;
	unsigned char note = *celldata & 0x80;
	unsigned char ins = *(celldata+1);

	if (((note) && (pass.delay>-1.0)) || (ins)){
		NoteData &nd = *(NoteData*)status.data;
		InstrumentInstance *li = nd.lastinstance[0];
		event.source = pi->parent;
		event.flags = 0;
		if (note){
			nd.lastnote = note;
			if (pass.delay>-1.0) event.flags |= MTIEF_ISNOTE;
		}
		else note = nd.lastnote;
		if (ins){
			nd.lastins = ins;
			event.flags |= MTIEF_ISINS;
		}
		else ins = nd.lastins;
		event.type = MTIE_NOTE;
		event.notelength = 0.0;
		event.noteoffset = 0.0;
		event.note = 60;
		event.gvolume = pass.gvolume;
		event.volume = pass.volume;
		event.gpanx = pass.gpanx;
		event.gpany = pass.gpany;
		event.gpanz = pass.gpanz;
		event.panx = pass.panx;
		event.pany = pass.pany;
		event.panz = pass.panz;
		nd.dnote = event.note;
		if ((pass.delay>-1.0) || (nd.cnote==0.0)) nd.cnote = nd.onote = event.note;
		else{
			event.note = nd.onote = nd.cnote;
		};
		if (event.flags!=(MTIEF_ISNOTE|MTIEF_ISINS)){
			if (pi->sendevents(1,(MTIEvent**)&cevent)){
				status.nextevent += 1.0;
				return;
			};
		};
		if ((nd.lastnote) && (nd.lastins) && (pi->module->instr->a[nd.lastins])){
			for (x=0;x<nd.ninstances;x++){
				InstrumentInstance *ci = nd.lastinstance[x];
				if ((!ci) || (ci->flags & IIF_BACKGROUND)) continue;
				event.type = MTIE_KILL;
				ci->sendevents(1,(MTIEvent**)&cevent);
			};
			event.type = MTIE_NOTE;
			Track *ctrk = (Track*)pi->module->trk->a[((MTPattern*)pi->parent)->tracks[pi->ctrack].id];
			if (ctrk){
				InstrumentInstance *ci = ((Instrument*)pi->module->instr->a[nd.lastins])->createinstance(ctrk,pi,nd.lastinstance[0]);
				if (ci){
					if (!pi->module->addchannel(ci)) delete ci;
					else{
						if (nd.lastinstance[MAX_POLYPHONY-1]){
							nd.lastinstance[MAX_POLYPHONY-1]->nextevent = -1.0;
							nd.ninstances--;
						};
						for (x=MAX_POLYPHONY-1;x>0;x--) nd.lastinstance[x] = nd.lastinstance[x-1];
						nd.lastinstance[0] = ci;
						nd.ninstances++;
						ci->sendevents(1,(MTIEvent**)&cevent);
					};
				};
			};
		};
	};
	status.nextevent += 1.0;
}

int DrumsColumn::getwidth(int charwidth)
{
	return (charwidth*7)/2;
}

void DrumsColumn::drawcolumn(MTBitmap* b,MTRect &r,unsigned char *celldata,ColumnDrawState &state)
{
	unsigned char note = *(unsigned char*)celldata;
	int x,w,color;

	if ((state.line%state.lpb)==0) color = SC_PATT_TEXT3;
	else color = SC_PATT_TEXT1;
	if (state.flags & CDS_SELECTED) color++;
	color = skin->getcolor(color);
	x = r.left;
	if (*celldata) skin->drawchar('\x90'+(state.line%8),b,x,r.top,color);
	else skin->drawchar('.',b,x,r.top,color);
	x += 4;
	if (*(celldata+1)) skin->drawhex(*(celldata+1),false,2,b,x,r.top,color);
	if (state.cursor>=0){
		w = skin->fontwidth;
		x = r.left;
		switch (state.cursor){
		case 0:
			w *= 2;
			break;
		case 1:
			x += 2*w+4;
			break;
		case 2:
			x += 3*w+4;
			break;
		};
		b->fill(x,r.bottom-2,w,2,skin->getcolor(SC_CURSOR));
	};
}

void DrumsColumn::onmessage(MTPattManager *pm,MTCMessage &msg,int cursor,unsigned char *celldata)
{
}
//---------------------------------------------------------------------------
VolumeColumn::VolumeColumn()
{
	description = "Volume";
	type = CT_OTHER;
	nbytes = 1;
	ndata = 0;
	ncpos = 2;
}

void VolumeColumn::init(MTPatternInstance*,ColumnStatus &status)
{
}

void VolumeColumn::firstpass(MTPatternInstance*,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	unsigned char vol = *celldata;

	if ((pass.flags & MTFP_ISNOTE) && (vol>=0x10) && (vol<=0x90)){
		pass.volume = (double)(vol-0x10)/128.0;
	};
}

void VolumeColumn::columnhandle(MTPatternInstance *pi,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	unsigned char vol = *celldata;

	if (vol>=0x10){
		event.type = MTIE_PARAM;
		MTIParamEvent &pe = *(MTIParamEvent*)&event;
		pe.param = MTIP_VOLUME;
		pe.dvalue1 = -1.0;
		pe.flags = 0;
	// Set volume
		if (vol<=0x090){
			status.nextevent += 1.0;
			if ((pass.flags & MTFP_ISNOTE)==0){
				pe.dvalue1 = /*pass.volume**/(double)(vol-0x10)/128.0;
			};
		}
	// Nothing
		else if (vol>=0xA0){
			if (vol<0x0C0)	status.nextevent += 1.0/nticks;
			else status.nextevent += 1.0;
			pe.flags = MTIEF_ADD;
		// Volume down
			if (vol<0x0B0){
				pe.dvalue1 = -/*pass.volume**/((double)(vol-0xA0)/64.0);
			}
		// Volume up
			else if (vol<0x0C0){
				pe.dvalue1 = /*pass.volume**/((double)(vol-0xB0)/64.0);
			}
		// Fine volume down
			else if (vol<0x0D0){
				if (tick==0) pe.dvalue1 = -/*pass.volume**/((double)(vol-0xC0)/64.0);
			}
		// Fine volume up
			else if (vol<0x0E0){
				if (tick==0) pe.dvalue1 = /*pass.volume**/((double)(vol-0xD0)/64.0);
			};
		};
		if (pe.dvalue1!=-1.0) pi->sendevents(1,(MTIEvent**)&cevent);
	}
	else{
		status.nextevent += 1.0;
	};
}

int VolumeColumn::getwidth(int charwidth)
{
	return charwidth*2;
}

void VolumeColumn::drawcolumn(MTBitmap *b,MTRect &r,unsigned char *celldata,ColumnDrawState &state)
{
	int x = r.left;
	int w,color;
	unsigned char vol = *celldata;

	if ((state.line%state.lpb)==0) color = SC_PATT_TEXT3;
	else color = SC_PATT_TEXT1;
	if (state.flags & CDS_SELECTED) color++;
	color = skin->getcolor(color);
// Nothing
	if (vol<0x10){
		skin->drawtext((unsigned char*)"..",b,x,r.top,color);
	}
// Set volume
	else if (vol<=0x90){
		skin->drawhex(vol-0x10,true,2,b,x,r.top,color);
	}
// Nothing
	else if (vol<0xA0){
	}
// Fine volume down
	else if (vol<0xB0){
		skin->drawchar('\x99',b,x,r.top,color);
		skin->drawhex(vol-0xA0,true,1,b,x,r.top,color);
	}
// Fine volume up
	else if (vol<0xC0){
		skin->drawchar('\x98',b,x,r.top,color);
		skin->drawhex(vol-0xA0,true,1,b,x,r.top,color);
	}
// Volume down
	else if (vol<0xD0){
		skin->drawchar('\x9B',b,x,r.top,color);
		skin->drawhex(vol-0xA0,true,1,b,x,r.top,color);
	}
// Volume up
	else if (vol<0xE0){
		skin->drawchar('\x9A',b,x,r.top,color);
		skin->drawhex(vol-0xA0,true,1,b,x,r.top,color);
	};
	if (state.cursor>=0){
		w = skin->fontwidth;
		x = r.left+state.cursor*w;
		b->fill(x,r.bottom-2,w,2,skin->getcolor(SC_CURSOR));
	};
}

void VolumeColumn::onmessage(MTPattManager *pm,MTCMessage &msg,int cursor,unsigned char *celldata)
{
	int offset = -1;
	unsigned char data = 0;

	switch (msg.msg){
	case MTCM_KEYDOWN:
		if ((msg.s) && (msg.s->group==pt->getksgroup())){
			switch (msg.s->user){
			case MTT_delete:
				offset = 0;
				break;
			};
		};
		if (offset<0){
			if (((msg.key>='0') && (msg.key<='9')) || ((msg.key>='A') && (msg.key<='F'))){
				offset = 0;
				if (msg.key<='9') data = msg.key-'0';
				else data = msg.key-'A'+10;
				if (cursor==0){
					data++;
					data = (*celldata & 0xF)|(data<<4);
				}
				else{
					data = (*celldata & 0xF0)|(data);
				};
				if (data<0x10) data += 0x10;
			};
		};
		if (offset>=0){
			*(celldata+offset) = data;
			pm->stepit();
		};
	};
}
//---------------------------------------------------------------------------
PanningColumn::PanningColumn()
{
	description = "Panning";
	type = CT_OTHER;
	nbytes = 1;
	ndata = 0;
	ncpos = 2;
}

void PanningColumn::init(MTPatternInstance*,ColumnStatus &status)
{
}

void PanningColumn::firstpass(MTPatternInstance*,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	if ((pass.flags & MTFP_ISNOTE) && (*celldata)){
		pass.panx = (float)(*celldata-128)/127;
	};
}

void PanningColumn::columnhandle(MTPatternInstance *pi,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	if ((*celldata) && ((pass.flags & MTFP_ISNOTE)==0)){
		event.type = MTIE_PARAM;
		MTIParamEvent &pe = *(MTIParamEvent*)&event;
		pe.param = MTIP_PANNING;
		pe.flags = 0;
		pe.fvalue1 = (float)(*celldata-128)/127;
		pe.fvalue2 = pe.fvalue3 = 0.0;
		pi->sendevents(1,(MTIEvent**)&cevent);
	};
	status.nextevent += 1.0;
}

int PanningColumn::getwidth(int charwidth)
{
	return charwidth*2;
}

void PanningColumn::drawcolumn(MTBitmap *b,MTRect &r,unsigned char *celldata,ColumnDrawState &state)
{
	int x = r.left;
	int w,color;

	if ((state.line%state.lpb)==0) color = SC_PATT_TEXT3;
	else color = SC_PATT_TEXT1;
	if (state.flags & CDS_SELECTED) color++;
	color = skin->getcolor(color);
	if (*celldata){
		skin->drawhex(*celldata,true,2,b,x,r.top,color);
	}
	else{
		skin->drawtext((unsigned char*)"..",b,x,r.top,color);
	};
	if (state.cursor>=0){
		w = skin->fontwidth;
		b->fill(r.left+state.cursor*w,r.bottom-2,w,2,skin->getcolor(SC_CURSOR));
	};
}

void PanningColumn::onmessage(MTPattManager *pm,MTCMessage &msg,int cursor,unsigned char *celldata)
{
	int offset = -1;
	unsigned char data = 0;

	switch (msg.msg){
	case MTCM_KEYDOWN:
		if ((msg.s) && (msg.s->group==pt->getksgroup())){
			switch (msg.s->user){
			case MTT_delete:
				offset = 0;
				break;
			};
		};
		if (offset<0){
			if (((msg.key>='0') && (msg.key<='9')) || ((msg.key>='A') && (msg.key<='F'))){
				offset = 0;
				if (msg.key<='9') data = msg.key-'0';
				else data = msg.key-'A'+10;
				if (cursor==0){
					data = (*celldata & 0xF)|(data<<4);
				}
				else{
					data = (*celldata & 0xF0)|(data);
				};
			};
		};
		if (offset>=0){
			*(celldata+offset) = data;
			pm->stepit();
		};
	};
}
//---------------------------------------------------------------------------
EffectColumn::EffectColumn()
{
	description = "Effect";
	type = CT_EFFECT;
	nbytes = 3;
	ndata = sizeof(EffectData);
	ncpos = 6;
}

void EffectColumn::init(MTPatternInstance*,ColumnStatus &status)
{
	register EffectData &ed = *(EffectData*)&status.data;

	ed.needbreak = false;
	ed.needjump = 0;
}

void EffectColumn::firstpass(MTPatternInstance *pi,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	unsigned char effect = *celldata++;
	unsigned short param = *(unsigned short*)celldata;

	switch (effect){
// XM Effect
	case 0x00:
		xmfirstpass(pi,param,pass,status,tick,nticks);
		break;
// Note delay
	case 0x0D:
		pass.delay = (double)param/(double)(nticks*256)-status.cpos;
		break;
	};
}

void EffectColumn::columnhandle(MTPatternInstance *pi,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	int x;
	NoteData *nd = (NoteData*)&pi->getnotestatus()->data;
	unsigned char effect = *celldata++;
	unsigned short param = *(unsigned short*)celldata;
	register EffectData &ed = *(EffectData*)&status.data;
	double inc = 1.0;
	static MTIParamEvent pe[2] = {{0,MTIE_PARAM,0.0,0,sizeof(MTIParamEvent)},{0,MTIE_PARAM,0.0,0,sizeof(MTIParamEvent)}};
	static MTIParamEvent *cpe = pe;

	switch (effect){
// XM Effect
	case 0x00:
		inc = xmcommand(pi,param,pass,status,tick,nticks);
		break;
// Portamento up
	case 0x01:
		if (tick){
			if (!param) param = ed.old[0];
			else ed.old[0] = param;
			pe[0].param = MTIP_NOTE;
			pe[0].flags = MTIEF_ADD;
			pe[0].dvalue1 = (double)param/256;
			pi->sendevents(1,(MTIEvent**)&cpe);
		};
		inc = 1.0/nticks;
		break;
// Portamento down
	case 0x02:
		if (tick){
			if (!param) param = ed.old[0];
			else ed.old[0] = param;
			pe[0].param = MTIP_NOTE;
			pe[0].flags = MTIEF_ADD;
			pe[0].dvalue1 = -(double)param/256;
			pi->sendevents(1,(MTIEvent**)&cpe);
		};
		inc = 1.0/nticks;
		break;
// Tone portamento
	case 0x03:
		if (tick){
			if (!param) param = ed.old[0];
			else ed.old[0] = param;
			pe[0].param = MTIP_NOTE;
			pe[0].flags = MTIEF_ADD;
			if (nd->ninstances){
				unsigned char note = *pi->getnotedata();
				double ref = (double)param/256;
				if (note) nd->dnote = note+11;
				pe[0].dvalue1 = nd->dnote-nd->cnote;
				if (pe[0].dvalue1<-ref) pe[0].dvalue1 = -ref;
				else if (pe[0].dvalue1>ref) pe[0].dvalue1 = ref;
				nd->cnote += pe[0].dvalue1;
				pi->sendevents(1,(MTIEvent**)&cpe);
			};
		};
		inc = 1.0/nticks;
		break;
// IT Effect
	case 0x10:
		break;
// Reverse
	case 0x24:
		pe[0].param = MTIP_NOTE;
		pe[0].flags = MTIEF_MUL;
		pe[0].dvalue1 = -1.0;
		for (x=0;x<nd->ninstances;x++){
			InstrumentInstance &ci = *nd->lastinstance[x];
			if (ci.flags & IIF_BACKGROUND) continue;
			if (pass.flags & MTFP_ISNOTE) ci.seek(0.0,MTIS_END);
			ci.sendevents(1,(MTIEvent**)&cpe);
		};
		break;
	};
	status.nextevent += inc;
}

int EffectColumn::getwidth(int charwidth)
{
	return (charwidth*13)/2;
}

void EffectColumn::drawcolumn(MTBitmap *b,MTRect &r,unsigned char *celldata,ColumnDrawState &state)
{
	int x = r.left;
	int w,color;
	unsigned short param;

	if ((state.line%state.lpb)==0) color = SC_PATT_TEXT3;
	else color = SC_PATT_TEXT1;
	if (state.flags & CDS_SELECTED) color++;
	color = skin->getcolor(color);
	if (*celldata) skin->drawhex(*celldata,false,2,b,x,r.top,color);
	else skin->drawtext((unsigned char*)"..",b,x,r.top,color);
	x += 4;
	param = *(unsigned short*)(celldata+1);
	if (param) skin->drawhex(param,false,4,b,x,r.top,color);
	else skin->drawtext((unsigned char*)"....",b,x,r.top,color);
	if (state.cursor>=0){
		w = skin->fontwidth;
		x = r.left+state.cursor*w;
		if (state.cursor>1) x += 4;
		b->fill(x,r.bottom-2,w,2,skin->getcolor(SC_CURSOR));
	};
}

void EffectColumn::onmessage(MTPattManager *pm,MTCMessage &msg,int cursor,unsigned char *celldata)
{
	int offset = -1;
	unsigned char data = 0;

	switch (msg.msg){
	case MTCM_KEYDOWN:
		if ((msg.s) && (msg.s->group==pt->getksgroup())){
			switch (msg.s->user){
			case MTT_delete:
				offset = cursor/2;
				if (offset) offset = 3-offset;
				break;
			};
		};
		if (offset<0){
			if (((msg.key>='0') && (msg.key<='9')) || ((msg.key>='A') && (msg.key<='F'))){
				offset = cursor/2;
				if (offset) offset = 3-offset;
				if (msg.key<='9') data = msg.key-'0';
				else data = msg.key-'A'+10;
				if ((cursor%2)==0){
					data = (*(celldata+offset) & 0xF)|(data<<4);
				}
				else{
					data = (*(celldata+offset) & 0xF0)|(data);
				};
			};
		};
		if (offset>=0){
			*(celldata+offset) = data;
			pm->stepit();
		};
	};
}
//---------------------------------------------------------------------------
