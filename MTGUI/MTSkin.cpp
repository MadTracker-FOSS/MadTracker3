//---------------------------------------------------------------------------
//
//	MadTracker GUI Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <math.h>
#include "../Headers/MTXSystem.h"
#include "MTGUI1.h"
#include "MTSkin.h"
#include "MTSysControls.h"
#include "MTSign.h"
#include "MTItems.h"
#include "MTLabel.h"
#include "MTTabControl.h"
#include "MTButton.h"
#include "MTCheckBox.h"
#include "MTProgress.h"
//---------------------------------------------------------------------------
struct MTCheckBoxData{
	int timerid;
	int clicktime;
	float cstate;
	int fromstate;
	float fromcstate;
};
struct MTSignData{
	int timerid;
	int frame;
};
//---------------------------------------------------------------------------
MTResources *skinres;
unsigned char MTSkin::fontmap[256];
static const char *passtxt = {"********************************"};
//---------------------------------------------------------------------------
MTSkin::MTSkin():
Skin()
{
	unsigned char x;

	updatemetrics();
	mtmemzero(fontmap,sizeof(fontmap));
	for (x=0;x<96;x++){
		fontmap[fnm.fontseq[x]] = x;
	};
	mtmemzero(bmpid,sizeof(bmpid));
	mtmemzero(skinbmp,sizeof(skinbmp));
	mtmemzero(mask,sizeof(mask));
	mtmemzero(mr,sizeof(mr));
	mtmemzero(hskfont,sizeof(hskfont));
	mtmemzero(br,sizeof(br));
	sko[0].k = "Background"; sko[0].t = sk_back; sko[0].a = &bkg; sko[0].s = sizeof(bkg); sko[0].p = sizeof(MTSkinPart);
	sko[1].k = "Fonts"; sko[1].t = sk_font; sko[1].a = &fnm; sko[1].s = sizeof(fnm); sko[1].p = sizeof(MTFNMetrics);
	sko[2].k = "Windows"; sko[2].t = sk_wins; sko[2].a = &wnm; sko[2].s = sizeof(wnm); sko[2].p = sizeof(MTWNMetrics);
	sko[3].k = "Buttons"; sko[3].t = sk_btns; sko[3].a = &btm; sko[3].s = sizeof(btm); sko[3].p = sizeof(MTBTMetrics);
	sko[4].k = "Tab Buttons"; sko[4].t = sk_tbtn; sko[4].a = &tbtm; sko[4].s = sizeof(tbtm); sko[4].p = sizeof(MTBTMetrics);
	sko[5].k = "Disabled Tab Buttons"; sko[5].t = sk_dtbt; sko[5].a = &dtbtm; sko[5].s = sizeof(dtbtm); sko[5].p = sizeof(MTBTMetrics);
	sko[6].k = "Window Buttons"; sko[6].t = sk_wbtn; sko[6].a = &wbtns; sko[6].s = sizeof(wbtns); sko[6].p = sizeof(MTSQMetrics);
	sko[7].k = "Disabled Window Buttons"; sko[7].t = sk_dbtn; sko[7].a = &dbtns; sko[7].s = sizeof(dbtns); sko[7].p = sizeof(MTSQMetrics);
	sko[8].k = "Main Window Buttons"; sko[8].t = sk_mbtn; sko[8].a = &mbtns; sko[8].s = sizeof(mbtns); sko[8].p = sizeof(MTSQMetrics);
	sko[9].k = "Arrows"; sko[9].t = sk_arrw; sko[9].a = &arm; sko[9].s = sizeof(arm); sko[9].p = sizeof(MTSkinPart);
	sko[10].k = "Combo-box"; sko[10].t = sk_cbbt; sko[10].a = &cbbm; sko[10].s = sizeof(cbbm); sko[10].p = sizeof(MTSkinPart);
	sko[11].k = "Horizontal Scroller"; sko[11].t = sk_hscr; sko[11].a = &hscm; sko[11].s = sizeof(hscm); sko[11].p = sizeof(MTBTMetrics);
	sko[12].k = "Vertical Scroller"; sko[12].t = sk_vscr; sko[12].a = &vscm; sko[12].s = sizeof(vscm); sko[12].p = sizeof(MTBTMetrics);
	sko[13].k = "Horizontal Lift"; sko[13].t = sk_hscb; sko[13].a = &hscbm; sko[13].s = sizeof(hscbm); sko[13].p = sizeof(MTBTMetrics);
	sko[14].k = "Vertical Lift"; sko[14].t = sk_vscb; sko[14].a = &vscbm; sko[14].s = sizeof(vscbm); sko[14].p = sizeof(MTBTMetrics);
	sko[15].k = "Disabled Horizontal Scroller"; sko[15].t = sk_hdsc; sko[15].a = &hdscm; sko[15].s = sizeof(hdscm); sko[15].p = sizeof(MTBTMetrics);
	sko[16].k = "Disabled Vertical Scroller"; sko[16].t = sk_vdsc; sko[16].a = &vdscm; sko[16].s = sizeof(vdscm); sko[16].p = sizeof(MTBTMetrics);
	sko[17].k = "Status Buttons"; sko[17].t = sk_stbt; sko[17].a = &sbm; sko[17].s = sizeof(sbm); sko[17].p = sizeof(MTSkinPart);
	sko[18].k = "Pattern"; sko[18].t = sk_patt; sko[18].a = &ptm; sko[18].s = sizeof(ptm); sko[18].p = sizeof(MTBTMetrics);
	sko[19].k = "Horizontal Slider"; sko[19].t = sk_hsli; sko[19].a = &hslm; sko[19].s = sizeof(hslm); sko[19].p = sizeof(MTSLMetrics);
	sko[20].k = "Vertical Slider"; sko[20].t = sk_vsli; sko[20].a = &vslm; sko[20].s = sizeof(vslm); sko[20].p = sizeof(MTSLMetrics);
	sko[21].k = "Knob"; sko[21].t = sk_knob; sko[21].a = &knm; sko[21].s = sizeof(knm); sko[21].p = sizeof(MTSLMetrics);
	sko[22].k = "Volume Slider"; sko[22].t = sk_vols; sko[22].a = &vsm; sko[22].s = sizeof(vsm); sko[22].p = sizeof(MTSLMetrics);
	sko[23].k = "Vu-meter"; sko[23].t = sk_vume; sko[23].a = &vum; sko[23].s = sizeof(vum); sko[23].p = sizeof(MTSLMetrics);
	sko[24].k = "Animation"; sko[24].t = sk_anim; sko[24].a = &animm; sko[24].s = sizeof(animm); sko[24].p = sizeof(MTSQMetrics);
	sko[25].k = "Icon"; sko[25].t = sk_icon; sko[25].a = &icom; sko[25].s = sizeof(icom); sko[25].p = sizeof(MTSQMetrics);
	sko[26].k = "Checkbox Buttons"; sko[26].t = sk_chkb; sko[26].a = &cbm; sko[26].s = sizeof(cbm); sko[26].p = sizeof(MTSkinPart);
	sko[27].k = "Radio Buttons"; sko[27].t = sk_radb; sko[27].a = &rbm; sko[27].s = sizeof(rbm); sko[27].p = sizeof(MTSkinPart);
	sko[28].k = "Image List"; sko[28].t = sk_imag; sko[28].a = &ilm; sko[28].s = sizeof(ilm); sko[28].p = sizeof(MTSQMetrics);
	sko[29].k = "Progress Bar"; sko[29].t = sk_prog; sko[29].a = &prm; sko[29].s = sizeof(prm); sko[29].p = sizeof(MTSLMetrics);
	sko[30].k = "Skin Information"; sko[30].t = sk_skin; sko[30].a = &info; sko[30].s = sizeof(info); sko[30].p = sizeof(MTSkinInfo);
	sko[31].k = "Bitmap Indexes"; sko[31].t = sk_bmid; sko[31].a = &bmpid; sko[31].s = sizeof(bmpid); sko[31].p = sizeof(int);
}

MTSkin::~MTSkin()
{
	int x;

	delfonts();
	delshadows();
	for (x=0;x<16;x++){
		if (skinbmp[x]){
			di->delbitmap(skinbmp[x]);
			skinbmp[x] = 0;
			di->setskinbitmap(x,0);
		};
	};
}

void MTSkin::loadfromres(MTResources *res)
{
	int type,size,csize,x,l;
	void *stream;
	bool hadshadows;
	
	FENTER1("loadskinfromres(%.8X)",res);
	hadshadows = (mask[0]!=0);
	delfonts();
	delshadows();
	for (x=0;x<16;x++){
		if (skinbmp[x]){
			di->delbitmap(skinbmp[x]);
			skinbmp[x] = 0;
			di->setskinbitmap(x,0);
		};
	};
	stream = res->getresource(MTR_SKIN,0,&size);
	if (stream){
		char *sp = (char*)stream;
		char *ep = sp+size;
		while (sp<ep){
			type = *(int*)sp;
			sp += 4;
			csize = *(int*)sp;
			sp += 4;
			for (x=0;x<32;x++){
				if (type==sko[x].t){
					l = csize;
					if (sko[x].s<l) l = sko[x].s;
					memcpy(sko[x].a,sp,l);
					switch (sko[x].t){
					case sk_hsli:
						hslm.orientation = SKIN_HORIZ;
						break;
					case sk_vsli:
						vslm.orientation = SKIN_VERT;
						break;
					case sk_vols:
						vsm.orientation = SKIN_VERT;
						break;
					case sk_vume:
						vum.orientation = SKIN_VERT;
						break;
					case sk_knob:
						knm.orientation = SKIN_BOTH;
						break;
					};
					break;
				};
			};
			sp += csize;
		};
		for (x=0;x<8;x++) calcbounds(x);
		updatemetrics();
		sysimages.setmetrics(&ilm);
		res->releaseresource(stream);
		for (x=0;x<16;x++){
			if (bmpid[x]){
				MTBitmap *cskin = di->newresbitmap(MTB_SKIN,res,bmpid[x],info.colorkey);
				skinbmp[x] = cskin;
				cskin->changeproc = skinchange;
				cskin->param = (void*)x;
				di->setskinbitmap(x,cskin);
			};
		};
		setfonts();
		if (hadshadows) setshadows(true);
	}
	else{
		LOGD("%s - [GUI] ERROR: Cannot load skin!"NL);
	};
	LEAVE();
}

void MTSkin::savetores(MTResources *res)
{
}

void MTSkin::skinchange(MTBitmap *oldskin,MTBitmap *newskin,void *param)
{
	int id = (int)param;

	if (skin->skinbmp[id]==oldskin){
		skin->skinbmp[id] = newskin;
		di->setskinbitmap(id,newskin);
	};
}

