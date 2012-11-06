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
#include <stdio.h>
#include "MTGUI1.h"
#include "../Headers/MTXSkin.h"
#include "MTEdit.h"
#include "MTSysControls.h"
#include "MTWindow.h"
#include "../Headers/MTXInput.h"
//---------------------------------------------------------------------------
void MTCT edit_undo(MTShortcut *s,MTControl *c,MTUndo*)
{
	MTEdit *edit = (MTEdit*)((MTMenu*)((MTMenuItem*)c)->parent)->caller;
	edit->undo();
}

void MTCT edit_cut(MTShortcut *s,MTControl *c,MTUndo*)
{
}

void MTCT edit_copy(MTShortcut *s,MTControl *c,MTUndo*)
{
}

void MTCT edit_paste(MTShortcut *s,MTControl *c,MTUndo*)
{
}

void MTCT edit_delete(MTShortcut *s,MTControl *c,MTUndo*)
{
}

void MTCT edit_select(MTShortcut *s,MTControl *c,MTUndo*)
{
	MTEdit *edit = (MTEdit*)((MTMenu*)((MTMenuItem*)c)->parent)->caller;
	edit->setselstart(0);
	edit->setselend(-1);
}
//---------------------------------------------------------------------------
// MTControl
//   MTEdit
//---------------------------------------------------------------------------
MTEdit::MTEdit(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_EDIT,tag,p,l,t,w,h),
maxlength(0),
password(false),
lblank(0),
rblank(0),
md(false),
focused(false),
isnew(false),
cursor(0),
offset(0),
selstart(0),
selend(0),
mss(0),
mse(0),
oldtext(0),
oldselstart(0),
oldselend(0),
timer(0)
{
	if (parent){
		if (!popup){
			popup = (MTMenu*)gi->newcontrol(MTC_MENU,0,parent->dsk,0,0,0,0,0);
			popup->flags |= MTCF_DONTSAVE;
		}
		else{
			if (popup->numitems>0) popup->additem("|Edit",0,0,false,0);
		};
		((MTMenuItem*)popup->additem("&Undo",-1,0,false,0))->command = edit_undo;
		popup->additem("|",-1,0,false,0);
		((MTMenuItem*)popup->additem("Cu&t",-1,0,false,0))->command = edit_cut;
		((MTMenuItem*)popup->additem("&Copy",-1,0,false,0))->command = edit_copy;
		((MTMenuItem*)popup->additem("&Paste",-1,0,false,0))->command = edit_paste;
		((MTMenuItem*)popup->additem("&Delete",-1,0,false,0))->command = edit_delete;
		popup->additem("|",-1,0,false,0);
		((MTMenuItem*)popup->additem("Select A&ll",-1,0,false,0))->command = edit_select;
	};
	flags |= MTCF_ACCEPTINPUT;
	if ((w==0) || (h==0)){
		width = 128;
		height = 22;
	};
	if (width<16) width = 16;
	if (height<16) height = 16;
	text = (char*)si->memalloc(1024,MTM_ZERO);
	if (candesign){
		gi->setcontrolname(this,"edit");
		strcpy(text,name);
	};
}

MTEdit::~MTEdit()
{
	si->memfree(text);
	if (oldtext) si->memfree(oldtext);
	if (timer) gi->deltimer(this,timer);
}

int MTEdit::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTControl::loadfromstream(f,size,flags);
	int l,x;

	f->readln(text,1024);
	text[1023] = 0;
	l = strlen(text)+1;
	x = (4-(l & 3)) & 3;
	f->seek(x,MTF_CURRENT);
	l += x;
	f->read(&maxlength,4);
	if (guiid==MTC_EDIT){
		f->read(&password,4);
		l += 4;
	};
	return csize+l+4;
}

int MTEdit::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	int l = strlen(text)+1;
	int o,zero;
	
	zero = 0;
	o = ((l+3)>>2)<<2;
	f->write(text,l);
	f->write(&zero,o-l);
	f->write(&maxlength,4);
	if (guiid==MTC_EDIT){
		l = (int)password;
		f->write(&l,4);
		o += 4;
	};
	return csize+o+4;
}

#define EditNP (ControlNP+3)
int MTEdit::getnumproperties(int id)
{
	if (id==-1) return EditNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	return 0;
}

bool MTEdit::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[3] = {"Text","MaxLength","Password"};
	static int propflags[3] = {MTP_TEXT,MTP_INT,MTP_BOOL};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if (id>=EditNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTEdit::getproperty(int id,void *value)
{
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		strcpy((char*)value,text);
		break;
	case 1:
		*(int*)value = maxlength;
		break;
	case 2:
		*(int*)value = password;
		break;
	default:
		return false;
	};
	return true;
}