void MTSkin::initcontrol(MTControl *ctrl)
{
	switch (ctrl->guiid){
	case MTC_CHECKBOX:
		{
			MTCheckBoxData *cbd = mtnew(MTCheckBoxData);
			cbd->timerid = gi->ctrltimer(ctrl,MTTF_SKINTIMER,guiprefs.animinterval,false,true);
			ctrl->skindata = cbd;
		};
		break;
	case MTC_SIGN:
		{
			MTSignData *ad = mtnew(MTSignData);
			ad->timerid = gi->ctrltimer(ctrl,MTTF_SKINTIMER,1000/animm[0].speed,false,true);
			ctrl->skindata = ad;
		};
		break;
	};
}

void MTSkin::uninitcontrol(MTControl *ctrl)
{
	switch (ctrl->guiid){
	case MTC_CHECKBOX:
		{
			MTCheckBoxData *cbd = (MTCheckBoxData*)ctrl->skindata;
			gi->deltimer(ctrl,cbd->timerid);
		};
		break;
	case MTC_SIGN:
		{
			MTSignData *ad = (MTSignData*)ctrl->skindata;
			gi->deltimer(ctrl,ad->timerid);
		};
		break;
	};
	if (ctrl->skindata){
		si->memfree(ctrl->skindata);
		ctrl->skindata = 0;
	};
}

void MTSkin::resetcontrol(MTControl *ctrl,bool skinchange)
{
	switch (ctrl->guiid){
	case MTC_CHECKBOX:
		{
			MTCheckBoxData *cbd = (MTCheckBoxData*)ctrl->skindata;
			MTCheckBox &cb = *(MTCheckBox*)ctrl;
			if (cbd->timerid) gi->deltimer(ctrl,cbd->timerid);
			cbd->timerid = gi->ctrltimer(ctrl,MTTF_SKINTIMER,guiprefs.animinterval,false,true);
			cbd->cstate = cb.state;
		};
		break;
	case MTC_SIGN:
		{
			MTSignData *ad = (MTSignData*)ctrl->skindata;
			if (ad->timerid) gi->deltimer(ctrl,ad->timerid);
			ad->timerid = gi->ctrltimer(ctrl,MTTF_SKINTIMER,1000/animm[((MTSign*)ctrl)->sign].speed,false,true);
			break;
		};
	};
}

void MTSkin::timercontrol(MTControl *ctrl)
{
	switch (ctrl->guiid){
	case MTC_CHECKBOX:
		{
			MTCheckBoxData &cbd = *(MTCheckBoxData*)ctrl->skindata;
			MTCheckBox &cb = *(MTCheckBox*)ctrl;
			cbd.cstate = cbd.fromcstate+((float)cb.state-cbd.fromcstate)*(float)(si->syscounter()-cbd.clicktime)/guiprefs.animtime;
			if (cb.state>cbd.fromstate){
				if (cbd.cstate>(float)cb.state){
					cbd.cstate = (float)cb.state;
					if (cbd.timerid){
						gi->deltimer(ctrl,cbd.timerid);
						cbd.timerid = 0;
					};
				};
			}
			else{
				if (cbd.cstate<(float)cb.state){
					cbd.cstate = (float)cb.state;
					if (cbd.timerid){
						gi->deltimer(ctrl,cbd.timerid);
						cbd.timerid = 0;
					};
				};
			};
			MTCMessage msg = {MTCM_CHANGE,0,ctrl};
			ctrl->parent->message(msg);
		};
		break;
	case MTC_SIGN:
		{
			MTSignData *ad = (MTSignData*)ctrl->skindata;
			if (++ad->frame==animm[((MTSign*)ctrl)->sign].nx*animm[((MTSign*)ctrl)->sign].ny) ad->frame = 0;
			MTCMessage msg = {MTCM_CHANGE,0,ctrl};
			ctrl->parent->message(msg);
		};
		break;
	};
}

void MTSkin::notify(MTControl *ctrl,int type,int param1,int param2,void *param3)
{
	switch (ctrl->guiid){
	case MTC_CHECKBOX:
		{
			MTCheckBoxData &cbd = *(MTCheckBoxData*)ctrl->skindata;
			MTCheckBox &cb = *(MTCheckBox*)ctrl;
			cbd.fromstate = cb.state;
			cbd.fromcstate = cbd.cstate;
			if ((guiprefs.animctrl) && (!design)){
				cbd.clicktime = si->syscounter();
				if (!cbd.timerid) cbd.timerid = gi->ctrltimer(ctrl,MTTF_SKINTIMER,guiprefs.animinterval,false,true);
			}
			else{
				cbd.cstate = (float)cb.state;
			};
		};
		break;
	};
}

void MTSkin::drawcontrol(MTControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y,int flags)
{
	switch (ctrl->guiid){
	case MTC_SIGN:
//	Sign
		{
			int anim = ((MTSign*)ctrl)->sign;
			if (animm[anim].nx<=0) break;
			b->skinblta(x+(ctrl->width-animm[anim].a.b.w/animm[anim].nx)/2,y+(ctrl->height-animm[anim].a.b.h/animm[anim].ny)/2,0,0,animm[anim].a,animm[anim].nx,animm[anim].ny,((MTSignData*)ctrl->skindata)->frame);
		};
		break;
	case MTC_LABEL:
//	Label
		{
			if (ctrl->parent->open(0)){
				ctrl->parent->settextcolor(fnm.colors[SC_TEXT_NORMAL & 0xFFFF]);
				ctrl->parent->setfont(hskfont[1]);
				ctrl->parent->drawtext(((MTLabel*)ctrl)->caption,-1,rect,DTXT_MULTILINE);
				ctrl->parent->close(0);
			};
		};
		break;
	case MTC_SCROLLER:
//	Scroller
		{
			MTScroller &s = *(MTScroller*)ctrl;
			int ah,i,h,w;
			bool ok = ((s.page<s.maxpos) && (s.maxpos>0));

			ah = arm.b.h/2;
			if (s.type==MTST_HBAR){
				i = 2;
				if (!ok) i += 8;
				b->skinblta(x,y,0,0,arm,8,2,i+((s.carrow==0)?4:0));
				i++;
				b->skinblta(x+s.width-ah,y,0,0,arm,8,2,i+((s.carrow==1)?4:0));
			}
			else{
				i = 0;
				if (!ok) i += 8;
				b->skinblta(x,y,0,0,arm,8,2,i+((s.carrow==0)?4:0));
				i++;
				b->skinblta(x,y+s.height-ah,0,0,arm,8,2,i+((s.carrow==1)?4:0));
			};
			if (s.type==MTST_HBAR){
				w = s.width-ah*2;
				drawbutton(b,x+ah,y,w,ah,(ok)?&hscm:&hdscm);
				if (ok){
					s.f = (float)w/s.maxpos;
					s.bs = (int)(s.f*s.page);
					if (s.bs<s.minsize){
						s.bs = s.minsize;
						s.f = (float)(w-s.bs)/(s.maxpos-s.page);
					};
					drawbutton(b,x+ah+(int)(s.pos*s.f),y,s.bs,ah,&hscbm);
				};
			}
			else{
				h = s.height-ah*2;
				drawbutton(b,x,y+ah,ah,h,(ok)?&vscm:&vdscm);
				if (ok){
					s.f = (float)h/s.maxpos;
					s.bs = (int)(s.f*s.page);
					if (s.bs<s.minsize){
						s.bs = s.minsize;
						s.f = (float)(h-s.bs)/(s.maxpos-s.page);
					};
					drawbutton(b,x,y+ah+(int)(s.pos*s.f),ah,s.bs,&vscbm);
				};
			};
		};
		break;
	case MTC_DESKTOP:
//	Desktop
		{
			((MTWinControl*)ctrl)->skinblt(0,0,ctrl->width,ctrl->height,bkg);
		};
		break;
	case MTC_USERLIST:
	case MTC_FILELISTBOX:
//	List
		{
			((MTWinControl*)ctrl)->fillcolor(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,fnm.colors[SC_EDIT_BACKGROUND & 0xFFFF]);
		};
		break;
	case MTC_LISTITEM:
//	List Item
		{
			int l,color;
			MTPoint p;
			MTListItem &li = *(MTListItem*)ctrl;

			if (li.caption[0]=='|'){
				if (b->open(0)){
					int h = li.height/2;
					MTRect r = {x+20,y};

					b->setbrush(fnm.colors[SC_EDIT_BACKGROUND & 0xFFFF]);
					b->setpen(fnm.colors[SC_CTRL_S & 0xFFFF]);
					b->moveto(x,y+h-1);
					b->lineto(x+li.width,y+h-1);
					b->setpen(fnm.colors[SC_CTRL_L & 0xFFFF]);
					b->moveto(x,y+h);
					b->lineto(x+li.width,y+h);
					b->gettextsize(&li.caption[1],-1,(MTPoint*)&r.right);
					r.right += r.left;
					r.bottom += r.top;
					if (r.right>x+li.width-2) r.right = x+li.width-2;
					if (r.bottom>y+li.height) r.bottom = y+li.height;
					b->fillex(r.left,r.top,r.right-r.left,r.bottom-r.top);
					b->settextcolor(fnm.colors[SC_CTRL_S & 0xFFFF]);
					b->setfont(hskfont[1]);
					l = b->gettextextent(&li.caption[1],-1,r.right-r.left);
					if (l<strlen(&li.caption[1])){
						if (b->gettextsize("...",-1,&p)){
							l = b->gettextextent(&li.caption[1],-1,r.right-r.left-p.x);
						}
						else{
							l -= 3;
						};
					}
					else l = -1;
					b->drawtext(&li.caption[1],l,r,DTXT_VCENTER);
					if (l>0){
						if (b->gettextsize(&li.caption[1],l,&p)){
							r.left += p.x;
							b->drawtext("...",-1,r,DTXT_VCENTER);
						};
					};
					b->close(0);
				};
			}
			else{
				MTRect r = {x,y};
				b->fill(x,y,li.width,li.height,(li.flags & MTCF_SELECTED)?fnm.colors[SC_EDIT_SELECTION & 0xFFFF]:fnm.colors[SC_EDIT_BACKGROUND & 0xFFFF]);
				if (((MTListBox*)li.parent)->viewflags & MTVF_IMAGES){
					sysimages.drawimage(li.imageindex,b,x+2,y);
					r.left += 18;
				};
				if (li.flags & MTCF_SELECTED) color = fnm.colors[SC_EDIT_SELECTED & 0xFFFF];
				else if (li.flags & MTCF_OVER) color = fnm.colors[SC_TEXT_FOCUSED & 0xFFFF];
				else color = fnm.colors[SC_TEXT_NORMAL & 0xFFFF];
				if (b->open(0)){
					b->settextcolor(color);
					b->setfont(hskfont[1]);
					l = b->gettextextent(li.caption,-1,r.right-r.left);
					if (l<strlen(li.caption)){
						if (b->gettextsize("...",-1,&p)){
							l = b->gettextextent(li.caption,-1,r.right-r.left-p.x);
						}
						else{
							l -= 3;
						};
					}
					else l = -1;
					b->drawtext(li.caption,l,r,DTXT_VCENTER);
					if (l>0){
						if (b->gettextsize(li.caption,l,&p)){
							r.left += p.x;
							b->drawtext("...",-1,r,DTXT_VCENTER);
						};
					};
					b->close(0);
				};
			};
		};
		break;
	case MTC_TABCONTROL:
//	Tab Control
		{
			int cbtn;
			int bx,by;
			MTRect tr;
			MTTabControl &ctc = *(MTTabControl*)ctrl;
			MTWNMetrics &cwnm = wnm[ctc.style];

			if (!(ctrl->flags & MTCF_DONTDRAW)){
				ctc.skinblt(0,0,ctc.width,ctc.height,bkg);
			};
			drawinnerwindow(b,x,y,ctc.width,ctc.height,ctc.style);
			if ((ctc.cstyle) && (ctc.page)){
				tr.left = ctc.width-ctc.btnx;
				tr.top = ctc.btny;
				tr.right = ctc.width-cwnm.btno.x;
				tr.bottom = ctc.br.top-cwnm.btno.y;
				if ((!&rect) || (cliprect(tr,rect))){
					bx = x+ctc.width-ctc.btnx;
					by = y+ctc.btny;
					cbtn = 0;
					if (ctc.cstyle & MTWS_STAYONTOP){
						b->skinblta(bx,by,0,0,dbtns.a,dbtns.nx,dbtns.ny,0+((ctc.btnd==cbtn)?12:((cbtn==ctc.btno)?6:0)));
						bx += ctc.btnw;
						cbtn++;
					};
					if (ctc.cstyle & MTWS_HELP){
						b->skinblta(bx,by,0,0,dbtns.a,dbtns.nx,dbtns.ny,1+((ctc.btnd==cbtn)?12:((cbtn==ctc.btno)?6:0)));
						bx += ctc.btnw;
						cbtn++;
					};
					if (ctc.cstyle & MTWS_MINIMIZE){
						b->skinblta(bx,by,0,0,dbtns.a,dbtns.nx,dbtns.ny,2+((ctc.btnd==cbtn)?12:((cbtn==ctc.btno)?6:0)));
						bx += ctc.btnw;
						cbtn++;
					};
					if (ctc.cstyle & MTWS_MAXIMIZE){
						b->skinblta(bx,by,0,0,dbtns.a,dbtns.nx,dbtns.ny,3+((ctc.btnd==cbtn)?12:((cbtn==ctc.btno)?6:0)));
						bx += ctc.btnw;
						cbtn++;
					};
					if (ctc.cstyle & MTWS_CLOSE){
						b->skinblta(bx,by,0,0,dbtns.a,dbtns.nx,dbtns.ny,5+((ctc.btnd==cbtn)?12:((cbtn==ctc.btno)?6:0)));
						bx += ctc.btnw;
						cbtn++;
					};
				};
			};
		};
		break;
	case MTC_LISTBOX:
//	List Box
		{
			((MTListBox*)ctrl)->fillcolor(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,fnm.colors[SC_EDIT_BACKGROUND & 0xFFFF]);
		};
		break;
	case MTC_MENUITEM:
//	Menu Item
		{
			MTMenuItem &mi = *(MTMenuItem*)ctrl;
			MTPoint size;
			int color;

			if (mi.caption[0]=='|'){
				if (b->open(0)){
					int h = mi.height/2;
					MTRect r = {x+20,y};

					b->setbrush(fnm.colors[SC_BACKGROUND & 0xFFFF]);
					b->setpen(fnm.colors[SC_CTRL_S & 0xFFFF]);
					b->moveto(x,y+h-1);
					b->lineto(x+mi.width,y+h-1);
					b->setpen(fnm.colors[SC_CTRL_L & 0xFFFF]);
					b->moveto(x,y+h);
					b->lineto(x+mi.width,y+h);
					b->gettextsize(&mi.caption[1],-1,(MTPoint*)&r.right);
					r.right += r.left;
					r.bottom += r.top;
					if (r.right>x+mi.width-2) r.right = x+mi.width-2;
					if (r.bottom>y+mi.height) r.bottom = y+mi.height;
					b->fillex(r.left,r.top,r.right-r.left,r.bottom-r.top);
					b->settextcolor(fnm.colors[SC_TEXT_NORMAL & 0xFFFF]);
					b->setfont(hskfont[1]);
					b->drawtext(&mi.caption[1],-1,r,DTXT_VCENTER);
					b->close(0);
				};
			}
			else{
				MTRect r = {x+2,y,x+mi.width-2,y+mi.height};
				b->fill(x,y,mi.width,mi.height,(mi.flags & MTCF_OVER)?fnm.colors[SC_EDIT_SELECTION & 0xFFFF]:fnm.colors[SC_BACKGROUND & 0xFFFF]);
				if (((MTListBox*)mi.parent)->viewflags & MTVF_IMAGES){
					sysimages.drawimage(mi.imageindex,b,x+2,y);
					r.left += 18;
				};
				if (mi.flags & MTCF_OVER) color = fnm.colors[SC_EDIT_SELECTED & 0xFFFF];
				else color = fnm.colors[SC_TEXT_NORMAL & 0xFFFF];
				if (b->open(0)){
					b->settextcolor(color);
					b->setfont(hskfont[1]);
					b->drawtext(mi.caption,-1,r,DTXT_VCENTER);
					if (mi.hotkeyoffset>=0){
						if (b->gettextsize(mi.caption,mi.hotkeyoffset,&size,r.right-r.left)){
							size.y = b->getcharwidth(mi.hotkey);
							r.bottom = r.top+b->getbaseline()+2;
							b->setpen(color);
							b->moveto(r.left+size.x,r.bottom);
							b->lineto(r.left+size.x+size.y,r.bottom);
						};
					};
					b->close(0);
				};
			};
		};
		break;
	case MTC_MENU:
//	Menu
		{
			((MTMenu*)ctrl)->fillcolor(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,fnm.colors[SC_BACKGROUND & 0xFFFF]);
		};
		break;
	case MTC_WINDOW:
//	Window
		{
			MTWindow &w = *(MTWindow*)ctrl;
			int tflags,bx,by,cbtn;
			int cstyle = w.style & 0xFF;
			MTWNMetrics &cwnm = wnm[cstyle];
			bool clipped = true;
			MTRect cr = {0,0,w.width,w.height};
			MTRect tr;

			if (&rect){
				clipped = cliprect(cr,rect);
			};
			if ((clipped) && (!(w.flags & MTCF_DONTDRAW))){
				w.clip(rect);
				w.skinblt(0,0,w.width,w.height,cwnm.bkg);
				w.unclip();
			};
			if (ctrl->guiid!=MTC_WINDOW) break;
			if (&rect) w.clip(rect);
			drawwindow(b,x,y,w.width,w.height,cstyle);
			tr.left = w.width-w.btnx;
			tr.top = -w.btny;
			tr.right = w.width-cwnm.btno.x;
			tr.bottom = -cwnm.btno.y;
			if ((!&rect) || (cliprect(tr,rect))){
				MTSQMetrics &btns = (cstyle==7)?mbtns:wbtns;
				bx = x+w.width-w.btnx;
				by = y-w.btny;
				cbtn = 0;
				if (w.style & MTWS_STAYONTOP){
					b->skinblta(bx,by,0,0,btns.a,btns.nx,btns.ny,0+((w.btnd==cbtn)?12:((cbtn==w.btno)?6:0)));
					bx += w.btnw;
					cbtn++;
				};
				if (w.style & MTWS_HELP){
					b->skinblta(bx,by,0,0,btns.a,btns.nx,btns.ny,1+((w.btnd==cbtn)?12:((cbtn==w.btno)?6:0)));
					bx += w.btnw;
					cbtn++;
				};
				if (w.style & MTWS_MINIMIZE){
					b->skinblta(bx,by,0,0,btns.a,btns.nx,btns.ny,2+((w.btnd==cbtn)?12:((cbtn==w.btno)?6:0)));
					bx += w.btnw;
					cbtn++;
				};
				if (w.style & MTWS_MAXIMIZE){
					b->skinblta(bx,by,0,0,btns.a,btns.nx,btns.ny,4+((w.btnd==cbtn)?12:((cbtn==w.btno)?6:0)));
					bx += w.btnw;
					cbtn++;
				};
				if (w.style & MTWS_CLOSE){
					b->skinblta(bx,by,0,0,btns.a,btns.nx,btns.ny,5+((w.btnd==cbtn)?12:((cbtn==w.btno)?6:0)));
					bx += w.btnw;
					cbtn++;
				};
			};
			if ((cstyle==1) || (cstyle==3) || (cstyle==4)){
				tr.left = cwnm.co.left;
				tr.top = w.br.top+cwnm.co.top;
				tr.right = w.width-cwnm.co.right;
				tr.bottom = -cwnm.co.bottom;
				if (w.imageindex>=0){
					sysimages.drawimage(w.imageindex,b,x+tr.left,y+tr.top+(tr.bottom-tr.top-sysimages.ih)/2);
					tr.left += sysimages.iw+2;
				};
				tflags = DTXT_VCENTER;
				if (fnm.caption.flags & SF_CENTER) tflags |= DTXT_CENTER;
				else if (fnm.caption.flags & SF_RIGHT) tflags |= DTXT_RIGHT;
				w.open(0);
				w.setfont(hskfont[0]);
				w.settextcolor((w.flags & MTCF_FOCUSED)?fnm.colors[SC_WINDOW_FOCUSED & 0xFFFF]:fnm.colors[SC_WINDOW_NORMAL & 0xFFFF]);
				w.drawtext(w.caption,-1,tr,tflags);
				w.close(0);
			};
			if (&rect) w.unclip();
		};
		break;
	case MTC_EDIT:
	case MTC_ITEMCOMBOBOX:
	case MTC_USERCOMBOBOX:
// Edit Box / Combo Box
		{
			MTEdit &e = *(MTEdit*)ctrl;
			int color;
			int ml,th;
			const char *mtext = e.text;
			MTRect r = {x+e.lblank+4,y+2,x+e.width-e.rblank-4,y+e.height-2};

			b->fill(x+rect.left,y+rect.top,rect.right-rect.left,rect.bottom-rect.top,fnm.colors[SC_EDIT_BACKGROUND & 0xFFFF]);
			b->open(0);
			color = fnm.colors[SC_EDIT_NORMAL & 0xFFFF];
			if (e.flags & (MTCF_OVER|MTCF_FOCUSED)) color = fnm.colors[SC_EDIT_FOCUSED & 0xFFFF];
			b->settextcolor(color);
			b->setfont(hskfont[3]);
			ml = strlen(e.text);
			if (e.password) mtext = passtxt;
			th = b->gettextheight();
			r.top = r.top+(r.bottom-r.top-th)/2;
			r.bottom = r.top+th;
			if ((e.selstart!=e.selend) && (e.flags & MTCF_FOCUSED)){
				int ss = e.mss;
				int se = e.mse;
				if (ss<e.offset) ss = e.offset;
				if (se>ss){
					MTPoint tss,tse;
					if (ss==e.offset) tss.x = 0;
					else{
						b->gettextsize(&mtext[e.offset],ss-e.offset,&tss);
						b->drawtext(&mtext[e.offset],ss-e.offset,r,0);
					};
					tss.x += e.lblank;
					b->gettextsize(&mtext[e.offset],se-e.offset,&tse);
					tse.x += e.lblank;
					if (tss.x<e.width-e.rblank){
						if (tse.x>=e.width-e.rblank) tse.x = e.width-e.rblank-1;
						b->setbrush(fnm.colors[SC_EDIT_SELECTION & 0xFFFF]);
						b->fillex(x+4+tss.x,r.top,tse.x-tss.x,r.bottom-r.top);
						r.left = x+4+tss.x;
						if (flags & MTCF_FOCUSED) b->settextcolor(fnm.colors[SC_EDIT_SELECTED & 0xFFFF]);
						b->drawtext(&mtext[ss],se-ss,r,0);
						if (tse.x<e.width-e.rblank-1){
							r.left = x+4+tse.x;
							b->settextcolor(color);
							b->drawtext(&mtext[se],ml-se,r,0);
						};
					};
				};
			}
			else{
				b->drawtext(&mtext[e.offset],ml-e.offset,r,0);
			};
			if ((e.flags & MTCF_FOCUSED) && (e.cursor>=e.offset) && (flags)){
				MTPoint ts;
				if (e.cursor==e.offset){
					ts.x = 0;
				}
				else{
					b->gettextsize(&mtext[e.offset],e.cursor-e.offset,&ts);
				};
				ts.x += e.lblank;
				if (ts.x<e.width-e.rblank){
					b->setpen(fnm.colors[SC_CURSOR & 0xFFFF]);
					b->moveto(x+4+ts.x,r.top);
					b->lineto(x+4+ts.x,r.bottom);
				};
			};
			b->close(0);
			if (e.guiid!=MTC_EDIT){
				MTComboBox &cb = *(MTComboBox*)ctrl;
				int imageindex;

				if (cb.lblank){
					if (cb.mlb->selected>=0){
						if (cb.getiteminfo(cb.mlb->selected,0,&imageindex,0,0)){
							sysimages.drawimage(imageindex,b,x+4,y+(e.height-sysimages.ih)/2);
						};
					};
				};
				if (e.rblank){
					b->skinblta(x+e.width-2-cbbm.b.w/2,y+e.height-2-cbbm.b.h,0,0,cbbm,2,1,(cb.mlb->flags & MTCF_HIDDEN)?0:1);
				};
			};
			drawframe(b,x,y,e.width,e.height);
		};
		break;
	case MTC_BUTTON:
//	Button
		{
			MTButton &bc = *(MTButton*)ctrl;
			int iw2,ih2;
			int bt,color,tflags;
			MTPoint size;

			MTRect r = {x+8,y,x+bc.width-8,y+bc.height};
			if (bc.flags & MTCF_DISABLED){
				bt = 4;
			}
			else if (bc.flags & MTCF_DOWN){
				bt = 3;
				r.left++;
				r.top++;
				r.right++;
			}
			else if ((bc.flags & MTCF_FOCUSED) && (flags)) bt = 2;
			else if (bc.flags & MTCF_OVER) bt = 1;
			else bt = 0;
			MTTabControl &ctb = *(MTTabControl*)bc.parent;
			if ((&ctb) && (ctb.guiid==MTC_TABCONTROL)){
				if (ctb.getpageid(ctb.page)==bc.tag) bt = 2;
				if ((ctb.parent) && (ctb.parent->guiid==MTC_DESKTOP)) drawbutton(b,x,y,bc.width,bc.height,&dtbtm[bt]);
				else drawbutton(b,x,y,bc.width,bc.height,&tbtm[bt]);
			}
			else drawbutton(b,x,y,bc.width,bc.height,&btm[bt]);
			if (bc.imageindex>=0){
				iw2 = 0;
				ih2 = (bc.height-sysimages.ih)/2;
				if (bc.caption[0]==0) iw2 = (bc.width-sysimages.iw)/2-8;
				x += 8+iw2;
				y += ih2;
				if (bc.flags & MTCF_DOWN){
					x++;
					y++;
				};
				sysimages.drawimage(bc.imageindex,b,x,y);
				r.left += sysimages.iw+2;
			};
			if (bc.caption[0]){
				tflags = DTXT_VCENTER;
				if ((&ctb) && (ctb.guiid==MTC_TABCONTROL)){
					color = fnm.colors[SC_TAB_NORMAL & 0xFFFF];
					if ((bc.flags & (MTCF_OVER|MTCF_DOWN)) || (ctb.getpageid(ctb.page)==bc.tag) || (bc.flags & MTCF_FOCUSED)) color = fnm.colors[SC_TAB_FOCUSED & 0xFFFF];
				}
				else{
					color = fnm.colors[SC_BUTTON_NORMAL & 0xFFFF];
					if (bc.flags & MTCF_DISABLED) color = fnm.colors[SC_BUTTON_DISABLED & 0xFFFF];
					else if ((bc.flags & (MTCF_OVER|MTCF_DOWN)) || ((bc.flags & MTCF_FOCUSED) && (flags))) color = fnm.colors[SC_BUTTON_FOCUSED & 0xFFFF];
				};
				if (fnm.button.flags & SF_CENTER) tflags |= DTXT_CENTER;
				else if (fnm.button.flags & SF_RIGHT) tflags |= DTXT_RIGHT;
				if (b->open(0)){
					b->setfont(hskfont[2]);
					b->settextcolor(color);
					b->drawtext(bc.caption,-1,r,tflags);
					if (bc.hotkeyoffset>=0){
						if (b->gettextsize(bc.caption,bc.hotkeyoffset,&size,r.right-r.left)){
							r.bottom = r.top+(r.bottom-r.top-b->gettextheight())/2+b->getbaseline()+1;
							size.y = b->getcharwidth(bc.hotkey);
							b->setpen(color);
							b->moveto(r.left+size.x,r.bottom);
							b->lineto(r.left+size.x+size.y,r.bottom);
						};
					};
					b->close(0);
				};
			};
		};
		break;
	case MTC_CHECKBOX:
//	Check box
		{
			MTCheckBoxData &cbd = *(MTCheckBoxData*)ctrl->skindata;
			MTCheckBox &cb = *(MTCheckBox*)ctrl;
			int w,ident,o;

			MTSkinPart &csp = (cb.radio)?rbm:cbm;
			ident = csp.b.w/3+4;
			MTRect r = {x+ident,y,x+cb.width,y+cb.height};
			w = csp.b.w/3;
			if ((guiprefs.animctrl) && (cbd.cstate!=(float)cb.state)){
				b->skinblta(x,y+(cb.height-csp.b.h)/2,w,csp.b.h,csp,3,1,cbd.fromstate);
				o = (int)(((cbd.cstate-(float)cbd.fromstate)/(cb.state-cbd.fromstate))*255.0);
				skinbmp[csp.bmpid]->blendblt(b,x,y+(cb.height-csp.b.h)/2,csp.b.w/3,csp.b.h,csp.b.x+cb.state*w,csp.b.y,o);
			}
			else{
				b->skinblta(x,y+(cb.height-csp.b.h)/2,w,csp.b.h,csp,3,1,cb.state);
			};
			if (b->open(0)){
				b->settextcolor(((cb.flags & MTCF_OVER) || ((cb.flags & MTCF_FOCUSED) && (gi->getcursorphase()>0)))?fnm.colors[SC_TEXT_FOCUSED & 0xFFFF]:fnm.colors[SC_TEXT_NORMAL & 0xFFFF]);
				b->setfont(hskfont[1]);
				b->drawtext(cb.caption,-1,r,DTXT_VCENTER);
				b->close(0);
			};
		};
		break;
	case MTC_PROGRESS:
//	Progress bar
		{
			drawslider(b,x,y,ctrl->width,ctrl->height,0,0,((MTProgress*)ctrl)->maxpos,((MTProgress*)ctrl)->pos,false);
		};
		break;
	case MTC_SLIDER:
//	Slider
		{
			MTSlider &s = *(MTSlider*)ctrl;
			int ox,oy,ow,oh;
			MTSLMetrics *cm = getslider(s.type,s.orientation);

			switch (cm->type){
			case SKIN_ANIM:
				ow = cm->c.a.b.w/cm->c.nx;
				oh = cm->c.a.b.h/cm->c.ny;
				ox = (s.width-ow)/2;
				oy = (s.height-oh)/2;
				break;
			case SKIN_CIRCLE:
				ow = cm->d.s.b.w;
				oh = cm->d.s.b.h;
				ox = (s.width-ow)/2;
				oy = (s.height-oh)/2;
				break;
			default:
				ox = 0;
				oy = 0;
				ow = s.width;
				oh = s.height;
				break;
			};
			drawslider(b,x+ox,y+oy,ow,oh,cm,s.minpos,s.maxpos,s.value,(s.flags & MTCF_OVER) || ((s.flags & MTCF_FOCUSED) && (gi->getcursorphase()>0)));
		};
		break;
	};
}