bool MTEdit::setproperty(int id,void *value)
{
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ControlNP){
	case 0:
		settext((const char*)value);
		return true;
	case 1:
		maxlength = *(int*)value;
		if (maxlength<0) maxlength = 0;
		if (maxlength>1023) maxlength = 1023;
		if ((password) && (maxlength>32)) maxlength = 32;
		break;
	case 2:
		password = *(int*)value!=0;
		if ((password) && (maxlength>32)) maxlength = 32;
		break;
	default:
		return false;
	};
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
	return true;
}

void MTEdit::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	MTRect cr = {0,0,width,height};
	preparedraw(&b,x,y);
	if (&rect){
		if (!cliprect(cr,rect)) goto exit;
	};
	skin->drawcontrol(this,cr,b,x,y);
exit:
	if (guiid==MTC_EDIT) MTControl::draw(rect);
}

bool MTEdit::message(MTCMessage &msg)
{
	int l;
	int max = maxlength-1;
	char *r;
	static unsigned char keys[256];
	
	if (max<0) max = 1023;
	switch (msg.msg){
	case MTCM_ENTER:
		setselstart(0);
		setselend(-1);
		break;
	case MTCM_LEAVE:
		focused = false;
		break;
	case MTCM_MOUSEDOWN:
		if (msg.button!=DB_LEFT) break;
		if (msg.buttons & DB_DOUBLE){
			setselstart(0);
			setselend(-1);
		}
		else{
			if (focused){
				md = true;
				if (password) l = skin->gettextextent(this,0,strlen(text)-offset,msg.x-lblank-4);
				else l = skin->gettextextent(this,&text[offset],-1,msg.x-lblank-4);
				setcursor(offset+l);
			}
			else focused = true;
		};
		break;
	case MTCM_MOUSEMOVE:
		if (md){
			if (msg.x>=lblank+4){
				if (password) l = offset+skin->gettextextent(this,0,strlen(text)-offset,msg.x-lblank-4);
				else l = offset+skin->gettextextent(this,&text[offset],-1,msg.x-lblank-4);
			}
			else{
				if (offset==0) break;
				MTPoint ts;
				skin->gettextsize(this,(password)?0:text,offset,&ts);
				ts.x -= lblank;
				if (ts.x+msg.x-4<=0) l = 0;
				else{
					if (password) l = skin->gettextextent(this,0,strlen(text),ts.x+msg.x-4);
					else l = skin->gettextextent(this,text,-1,ts.x+msg.x-4);
				};
			};
			setselend(l);
		};
		break;
	case MTCM_MOUSEUP:
		md = false;
		break;
	case MTCM_CHAR:
		if (msg.key<KB_SPACE) break;
		l = strlen(text);
		if (l==max) break;
		modify();
		if (mss==l){
			strcat(text,(char*)&msg.key);
			setcursor(cursor+1);
		}
		else{
			r = &text[mse];
			memmove(&text[mss+1],r,strlen(r)+1);
			text[mss] = msg.key;
			setcursor(mss+1);
		};
		return true;
	case MTCM_KEYDOWN:
		if (MTControl::message(msg)) return true;
		switch (msg.key){
		case KB_DELETE:
			if (mss==mse){
				if (mss==strlen(text)) break;
				r = &text[mss+1];
			}
			else r = &text[mse];
			modify();
			memmove(&text[mss],r,strlen(r)+1);
			setcursor(mss);
			return true;
		case KB_BACK:
			r = &text[mse];
			if (mss==mse){
				if (mss==0) break;
				modify();
				memmove(&text[mss-1],r,strlen(r)+1);
				setcursor(mss-1);
			}
			else{
				modify();
				memmove(&text[mss],r,strlen(r)+1);
				setcursor(mss);
			};
			return true;
		case KB_HOME:
			if (msg.buttons & DB_SHIFT) setselend(0);
			else setcursor(0);
			return true;
		case KB_END:
			if (msg.buttons & DB_SHIFT) setselend(-1);
			else setcursor(-1);
			return true;
		case KB_LEFT:
			if (msg.buttons & DB_SHIFT){
				if (selend) setselend(selend-1);
			}
			else{
				if (mss){
					if (mss==mse) setcursor(mss-1);
					else setcursor(mss);
				}
				else setcursor(0);
			};
			return true;
		case KB_RIGHT:
			if (msg.buttons & DB_SHIFT){
				if (selend<strlen(text)) setselend(selend+1);
			}
			else{
				if (mse<strlen(text)){
					if (mse==mss) setcursor(mse+1);
					else setcursor(mse);
				}
				else setcursor(mse);
			};
			return true;
		};
		break;
	case MTCM_TIMER:
		if (timer){
			gi->deltimer(this,timer);
			timer = 0;
		};
		isnew = false;
		break;
	};
	return MTControl::message(msg);
}