void MTSkin::drawcontrol(int guiid,int id,MTRect &rect,MTBitmap *b,int x,int y,int flags)
{
	switch (guiid){
	case MTC_STATUS:
		{
			if (id<3){
				b->skinblta(x,y,sbm.b.h/2,sbm.b.h/2,sbm,6,2,id+6*flags);
			}
			else{
				b->skinblta(x,y,sbm.b.w-(sbm.b.h*3)/2,sbm.b.h/2,sbm,2,2,1);
			};
		};
		break;
	case MTC_SEQUENCE:
		{
			drawbutton(b,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,&ptm[id]);
		};
		break;
	};
}

void MTSkin::drawborder(MTWinControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y)
{
	drawframe(b,x,y,ctrl->width+4,ctrl->height+4,(ctrl->flags & MTCF_RAISED)?1:0);
}

void MTSkin::drawmodalveil(MTWinControl *ctrl,MTRect &rect)
{
	ctrl->fillcolor(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,0,64);
}

void MTSkin::drawdragbkg(MTBitmap *b,MTRect &rect,int style)
{
	b->skinblt(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,wnm[style & 0xF].bkg);
}

void MTSkin::updatemetrics()
{
	MTSQMetrics &csq = fnm.pattfont;

	fontwidth = csq.a.b.w/csq.nx;
	fontheight = csq.a.b.h/csq.ny;
}

MTBitmap* MTSkin::getbitmap(int id)
{
	return skinbmp[id];
}

MTSLMetrics* MTSkin::getslider(int type,int orientation)
{
	if (type==1) return &knm;
	else if (type==2) return &vsm;
	else if (type==3) return &vum;
	else return (orientation==0)?&hslm:&vslm;
}

void MTSkin::getcontrolsize(int guiid,int id,int &width,int &height)
{
	switch (guiid){
	case MTC_SIGN:
		width = animm[id].a.b.w/animm[id].nx;
		height = animm[id].a.b.h/animm[id].ny;
		break;
	case MTC_SCROLLER:
		switch (id){
		case 0:	// Horizontal
			height = arm.b.h/2;
			break;
		case 1:	// Vertical
			width = arm.b.h/2;
			break;
		// Minimum slider size
		case 2:	// Horizontal
			width = vscbm.tl.b.w+vscbm.br.b.w;
			break;
		case 3:	// Vertical
			height = vscbm.tl.b.h+vscbm.br.b.h;
			break;
		// Button size
		case 4:	// Horizontal
			width = height = arm.b.h/2;
			break;
		case 5:	// Vertical
			width = height = arm.b.h/2;
			break;
		};
		break;
	case MTC_WINDOW:
		switch (id){
		case 16:
			width = dbtns.a.b.w/dbtns.nx;
			height = dbtns.a.b.h/dbtns.ny;
			break;
		case 17:
			width = wbtns.a.b.w/wbtns.nx;
			height = wbtns.a.b.h/wbtns.ny;
			break;
		case 18:
			width = mbtns.a.b.w/mbtns.nx;
			height = mbtns.a.b.h/mbtns.ny;
			break;
		default:
			width = wnm[id].btno.x;
			height = wnm[id].btno.y;
			break;
		};
		break;
	case MTC_ITEMCOMBOBOX:
	case MTC_USERCOMBOBOX:
		if (id==1) width = cbbm.b.w/2;
		break;
	case MTC_PROGRESS:
		switch (prm.type){
		case SKIN_BUTTON:
		case SKIN_PROGR:
			height = prm.a.s.b.h;
			break;
		case SKIN_ANIM:
			height = prm.c.a.b.h/prm.c.ny;
			break;
		};
		break;
	case MTC_STATUS:
		width = sbm.b.w-(sbm.b.h*3)/2;	// Vu-meter size
		height = sbm.b.h/2;
		break;
	};
}

void MTSkin::getwindowborders(int style,MTRect *borders)
{
	*borders = br[style];
}

void MTSkin::getwindowoffsets(int style,MTRect *borders)
{
	*borders = wnm[style].co;
}

void MTSkin::getregions(MTControl *ctrl,void **opaque,void **transparent)
{
	void *oprgn,*trrgn;
	void *op;
	int b;
	MTRect r = {0,0,ctrl->width,ctrl->height};

	oprgn = recttorgn(r);
	if (ctrl->guiid!=MTC_WINDOW){
		r.right = r.bottom = 0;
		if (opaque) *opaque = oprgn;
		if (transparent) *transparent = recttorgn(r);
		return;
	};
	MTWindow &w = *(MTWindow*)ctrl;
	MTWNMetrics &m = wnm[w.style & 0xFF];
	r.left += w.br.left;
	r.top += w.br.top;
	r.right += w.br.right;
	r.bottom += w.br.bottom;
	trrgn = recttorgn(r);
	if ((m.tl.flags & SKIN_TRANSP)==0){
		r.left = -m.tl.b.w+m.tlo.x;
		r.top = -m.tl.b.h+m.tlo.y;
		r.right = r.left+m.tl.b.w;
		r.bottom = r.top+m.tl.b.h;
		op = recttorgn(r);
		addrgn(oprgn,op);
		deletergn(op);
	};
	if ((m.t.flags & SKIN_TRANSP)==0){
		b = w.width-m.tlo.x-m.tro.x;
		if (b){
			r.left = m.tlo.x;
			r.top = -m.t.b.h+m.to;
			r.right = r.left+b;
			r.bottom = r.top+m.t.b.h;
			op = recttorgn(r);
			addrgn(oprgn,op);
			deletergn(op);
		};
	};
	if ((m.tr.flags & SKIN_TRANSP)==0){
		r.left = w.width-m.tro.x;
		r.top = -m.tr.b.h+m.tro.y;
		r.right = r.left+m.tr.b.w;
		r.bottom = r.top+m.tr.b.h;
		op = recttorgn(r);
		addrgn(oprgn,op);
		deletergn(op);
	};
	if ((m.bl.flags & SKIN_TRANSP)==0){
		r.left = -m.bl.b.w+m.blo.x;
		r.top = w.height-m.blo.y;
		r.right = r.left+m.bl.b.w;
		r.bottom = r.top+m.bl.b.h;
		op = recttorgn(r);
		addrgn(oprgn,op);
		deletergn(op);
	};
	if ((m.b.flags & SKIN_TRANSP)==0){
		b = w.width-m.blo.x-m.bro.x;
		b = w.width-m.tlo.x-m.tro.x;
		if (b){
			r.left = m.blo.x;
			r.top = w.height-m.bo;
			r.right = r.left+b;
			r.bottom = r.top+m.b.b.h;
			op = recttorgn(r);
			addrgn(oprgn,op);
			deletergn(op);
		};
	};
	if ((m.br.flags & SKIN_TRANSP)==0){
		r.left = w.width-m.bro.x;
		r.top = w.height-m.bro.y;
		r.right = r.left+m.br.b.w;
		r.bottom = r.top+m.br.b.h;
		op = recttorgn(r);
		addrgn(oprgn,op);
		deletergn(op);
	};
	if ((m.l.flags & SKIN_TRANSP)==0){
		b = w.height-m.tlo.y-m.blo.y;
		if (b){
			r.left = -m.l.b.w+m.lo;
			r.top = m.tlo.y;
			r.right = r.left+m.l.b.w;
			r.bottom = r.top+b;
			op = recttorgn(r);
			addrgn(oprgn,op);
			deletergn(op);
		};
	};
	if ((m.r.flags & SKIN_TRANSP)==0){
		b = w.height-m.tro.y-m.bro.y;
		if (b){
			r.left = w.width-m.ro;
			r.top = m.tro.y;
			r.right = r.left+m.r.b.w;
			r.bottom = r.top+b;
			op = recttorgn(r);
			addrgn(oprgn,op);
			deletergn(op);
		};
	};
	subtractrgn(trrgn,oprgn);
	if (opaque) *opaque = oprgn;
	if (transparent) *transparent = trrgn;
}

int MTSkin::getcolor(int id)
{
	if (id<0xFF000000) return id;
	return fnm.colors[id-0xFF000000];
}

void* MTSkin::getfont(int id)
{
	return hskfont[id];
}

bool MTSkin::gettextsize(MTControl *ctrl,const char *text,int length,MTPoint *size,int maxwidth)
{
	bool ok = false;

	if (ctrl->parent->open(0)){
		if (ctrl->guiid==MTC_BUTTON) ctrl->parent->setfont(hskfont[2]);
		else ctrl->parent->setfont(hskfont[1]);
		switch (ctrl->guiid){
		case MTC_EDIT:
		case MTC_ITEMCOMBOBOX:
		case MTC_USERCOMBOBOX:
			{
				if (!text) ok = ctrl->parent->gettextsize(passtxt,length,size,maxwidth);
				else ok = ctrl->parent->gettextsize(text,length,size,maxwidth);
			}
			break;
		default:
			ok = ctrl->parent->gettextsize(text,length,size,maxwidth);
			break;
		};
		ctrl->parent->close(0);
	};
	return ok;
}

int MTSkin::gettextextent(MTControl *ctrl,const char *text,int length,int maxextent)
{
	int res = 0;

	if (ctrl->parent->open(0)){
		if (ctrl->guiid==MTC_BUTTON) ctrl->parent->setfont(hskfont[2]);
		else ctrl->parent->setfont(hskfont[1]);
		switch (ctrl->guiid){
		case MTC_EDIT:
		case MTC_ITEMCOMBOBOX:
		case MTC_USERCOMBOBOX:
			{
				if (!text) res = ctrl->parent->gettextextent(passtxt,length,maxextent);
				else res = ctrl->parent->gettextextent(text,length,maxextent);
			}
			break;
		default:
			res = ctrl->parent->gettextextent(text,length,maxextent);
			break;
		};
		ctrl->parent->close(0);
	};
	return res;
}

int MTSkin::gettextheight(MTControl *ctrl)
{
	int res = 0;

	if (ctrl->parent->open(0)){
		if (ctrl->guiid==MTC_BUTTON) ctrl->parent->setfont(hskfont[2]);
		else ctrl->parent->setfont(hskfont[1]);
		res = ctrl->parent->gettextheight();
		ctrl->parent->close(0);
	};
	return res;
}

int MTSkin::getcharwidth(MTControl *ctrl,char c)
{
	int res = 0;

	if (ctrl->parent->open(0)){
		if (ctrl->guiid==MTC_BUTTON) ctrl->parent->setfont(hskfont[2]);
		else ctrl->parent->setfont(hskfont[1]);
		res = ctrl->parent->getcharwidth(c);
		ctrl->parent->close(0);
	};
	return res;
}