void MTEdit::settext(const char *t)
{
	int l;

	modify();
	l = strlen(t);
	strcpy(text,t);
	if (selend>l){
		setselend(l);
		return;
	};
	if (cursor>l){
		setcursor(l);
		return;
	};
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}

void MTEdit::setselstart(int ss)
{
	if (ss<0) ss = 0;
	else if (ss>strlen(text)) ss = strlen(text);
	if (selstart==ss) return;
	selstart = ss;
	if (selstart>selend){
		mss = selend;
		mse = selstart;
	}
	else{
		mss = selstart;
		mse = selend;
	};
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}

void MTEdit::setselend(int se)
{
	if ((se<0) || (se>strlen(text))) se = strlen(text);
	if (selend==se) return;
	selend = se;
	if (selstart>selend){
		mss = selend;
		mse = selstart;
	}
	else{
		mss = selstart;
		mse = selend;
	};
	cursor = se;
	if (cursor<offset) offset = cursor;
	if (parent){
		char *tmp = &text[offset];
		MTPoint ts;
		if (password){
			if ((skin->gettextsize(this,0,strlen(tmp),&ts)) && (ts.x>width-lblank-rblank-8)){
				int nc = skin->gettextextent(this,0,cursor-offset,width-lblank-rblank-8);
				if (offset<cursor-nc){
					offset = cursor-nc;
					if (offset>strlen(text)) offset = strlen(text);
				};
			};
		}
		else{
			if ((skin->gettextsize(this,tmp,-1,&ts)) && (ts.x>width-lblank-rblank-8)){
				int nc = skin->gettextextent(this,tmp,cursor-offset,width-lblank-rblank-8);
				if (offset<cursor-nc){
					offset = cursor-nc;
					if (offset>strlen(text)) offset = strlen(text);
				};
			};
		};
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}

void MTEdit::setcursor(int c)
{
	if ((c<0) || (c>strlen(text))) c = strlen(text);
	cursor = c;
	selstart = c;
	selend = c;
	mss = c;
	mse = c;
	if (cursor<offset){
		offset = cursor-8;
		if (offset<0) offset = 0;
	};
	if (parent){
		char *tmp = &text[offset];
		MTPoint ts;
		if (password){
			if ((skin->gettextsize(this,0,strlen(tmp),&ts)) && (ts.x>width-lblank-rblank-8)){
				int nc = skin->gettextextent(this,0,cursor-offset,width-lblank-rblank-8);
				if (offset<cursor-nc){
					offset = cursor-nc;
					if (offset>strlen(text)) offset = strlen(text);
				};
			};
		}
		else{
			if ((skin->gettextsize(this,tmp,-1,&ts)) && (ts.x>width-lblank-rblank-8)){
				int nc = skin->gettextextent(this,tmp,cursor-offset,width-lblank-rblank-8);
				if (offset<cursor-nc){
					offset = cursor-nc;
					if (offset>strlen(text)) offset = strlen(text);
				};
			};
		};
		gi->resetcursor();
	};
}

void MTEdit::undo()
{
	char *newtext;
	int newselstart,newselend;

	isnew = false;
	if (oldtext){
		newtext = (char*)si->memalloc(strlen(text)+1,0);
		strcpy(newtext,text);
		strcpy(text,oldtext);
		si->memfree(oldtext);
		oldtext = newtext;
		newselstart = selstart;
		newselend = selend;
		setselstart(oldselstart);
		setselend(oldselend);
		oldselstart = newselstart;
		oldselend = newselend;
	}
	else if (text[0]){
		oldtext = (char*)si->memalloc(strlen(text)+1,0);
		strcpy(oldtext,text);
		oldselstart = selstart;
		oldselend = selend;
		text[0] = 0;
		setselstart(0);
		setselend(0);
	};
}

void MTEdit::modify()
{
	if (!isnew){
		if (oldtext) si->memfree(oldtext);
		oldtext = (char*)si->memalloc(strlen(text)+1,0);
		strcpy(oldtext,text);
		oldselstart = selstart;
		oldselend = selend;
	};
	isnew = true;
	if (timer) gi->deltimer(this,timer);
	timer = gi->ctrltimer(this,0,2000,false);
}
//---------------------------------------------------------------------------
// MTControl
//   MTEdit
//     MTComboBox
//---------------------------------------------------------------------------
MTComboBox::MTComboBox(int id,int tag,MTWinControl *p,int l,int t,int w,int h):
MTEdit(tag,p,l,t,w,h),
dropcount(8),
mlb(0),
modified(false)
{
	int tmp;

	guiid = id;
	skin->getcontrolsize(id,1,rblank,tmp);
}

MTComboBox::~MTComboBox()
{
	gi->delcontrol(mlb);
}

#define ComboNP (EditNP+1)
int MTComboBox::getnumproperties(int id)
{
	if (id==-1) return ComboNP;
	if (id<EditNP) return MTEdit::getnumproperties(id);
	return 0;
}

bool MTComboBox::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[1] = {"View"};
	static int propflags[1] = {MTP_FLAGS};
	
	if ((id<EditNP) || ((id & 0xFF00) && ((id>>8)<EditNP))) return MTEdit::getpropertytype(id,name,flags);
	if (id>=ComboNP) return false;
	*name = propname[id-EditNP];
	flags = propflags[id-EditNP];
	return true;
}

bool MTComboBox::getproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<EditNP) || ((id & 0xFF00) && ((id>>8)<EditNP))) return MTEdit::getproperty(id,value);
	switch (id-EditNP){
	case 0:
		iv = mlb->viewflags;
		break;
	default:
		return false;
	};
	return true;
}

bool MTComboBox::setproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<EditNP) || ((id & 0xFF00) && ((id>>8)<EditNP))) return MTEdit::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-EditNP){
	case 0:
		mlb->viewflags = iv!=0;
		break;
	default:
		return false;
	};
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
	return true;
}

bool MTComboBox::checkbounds(int &l,int &t,int &w,int &h)
{
	bool ok = true;

	if (w<16+lblank+rblank){
		w = 16+lblank+rblank;
		ok = false;
	};
	return ok & MTEdit::checkbounds(l,t,w,h);
}

void MTComboBox::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	lblank = (mlb->viewflags & MTVF_IMAGES)?sysimages.iw+2:0;
	MTEdit::draw(rect);
	MTControl::draw(rect);
}

bool MTComboBox::message(MTCMessage &msg)
{
	int l,x;
	char *caption;

	switch (msg.msg){
	case MTCM_MOUSEDOWN:
		if (msg.x>=width-2-rblank){
			if (guitick!=popuptick) pulldown();
			return true;
		};
		break;
	case MTCM_ACTION:
		if ((mlb->selected>=0) || (msg.param1==2)) setitem(mlb->selected);
		if ((msg.param1<2) && (parent)) parent->focus(this);
		return true;
	case MTCM_CHAR:
		MTEdit::message(msg);
		if (msg.key<KB_SPACE) break;
		modified = true;
		l = strlen(text);
		if ((l>2) && (selstart==selend) && (selstart==cursor) && (selstart==l)){
			x = mlb->searchitem(text,&caption);
			if (x>=0){
				x = strlen(caption);
				settext(caption);
				setcursor(l);
				setselend(x);
			};
		};
		return true;
	case MTCM_KEYDOWN:
		if (MTEdit::message(msg)) return true;
		if (msg.key==KB_DOWN) pulldown();
		if (mlb->message(msg)) return true;
		return false;
	case MTCM_LEAVE:
		if (modified){
			x = mlb->searchitem(text,0);
			if (x>=0) mlb->setitem(x);
			modified = false;
		};
		break;
	case MTCM_NOTIFY:
		if (msg.param1==0){
			int tmp;
			lblank = (mlb->viewflags & MTVF_IMAGES)?sysimages.iw+2:0;
			skin->getcontrolsize(guiid,1,rblank,tmp);
		};
		break;
	};
	return MTEdit::message(msg);
}

void MTComboBox::pulldown()
{
	int x;

	if (modified){
		x = mlb->searchitem(text,0);
		if (x>=0) mlb->setitem(x);
		modified = false;
	};
	if ((parent) && (parent->dsk)){
		parent->dsk->setcombo(this);
		if (parent){
			MTCMessage msg = {MTCM_CHANGE,0,this};
			parent->message(msg);
		};
	};
}

int MTComboBox::getselected()
{
	return mlb->selected;
}

bool MTComboBox::getiteminfo(int id,char **caption,int *imageindex,int *flags,bool *editable)
{
	return mlb->getiteminfo(id,caption,imageindex,flags,editable);
}

void MTComboBox::setitem(int id)
{
	char *c = 0;

	mlb->setitem(id);
	if (id<0){
		text[0] = 0;
		offset = 0;
		if (parent){
			MTCMessage msg = {MTCM_CHANGE,0,this};
			parent->message(msg);
		};
		return;
	};
	if (mlb->getiteminfo(id,&c,0,0,0)){
		if (c) settext(c);
	};
}