void MTSkin::drawitem(MTUserList *list,int i,MTRect &rect,MTBitmap *b,const char *caption,int imageindex,int itemflags,bool editable)
{
	int mid,l,color;
	MTPoint p;
	MTRect r;

	r = rect;
	if (caption[0]=='|'){
		mid = (r.top+r.bottom)/2;
		r.left = rect.left+18;
		r.top = rect.top;
		if (b->open(0)){
			b->setbrush(fnm.colors[SC_BACKGROUND & 0xFFFF]);
			b->setpen(fnm.colors[SC_CTRL_S & 0xFFFF]);
			b->moveto(r.left,mid-1);
			b->lineto(r.right,mid-1);
			b->setpen(fnm.colors[SC_CTRL_L & 0xFFFF]);
			b->moveto(r.left,mid);
			b->lineto(r.right,mid);
			r.left += 18;
			b->gettextsize(&caption[1],-1,(MTPoint*)&r.right);
			r.right += r.left;
			r.bottom += r.top;
			if (r.right>rect.right-2) r.right = rect.right-2;
			if (r.bottom>rect.bottom) r.bottom = rect.bottom;
			b->fillex(r.left,r.top,r.right-r.left,r.bottom-r.top);
			b->settextcolor(fnm.colors[SC_TEXT_NORMAL & 0xFFFF]);
			b->setfont(hskfont[1]);
			l = b->gettextextent(&caption[1],-1,r.right-r.left);
			if (l<strlen(&caption[1])){
				if (b->gettextsize("...",-1,&p)){
					l = b->gettextextent(&caption[1],-1,r.right-r.left-p.x);
				}
				else{
					l -= 3;
				};
			}
			else l = -1;
			b->drawtext(&caption[1],l,r,DTXT_VCENTER);
			if (l>0){
				if (b->gettextsize(&caption[1],l,&p)){
					r.left += p.x;
					b->drawtext("...",-1,r,DTXT_VCENTER);
				};
			};
			b->close(0);
		};
	}
	else{
		b->fill(r.left,r.top,r.right-r.left,r.bottom-r.top,(i==list->selected)?fnm.colors[SC_EDIT_SELECTION & 0xFFFF]:fnm.colors[SC_EDIT_BACKGROUND & 0xFFFF]);
		if (list->viewflags & MTVF_IMAGES){
			sysimages.drawimage(imageindex,b,r.left,r.top);
			r.left += 18;
		};
		if (i==list->over) color = fnm.colors[SC_TEXT_FOCUSED & 0xFFFF];
		else if (i==list->selected) color = fnm.colors[SC_EDIT_SELECTED & 0xFFFF];
		else color = fnm.colors[SC_TEXT_NORMAL & 0xFFFF];
		if (b->open(0)){
			b->settextcolor(color);
			b->setfont(hskfont[1]);
			l = b->gettextextent(caption,-1,r.right-r.left);
			if (l<strlen(caption)){
				if (b->gettextsize("...",-1,&p)){
					l = b->gettextextent(caption,-1,r.right-r.left-p.x);
				}
				else{
					l -= 3;
				};
			}
			else l = -1;
			b->drawtext(caption,l,r,DTXT_VCENTER);
			if (l>0){
				if (b->gettextsize(caption,l,&p)){
					r.left += p.x;
					b->drawtext("...",-1,r,DTXT_VCENTER);
				};
			};
			b->close(0);
		};
	};
}

void MTSkin::drawchar(unsigned char c,MTBitmap *bmp,int &x,int y,int color)
{
	MTSQMetrics &csq = fnm.pattfont;

	bmp->skinblta(x,y,fontwidth,fontheight,csq.a,csq.nx,csq.ny,fontmap[c],color);
	x += fontwidth;
}

void MTSkin::drawtext(unsigned char *text,MTBitmap *bmp,int &x,int y,int color)
{
	unsigned char *p = text;
	MTSQMetrics &csq = fnm.pattfont;

	while (*p){
		bmp->skinblta(x,y,fontwidth,fontheight,csq.a,csq.nx,csq.ny,fontmap[*p],color);
		x += fontwidth;
		p++;
	};
}

void MTSkin::drawdec(int val,bool zeroes,int n,MTBitmap *bmp,int &x,int y,int color)
{
	int z,x2;
	char c;
	MTSQMetrics &csq = fnm.pattfont;
	
	x += fontwidth*n;
	x2 = x-fontwidth;
	for (z=0;z<n;z++){
		c = val%10;
		val /= 10;
		bmp->skinblta(x2,y,fontwidth,fontheight,csq.a,csq.nx,csq.ny,fontmap[c+'0'],color);
		if ((!val) && (!zeroes)) return;
		x2 -= fontwidth;
	};
}

void MTSkin::drawhex(int val,bool zeroes,int n,MTBitmap *bmp,int &x,int y,int color)
{
	int z,x2;
	char c;
	MTSQMetrics &csq = fnm.pattfont;
	
	x += fontwidth*n;
	x2 = x-fontwidth;
	for (z=0;z<n;z++){
		c = val%16;
		val >>= 4;
		if (c<10) c += '0';
		else c += 'A'-10;
		bmp->skinblta(x2,y,fontwidth,fontheight,csq.a,csq.nx,csq.ny,fontmap[c],color);
		if ((!val) && (!zeroes)) return;
		x2 -= fontwidth;
	};
}

void MTSkin::drawshadow(MTBitmap *bmp,int x,int y,int w,int h,int style)
{
	int b,cx,cy;
	void *rgn,*op;
	MTWNMetrics &m = wnm[style];
	MTMask &cmask = *mask[style];
	MTRect *cmr = mr[style];
	MTRect r = {x,y,x+w,y+h};
	
	rgn = copyrgn(bmp->getclip());
	op = recttorgn(r);
	subtractrgn(rgn,op);
	deletergn(op);
	bmp->cliprgn(rgn);
	deletergn(rgn);
	x -= 8-guiprefs.shadowx;
	y -= 8-guiprefs.shadowy;
	b = m.tl.b.w-m.tlo.x;
	cx = m.bl.b.w-m.blo.x;
	if (cx>b) b = cx;
	cx = m.l.b.w-m.lo;
	if (cx>b) b = cx;
	r.left = x-b;
	b = m.tl.b.h-m.tlo.y;
	cx = m.t.b.h-m.to;
	if (cx>b) cx = b;
	cx = m.tr.b.h-m.tro.y;
	if (cx>b) cx = b;
	r.top = y-b;
	b = -m.tro.x+m.tr.b.w+20;
	cx = -m.bro.x+m.br.b.w+20;
	if (cx>b) b = cx;
	cx = -m.ro+m.r.b.w+20;
	if (cx>b) b = cx;
	r.right = x+w+b;
	b = -m.blo.y+m.bl.b.h+20;
	cx = -m.bo+m.b.b.h+20;
	if (cx>b) b = cx;
	cx = -m.bro.y+m.br.b.h+20;
	if (cx>b) b = cx;
	r.bottom = y+h+b;
//	bmp->openbits(r,&tmp,&b);
	// Top-left
	bmp->shade(x-m.tl.b.w+m.tlo.x,y-m.tl.b.h+m.tlo.y,m.tl.b.w,m.tl.b.h,&cmask,cmr[0].left,cmr[0].top);
	// Top
	b = w-m.tlo.x-m.tro.x;
	if (b){
		cx = x+m.tlo.x;
		cy = y-m.t.b.h+m.to;
		if (m.t.mode==SKIN_STRETCH)
			bmp->sshade(cx,cy,b,m.t.b.h,&cmask,cmr[1].left,cmr[1].top,m.t.b.w,m.t.b.h);
		else
			bmp->tshade(cx,cy,b,m.t.b.h,&cmask,cmr[1].left,cmr[1].top,m.t.b.w,m.t.b.h);
	};
	// Top-right
	bmp->shade(x+w-m.tro.x,y-m.tr.b.h+m.tro.y,m.tr.b.w+20,m.tr.b.h,&cmask,cmr[2].left,cmr[2].top);
	// Bottom-left
	bmp->shade(x-m.bl.b.w+m.blo.x,y+h-m.blo.y,m.bl.b.w,m.bl.b.h+20,&cmask,cmr[3].left,cmr[3].top);
	// Bottom
	b = w-m.blo.x-m.bro.x;
	if (b){
		cx = x+m.blo.x;
		cy = y+h-m.bo;
		if (m.b.mode==SKIN_STRETCH)
			bmp->sshade(cx,cy,b,m.b.b.h+20,&cmask,cmr[4].left,cmr[4].top,m.b.b.w,m.b.b.h+20);
		else
			bmp->tshade(cx,cy,b,m.b.b.h+20,&cmask,cmr[4].left,cmr[4].top,m.b.b.w,m.b.b.h+20);
	};
	// Bottom-Right
	bmp->shade(x+w-m.bro.x,y+h-m.bro.y,m.br.b.w+20,m.br.b.h+20,&cmask,cmr[5].left,cmr[5].top);
	// Left
	b = h-m.tlo.y-m.blo.y;
	if (b){
		cx = x-m.l.b.w+m.lo;
		cy = y+m.tlo.y;
		if (m.l.mode==SKIN_STRETCH)
			bmp->sshade(cx,cy,m.l.b.w,b,&cmask,cmr[6].left,cmr[7].top,m.l.b.w,m.l.b.h);
		else
			bmp->tshade(cx,cy,m.l.b.w,b,&cmask,cmr[6].left,cmr[7].top,m.l.b.w,m.l.b.h);
	};
	// Right
	b = h-m.tro.y-m.bro.y;
	if (b){
		cx = x+w-m.ro;
		cy = y+m.tro.y;
		if (m.r.mode==SKIN_STRETCH)
			bmp->sshade(cx,cy,m.r.b.w+20,b,&cmask,cmr[7].left,cmr[7].top,m.r.b.w+20,m.r.b.h);
		else
			bmp->tshade(cx,cy,m.r.b.w+20,b,&cmask,cmr[7].left,cmr[7].top,m.r.b.w+20,m.r.b.h);
	};
//	bmp->closebits();
	bmp->unclip();
}

void MTSkin::calcbounds(int m)
{
	MTWNMetrics &cwnm = wnm[m];
	int x;

	br[m].left = -cwnm.tl.b.w+cwnm.tlo.x;
	x = -cwnm.l.b.w+cwnm.lo;
	if (x<br[m].left) br[m].left = x;
	x = -cwnm.bl.b.w+cwnm.blo.x;
	if (x<br[m].left) br[m].left = x;
	br[m].top = -cwnm.tl.b.h+cwnm.tlo.y;
	x = -cwnm.t.b.h+cwnm.to;
	if (x<br[m].top) br[m].top = x;
	x = -cwnm.tr.b.h+cwnm.tro.y;
	if (x<br[m].top) br[m].top = x;
	br[m].right = cwnm.tr.b.w-cwnm.tro.x;
	x = cwnm.r.b.w-cwnm.ro;
	if (x>br[m].right) br[m].right = x;
	x = cwnm.br.b.w-cwnm.bro.x;
	if (x>br[m].right) br[m].right = x;
	br[m].bottom = cwnm.bl.b.h-cwnm.bro.y;
	x = cwnm.b.b.h-cwnm.bo;
	if (x>br[m].bottom) br[m].bottom = x;
	x = cwnm.br.b.h-cwnm.bro.y;
	if (x>br[m].bottom) br[m].bottom = x;
}