int MTComboBox::searchitem(const char *search,char **caption)
{
	return mlb->searchitem(search,caption);
}
//---------------------------------------------------------------------------
// MTControl
//   MTEdit
//     MTComboBox
//       MTUserComboBox
//---------------------------------------------------------------------------
MTUserComboBox::MTUserComboBox(int tag,MTWinControl *p,int l,int t,int w,int h):
MTComboBox(MTC_USERCOMBOBOX,tag,p,l,t,w,h)
{
	p = p->dsk;
	if (p){
		mlb = (MTList*)gi->newcontrol(MTC_FILELISTBOX,0,p,0,0,64,64,0);
		mlb->owner = this;
		mlb->switchflags(MTCF_DONTSAVE|MTCF_HIDDEN,true);
		((MTDesktop*)p)->puttoback(mlb);
	};
}

void MTUserComboBox::setnumitems(int n)
{
	((MTUserList*)mlb)->setnumitems(n);
}
//---------------------------------------------------------------------------
// MTControl
//   MTEdit
//     MTComboBox
//       MTItemComboBox
//---------------------------------------------------------------------------
MTItemComboBox::MTItemComboBox(int tag,MTWinControl *p,int l,int t,int w,int h):
MTComboBox(MTC_ITEMCOMBOBOX,tag,p,l,t,w,h)
{
	p = p->dsk;
	if (p){
		mlb = (MTList*)gi->newcontrol(MTC_LISTBOX,0,p,0,0,64,64,0);
		mlb->owner = this;
		mlb->switchflags(MTCF_DONTSAVE|MTCF_HIDDEN,true);
		((MTDesktop*)p)->puttoback(mlb);
	};
}

int MTItemComboBox::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTComboBox::loadfromstream(f,size,flags);
	MTItemView *clv = (MTItemView*)mlb;
	
	csize += clv->loadfromstream(f,size,1);
	return csize;
}

int MTItemComboBox::savetostream(MTFile *f,int flags)
{
	int csize = MTComboBox::savetostream(f,flags);
	MTItemView *clv = (MTItemView*)mlb;

	csize += clv->savetostream(f,1);
	return csize;
}

#define ItemComboNP (ComboNP+2)
int MTItemComboBox::getnumproperties(int id)
{
	if (id==-1) return ItemComboNP;
	if (id<ComboNP) return MTComboBox::getnumproperties(id);
	return 0;
}

bool MTItemComboBox::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[2] = {"Folder","Items"};
	static int propflags[2] = {MTP_BOOL,MTP_ITEMS};
	
	if ((id<ComboNP) || ((id & 0xFF00) && ((id>>8)<ComboNP))) return MTComboBox::getpropertytype(id,name,flags);
	if (id>=ItemComboNP) return false;
	*name = propname[id-ComboNP];
	flags = propflags[id-ComboNP];
	return true;
}

bool MTItemComboBox::getproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ComboNP) || ((id & 0xFF00) && ((id>>8)<ComboNP))) return MTComboBox::getproperty(id,value);
	switch (id-ComboNP){
	case 0:
		iv = 0;
		break;
	case 2:
		*(MTList**)value = mlb;
		break;
	default:
		return false;
	};
	return true;
}

bool MTItemComboBox::setproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ComboNP) || ((id & 0xFF00) && ((id>>8)<ComboNP))) return MTComboBox::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ComboNP){
	case 0:
		mlb->viewflags = iv!=0;
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		return false;
	};
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
	return true;
}

MTItem* MTItemComboBox::additem(const char *caption,int image,int flags,bool editable,void *data)
{
	return ((MTListBox*)mlb)->additem(caption,image,flags,editable,data);
}

void MTItemComboBox::removeitem(MTItem *item)
{
	((MTListBox*)mlb)->removeitem(item);
}

void MTItemComboBox::clearitems()
{
	((MTListBox*)mlb)->clearitems();
}

void MTItemComboBox::beginupdate()
{
	((MTListBox*)mlb)->beginupdate();
}

void MTItemComboBox::endupdate()
{
	((MTListBox*)mlb)->endupdate();
}

void MTItemComboBox::sort(int f)
{
	((MTListBox*)mlb)->sort(f);
}

MTItem* MTItemComboBox::getitem(int id)
{
	return ((MTListBox*)mlb)->getitem(id);
}

MTItem* MTItemComboBox::getitemfromtag(int tag)
{
	return ((MTListBox*)mlb)->getitemfromtag(tag);
}
//---------------------------------------------------------------------------