void MTSkin::drawwindow(MTBitmap *bmp,int x,int y,int w,int h,int style)
{
	MTWNMetrics &m = wnm[style];
	int b;
	
	if ((guiprefs.shadows) && (mask[style])) drawshadow(bmp,x,y,w,h,style);
	// Top-left
	bmp->skinblt(x-m.tl.b.w+m.tlo.x,y-m.tl.b.h+m.tlo.y,0,0,m.tl);
	// Top
	b = w-m.tlo.x-m.tro.x;
	if (b) bmp->skinblt(x+m.tlo.x,y-m.t.b.h+m.to,b,0,m.t);
	// Top-right
	bmp->skinblt(x+w-m.tro.x,y-m.tr.b.h+m.tro.y,0,0,m.tr);
	// Bottom-left
	bmp->skinblt(x-m.bl.b.w+m.blo.x,y+h-m.blo.y,0,0,m.bl);
	// Bottom
	b = w-m.blo.x-m.bro.x;
	if (b) bmp->skinblt(x+m.blo.x,y+h-m.bo,b,0,m.b);
	// Bottom-right
	bmp->skinblt(x+w-m.bro.x,y+h-m.bro.y,0,0,m.br);
	// Left
	b = h-m.tlo.y-m.blo.y;
	if (b) bmp->skinblt(x-m.l.b.w+m.lo,y+m.tlo.y,0,b,m.l);
	// Right
	b = h-m.tro.y-m.bro.y;
	if (b) bmp->skinblt(x+w-m.ro,y+m.tro.y,0,b,m.r);
}

void MTSkin::drawinnerwindow(MTBitmap *bmp,int x,int y,int w,int h,int style)
{
	drawwindow(bmp,x-br[style].left,y-br[style].top,w+br[style].left-br[style].right,h+br[style].top-br[style].bottom,style);
}

void MTSkin::drawbutton(MTBitmap *bmp,int x,int y,int w,int h,MTBTMetrics *m)
{
	unsigned short ox1,oy1,ox2,oy2,bw,bh;
	
	ox1 = m->tl.b.w;
	if (m->l.b.w>ox1) ox1 = m->l.b.w;
	oy1 = m->tl.b.h;
	ox2 = m->tr.b.w;
	if (m->r.b.w>ox2) ox2 = m->r.b.w;
	oy2 = m->bl.b.h;
	if (m->t.mode==SKIN_FIXED){
		if (m->t.b.w){
			if (w<ox1+ox2+4){
				ox1 = w/3;
				ox2 = ox1;
			};
		}
		else if ((ox1) && (ox1+ox2>h)){
			ox1 = w/2;
			ox2 = w-ox1;
		};
	};
	if (m->l.mode==SKIN_FIXED){
		if (m->l.b.h){
			if (h<oy1+oy2+4){
				oy1 = h/3;
				oy2 = oy1;
			};
		}
		else if ((oy1) && (oy1+oy2>w)){
			oy1 = h/2;
			oy2 = h-oy1;
		};
	};
	bw = w-ox1-ox2;
	bh = h-oy1-oy2;
	// Top-left
	bmp->skinblt(x,y,ox1,oy1,m->tl);
	// Top-right
	bmp->skinblt(x+w-ox2,y,ox2,oy1,m->tr);
	// Bottom-left
	bmp->skinblt(x,y+h-oy2,ox1,oy2,m->bl);
	// Bottom-right
	bmp->skinblt(x+w-ox2,y+h-oy2,ox2,oy2,m->br);
	if (bw){
		// Top
		bmp->skinblt(x+ox1,y,bw,oy1,m->t);
		// Bottom
		bmp->skinblt(x+ox1,y+h-oy2,bw,oy2,m->b);
	};
	if (bh){
		// Left
		bmp->skinblt(x,y+oy1,ox1,bh,m->l);
		// Right
		bmp->skinblt(x+w-ox1,y+oy1,ox2,bh,m->r);
	};
	// Background
	if ((bw) && (bh)) bmp->skinblt(x+ox1,y+oy1,bw,bh,m->bkg);
}

void MTSkin::drawslider(MTBitmap *bmp,int x,int y,int w,int h,MTSLMetrics *m,int min,int max,int pos,bool focused)
{
	int o1,o2,b,cx,cy;
	double a;
	
	if (!m) m = &prm;
	switch (m->type){
	case SKIN_BUTTON:
		bmp->skinblt(x,y,w,h,m->a.s);
		if (m->orientation==SKIN_HORIZ){
			b = w-m->a.sa.right-m->a.sa.left-m->a.b.b.w;
			b = b-((pos-min)*b)/(max-min);
			bmp->skinblt(x+m->a.sa.left+b,y+m->a.sa.top,0,0,(focused)?m->a.fb:m->a.b);
		}
		else{
			b = h-m->a.sa.bottom-m->a.sa.top-m->a.b.b.h;
			b = b-((pos-min)*b)/(max-min);
			bmp->skinblt(x+m->a.sa.left,y+m->a.sa.top+b,0,0,(focused)?m->a.fb:m->a.b);
		};
		break;
	case SKIN_XBUTTON:
		drawbutton(bmp,x,y,w,h,&m->xa.s);
		if (m->orientation==SKIN_HORIZ){
			b = w-m->xa.sa.right-m->xa.sa.left-m->xa.b.b.w;
			b = b-((pos-min)*b)/(max-min);
			bmp->skinblt(x+m->xa.sa.left+b,y+m->xa.sa.top,0,0,(focused)?m->xa.fb:m->xa.b);
		}
		else{
			b = h-m->xa.sa.bottom-m->xa.sa.top-m->xa.b.b.h;
			b = b-((pos-min)*b)/(max-min);
			bmp->skinblt(x+m->xa.sa.left,y+m->xa.sa.top+b,0,0,(focused)?m->xa.fb:m->xa.b);
		};
		break;
	case SKIN_PROGR:
		bmp->skinblt(x,y,w,h,m->b.s);
		x += m->b.sa.left;
		y += m->b.sa.top;
		w -= (m->b.sa.left+m->b.sa.right);
		h -= (m->b.sa.top+m->b.sa.bottom);
		if (m->orientation==SKIN_HORIZ){
			o1 = m->b.ps.b.w;
			o2 = m->b.pe.b.w;
			b = ((pos-min)*w)/(max-min);
			if (!b) break;
			if ((o1) && (o2)){
				if (b<o1+o2+4){
					o1 = b/3;
					o2 = o1;
				};
			};
			b = b-o1-o2;
			bmp->skinblt(x,y,o1,h,m->b.ps);
			bmp->skinblt(x+b+o1,y,o2,h,m->b.pe);
			if (b>0) bmp->skinblt(x+o1,y,b,h,m->b.pm);
		}
		else{
			o1 = m->b.ps.b.h;
			o2 = m->b.pe.b.h;
			b = ((pos-min)*h)/(max-min);
			if ((o1) && (o2)){
				if (b<o1+o2+4){
					o1 = b/3;
					o2 = o1;
				};
			};
			b = b-o1-o2;
			bmp->skinblt(x,y+h-o1,w,o1,m->b.ps);
			bmp->skinblt(x,y+h-o1-b-o2,w,o2,m->b.pe);
			if (b) bmp->skinblt(x,y+h-o1-b,w,b,m->b.pm);
		};
		break;
	case SKIN_XPROGR:
		drawbutton(bmp,x,y,w,h,&m->xb.s);
		x += m->xb.sa.left;
		y += m->xb.sa.top;
		w -= (m->xb.sa.left+m->xb.sa.right);
		h -= (m->xb.sa.top+m->xb.sa.bottom);
		if (m->orientation==SKIN_HORIZ){
			o1 = m->xb.ps.b.w;
			o2 = m->xb.pe.b.w;
			b = ((pos-min)*w)/(max-min);
			if (!b) break;
			if ((o1) && (o2)){
				if (b<o1+o2+4){
					o1 = b/3;
					o2 = o1;
				};
			};
			b = b-o1-o2;
			bmp->skinblt(x,y,o1,h,m->xb.ps);
			bmp->skinblt(x+b+o1,y,o2,h,m->xb.pe);
			if (b>0) bmp->skinblt(x+o1,y,b,h,m->xb.pm);
		}
		else{
			o1 = m->xb.ps.b.h;
			o2 = m->xb.pe.b.h;
			b = ((pos-min)*h)/(max-min);
			if ((o1) && (o2)){
				if (b<o1+o2+4){
					o1 = b/3;
					o2 = o1;
				};
			};
			b = b-o1-o2;
			bmp->skinblt(x,y+h-o1,w,o1,m->xb.ps);
			bmp->skinblt(x,y+h-o1-b-o2,w,o2,m->xb.pe);
			if (b>0) bmp->skinblt(x,y+h-o1-b,w,b,m->xb.pm);
		};
		break;
	case SKIN_ANIM:
		b = m->c.nx*m->c.ny-1;
		b = ((pos-min)*b)/(max-min);
		bmp->skinblta(x,y,w,h,m->c.a,m->c.nx,m->c.ny,b);
		break;
	case SKIN_CIRCLE:
		a = (double)(pos-min)/(double)(max-min);
		a = (1-a)*4.7123889803846898576939650749193-0.78539816339744830961566084581988;
		bmp->skinblt(x,y,w,h,m->d.s);
		if (focused){
			cx = m->d.cp.x+(int)((double)m->d.r*cos(a))-m->d.fb.b.w/2;;
			cy = m->d.cp.y-(int)((double)m->d.r*sin(a))-m->d.b.b.h/2;;
			bmp->skinblt(x+cx,y+cy,0,0,m->d.fb);
		}
		else{
			cx = m->d.cp.x+(int)((double)m->d.r*cos(a))-m->d.b.b.w/2;
			cy = m->d.cp.y-(int)((double)m->d.r*sin(a))-m->d.b.b.h/2;
			bmp->skinblt(x+cx,y+cy,0,0,m->d.b);
		};
		break;
	case SKIN_VUMETER:
		bmp->skinblt(x,y,w,h,m->e.s);
		b = ((pos-min)*m->e.v.b.h)/(max-min);
		skinbmp[m->e.v.bmpid]->blt(bmp,x+m->e.sa.left,y+h-m->e.sa.bottom-b,m->e.v.b.w,b,m->e.v.b.x,m->e.v.b.y+m->e.v.b.h-b);
		break;
 };
}

void MTSkin::drawframe(MTBitmap *bmp,int x,int y,int w,int h,int flags)
{
	if (flags==0){
		bmp->fill(x,y,w,1,fnm.colors[SC_CTRL_S & 0xFFFF]);
		bmp->fill(x,y,1,h,fnm.colors[SC_CTRL_S & 0xFFFF]);
		bmp->fill(x+1,y+h-1,w-1,1,fnm.colors[SC_CTRL_HL & 0xFFFF]);
		bmp->fill(x+w-1,y+1,1,h-1,fnm.colors[SC_CTRL_HL & 0xFFFF]);
		bmp->fill(x+1,y+1,w-2,1,fnm.colors[SC_CTRL_DS & 0xFFFF]);
		bmp->fill(x+1,y+1,1,h-2,fnm.colors[SC_CTRL_DS & 0xFFFF]);
		bmp->fill(x+2,y+h-2,w-3,1,fnm.colors[SC_CTRL_L & 0xFFFF]);
		bmp->fill(x+w-2,y+2,1,h-3,fnm.colors[SC_CTRL_L & 0xFFFF]);
	}
	else if (flags==1){
		bmp->fill(x,y,w,1,fnm.colors[SC_CTRL_HL & 0xFFFF]);
		bmp->fill(x,y,1,h,fnm.colors[SC_CTRL_HL & 0xFFFF]);
		bmp->fill(x+1,y+h-1,w-1,1,fnm.colors[SC_CTRL_DS & 0xFFFF]);
		bmp->fill(x+w-1,y+1,1,h-1,fnm.colors[SC_CTRL_DS & 0xFFFF]);
		bmp->fill(x+1,y+1,w-2,1,fnm.colors[SC_CTRL_L & 0xFFFF]);
		bmp->fill(x+1,y+1,1,h-2,fnm.colors[SC_CTRL_L & 0xFFFF]);
		bmp->fill(x+2,y+h-2,w-3,1,fnm.colors[SC_CTRL_S & 0xFFFF]);
		bmp->fill(x+w-2,y+2,1,h-3,fnm.colors[SC_CTRL_S & 0xFFFF]);
	};
}

void MTSkin::setfonts()
{
	hskfont[0] = createfont(fnm.caption.name,fnm.caption.size,(fnm.caption.flags & SF_BOLD)!=0);
	hskfont[1] = createfont(fnm.text.name,fnm.text.size,(fnm.text.flags & SF_BOLD)!=0);
	hskfont[2] = createfont(fnm.button.name,fnm.button.size,(fnm.button.flags & SF_BOLD)!=0);
	hskfont[3] = createfont(fnm.edit.name,fnm.edit.size,(fnm.edit.flags & SF_BOLD)!=0);
}

void MTSkin::delfonts()
{
	deletefont(hskfont[0]);
	deletefont(hskfont[1]);
	deletefont(hskfont[2]);
	deletefont(hskfont[3]);
}

void MTSkin::setshadows(bool enabled)
{
	int x,w,h,t,ox1,oy1,ox2,oy2;
	
	ENTER("MTSkin::setshadows");
	delshadows();
	if (!enabled){
		LEAVE();
		return;
	};
	for (x=0;x<8;x++){
		if ((x==5) || (x==6) || (x==7)) continue;
		MTWNMetrics &m = wnm[x];
		MTMask *cmask;
		MTRect *cr = mr[x];
		ox1 = m.tl.b.w-m.tlo.x;
		oy1 = m.tl.b.h-m.tlo.y;
		ox2 = m.br.b.w-m.bro.x;
		oy2 = m.br.b.h-m.bro.y;
		w = m.t.b.w*4+m.tlo.x+m.tro.x+ox1+ox2;
		t = m.b.b.w*4+m.blo.x+m.bro.x+ox1+ox2;
		if (t>w) w = t;
		h = m.l.b.h*4+m.tlo.y+m.blo.y+oy1+oy2;
		t = m.r.b.h*4+m.tro.y+m.bro.y+oy1+oy2;
		if (t>h) h = t;
		cmask = di->newmask(w+32,h+32);
		// Top-left
		cr[0].left = 16;
		cr[0].top = 16;
		skinbmp[m.tl.bmpid]->maskblt(cmask,cr[0].left,cr[0].top,0,0,m.tl);
		// Top
		cr[1].left = 16+m.tlo.x+ox1;
		cr[1].top = 16+m.to+oy1-m.t.b.h;
		t = w-m.tlo.x-m.tro.x-ox1-ox2;
		skinbmp[m.t.bmpid]->maskblt(cmask,cr[1].left,cr[1].top,t,0,m.t);
		// Top-right
		cr[2].left = 16+w-m.tro.x-ox2;
		cr[2].top = 16+m.tro.y+oy1-m.tr.b.h;
		skinbmp[m.tr.bmpid]->maskblt(cmask,cr[2].left,cr[2].top,0,0,m.tr);
		// Bottom-left
		cr[3].left = 16-m.bl.b.w+m.blo.x+ox1;
		cr[3].top = 16+h-m.blo.y-oy2;
		skinbmp[m.bl.bmpid]->maskblt(cmask,cr[3].left,cr[3].top,0,0,m.bl);
		// Bottom
		cr[4].left = 16+m.blo.x+ox1;
		cr[4].top = 16+h-m.bo-oy2;
		t = w-m.blo.x-m.bro.x-ox1-ox2;
		skinbmp[m.b.bmpid]->maskblt(cmask,cr[4].left,cr[4].top,t,0,m.b);
		// Bottom-right
		cr[5].left = 16+w-m.bro.x-ox2;
		cr[5].top = 16+h-m.bro.y-oy2;
		skinbmp[m.br.bmpid]->maskblt(cmask,cr[5].left,cr[5].top,0,0,m.br);
		// Left
		cr[6].left = 16+m.lo+ox1-m.l.b.w;
		cr[6].top = 16+m.tlo.y+oy1;
		t = h-m.tlo.y-m.blo.y-oy1-oy2;
		skinbmp[m.l.bmpid]->maskblt(cmask,cr[6].left,cr[6].top,0,t,m.l);
		// Right
		cr[7].left = 16+w-m.ro-ox1;
		cr[7].top = 16+m.tro.y+oy1;
		t = h-m.tro.y-m.bro.y-oy1-oy2;
		skinbmp[m.r.bmpid]->maskblt(cmask,cr[7].left,cr[7].top,0,t,m.r);
		cr[0].left -= 8;
		cr[0].top -= 8;
		cr[1].left += m.t.b.w-8;
		cr[1].top -= 8;
		cr[2].left -= 8;
		cr[2].top -= 8;
		cr[3].left -= 8;
		cr[3].top -= 8;
		cr[4].left += m.b.b.w-8;
		cr[4].top -= 8;
		cr[5].left -= 8;
		cr[5].top -= 8;
		cr[6].left -= 8;
		cr[6].top += m.l.b.h-8;
		cr[7].left -= 8;
		cr[7].top += m.r.b.h-8;
		cmask->fill(16+ox1,16+oy1,w-ox1-ox2,h-oy1-oy2,0);
		cmask->blur(guiprefs.shadowblur);
		cmask->opacity(guiprefs.shadowop);
		mask[x] = cmask;
	};
	LEAVE();
}

void MTSkin::delshadows()
{
	int x;

	for (x=0;x<8;x++){
		if (mask[x]){
			di->delmask(mask[x]);
			mask[x] = 0;
		};
	};
}
//---------------------------------------------------------------------------
bool initSkin()
{
	char defskin[512];
	char *ext;
	int flags;
	MTPreferences *prefs;
	MTResources *res;
	MTFile *f;

	ENTER("initSkin");
	skin = new MTSkin();
	prefs = (MTPreferences*)mtinterface->getprefs();
	if (prefs){
		strcpy(defskin,prefs->syspath[SP_SKINS]);
		strcat(defskin,"Default.mtr");
		flags = MTF_READ|MTF_SHAREREAD;
		if (mtinterface->type==guietype){
			char tmp[256];
			strcpy(tmp,"Default.mtr");
			si->filemaketemp(tmp,256);
			si->filecopy(defskin,tmp);
			strcpy(defskin,tmp);
			flags |= MTF_TEMP;
		};
		ext = strrchr(defskin,'.');
		f = si->fileopen(defskin,flags);
		if (f){
			res = si->resopen(f,true);
			if (res){
				gi->loadskin(res);
				if (skinres) si->resclose(skinres);
				skinres = res;
			};
			LEAVE();
			return true;
		};
	};
	LEAVE();
	return false;
}

void uninitSkin()
{
	ENTER("uninitSkin");
	if (skinres){
		si->resclose(skinres);
		skinres = 0;
	};
	delete skin;
	skin = 0;
	LEAVE();
}

/* FIXME
void loadskinfromstream(MTFile *f)
{
	int x;
	MTMiniConfig *mc;
	char p[5];

	mc = si->miniconfigcreate();
	mc->loadfromstream(f,MTMC_ALL|MTMC_MD5KEYS);

	p[4] = 0;
	for (x=0;x<32;x++){
		*(int*)p = sko[x].t;
		if (!mc->getparameter(p,sko[x].a,MTCT_BINARY,sko[x].s)) continue;
	};

	si->miniconfigdelete(mc);
}

int saveskintostream(MTFile *f)
{
	int x,l;
	MTMiniConfig *mc;
	char p[5];

	mc = si->miniconfigcreate();

	p[4] = 0;
	for (x=0;x<32;x++){
		*(int*)p = sko[x].t;
		mc->setparameter(p,sko[x].a,MTCT_BINARY,sko[x].s);
	};

	l = mc->savetostream(f,MTMC_ALL|MTMC_MD5KEYS);
	si->miniconfigdelete(mc);
	return l;
}


void *saveskin(int &size)
{
	int x;
	void *stream;
	char *sp;
	
	size = 32*8;
	for (x=0;x<32;x++) size += sko[x].s;
	stream = si->memalloc(size);
	sp = (char*)stream;
	for (x=0;x<32;x++){
		*(int*)sp = sko[x].t;
		sp += 4;
		*(int*)sp = sko[x].s;
		sp += 4;
		memcpy(sp,sko[x].a,sko[x].s);
		sp += sko[x].s;
	};
	return stream;
}

void *parseskin(const void *source,int &size)
{
	int x,y,osize,ct,cs,n;
	bool found;
	void *stream;
	char *scan;
	char *sp;
	
	osize = size;
	size = 32*8;
	for (x=0;x<32;x++) size += sko[x].s;
	stream = si->memalloc(size,MTM_ZERO);
	sp = (char*)stream;
	for (x=0;x<32;x++){
		*(int*)sp = sko[x].t;
		sp += 4;
		*(int*)sp = sko[x].s;
		sp += 4;
		found = false;
		scan = (char*)source;
		while (scan<(char*)source+osize){
			ct = *(int*)scan;
			scan += 4;
			cs = *(int*)scan;
			scan += 4;
			if (ct==sko[x].t){
				found = true;
				break;
			};
			scan += cs;
		};
		if (found){
			if (cs==sko[x].s) memcpy(sp,scan,cs);
			else{
				n = sko[x].s/sko[x].p;
				ct = 0;
				cs = cs/n;
				for (y=0;y<n;y++){
					memcpy(sp+ct,scan,cs);
					ct += sko[x].p;
					scan += cs;
				};
			};
		}
		else memcpy(sp,sko[x].a,sko[x].s);
		sp += sko[x].s;
	};
	return stream;
}
*/
//---------------------------------------------------------------------------
