//---------------------------------------------------------------------------
//
//	MadTracker GUI Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//    MTControl
//      MTItem
//        MTListItem
//        MTMenuItem
//      MTWinControl
//				MTUserList
//					MTFileListBox
//        MTItemView
//          MTListBox
//          MTMenu
//
//---------------------------------------------------------------------------
#include "MTGUI1.h"
#include "MTItems.h"
#include "MTSysControls.h"
#include "MTWindow.h"
#include "../Headers/MTXSystem2.h"
#include "../MTSystem/strnatcmp.h"
#include "../Headers/MTXInput.h"
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTList
//---------------------------------------------------------------------------
MTList::MTList(int id,int tag,MTWinControl *p,int l,int t,int w,int h):
MTWinControl(id,tag,p,l,t,w,h),
owner(0),
viewflags(0),
selected(-1),
numitems(0),
itemheight(16)
{
}

MTList::~MTList()
{
	if ((owner) && ((owner->guiid==MTC_USERCOMBOBOX) || (owner->guiid==MTC_ITEMCOMBOBOX))){
		((MTComboBox*)owner)->mlb = 0;
	};
}

#define ListNP (ControlNP+1)
int MTList::getnumproperties(int id)
{
	if (id==-1) return ListNP;
	if (id<ListNP) return MTControl::getnumproperties(id);
	return 0;
}

bool MTList::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[1] = {"View"};
	static int propflags[1] = {MTP_FLAGS};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if (id>=ListNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTList::getproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		iv = viewflags;
		break;
	default:
		return false;
	};
	return true;
}

bool MTList::setproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ControlNP){
	case 0:
		viewflags = iv;
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

bool MTList::message(MTCMessage &msg)
{
	int id,p;

	switch (msg.msg){
	case MTCM_KEYDOWN:
		switch (msg.key){
		case KB_PAGEUP:
			p = (height/itemheight)-1;
			if (selected>=0) id = selected-p;
			else id = numitems-1;
			if (id<0) id = 0;
			setitem(id);
			return true;
		case KB_PAGEDOWN:
			p = (height/itemheight)-1;
			if (selected>=0) id = selected+p;
			else id = 0;
			if (id>numitems-1) id = numitems-1;
			setitem(id);			
			return true;
	  case KB_END:
			if (numitems>0) setitem(numitems-1);
			return true;
	  case KB_HOME:
			if (numitems>0) setitem(0);
			return true;
	  case KB_UP:
			if (selected>=0) id = selected-1;
			else id = numitems-1;
			if (id<0) id = 0;
			setitem(id);
			return true;
	  case KB_DOWN:
			if (selected>=0) id = selected+1;
			else id = 0;
			if (id>numitems-1) id = numitems-1;
			setitem(id);
			return true;
		};
		break;
	case MTCM_ACTION:
		if (parent) return parent->message(msg);
		return false;
	};
	return MTWinControl::message(msg);
}

bool MTList::getiteminfo(int id,char **caption,int *imageindex,int *flags,bool *editable)
{
	return false;
}

int MTList::searchitem(const char *search,char **caption)
{
	return -1;
}

void MTList::setitem(int id)
{
	selected = id;
	if (owner){
		MTCMessage msg = {MTCM_ACTION,0,this,2};
		owner->message(msg);
	};
}
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTList
//       MTUserList
//---------------------------------------------------------------------------
MTUserList::MTUserList(int tag,MTWinControl *p,int l,int t,int w,int h):
MTList(MTC_USERLIST,tag,p,l,t,w,h),
over(-1),
userdrawproc(0),
getiteminfoproc(0),
itemmessageproc(0)
{
	int sw,sh;

	flags |= MTCF_ACCEPTINPUT;
	gi->setcontrolname(this,"userlist");
	skin->getcontrolsize(MTC_SCROLLER,1,sw,sh);
	vs = (MTScroller*)gi->newcontrol(MTC_SCROLLER,0,this,width-sw,0,0,height,0);
	vs->type = MTST_VBAR;
	vs->switchflags(MTCF_SYSTEM|MTCF_DONTSAVE,true);
	vs->align = MTCA_RIGHT;
	updatescroller();
}

void MTUserList::setbounds(int l,int t,int w,int h)
{
	MTWinControl::setbounds(l,t,w,h);
	updatescroller();
}

void MTUserList::switchflags(int f,bool set)
{
	MTWinControl::switchflags(f,set);
	if ((f & MTCF_OVER) && (!set)){
		over = -1;
		if (parent){
			MTCMessage msg = {MTCM_CHANGE,0,this,f,set};
			parent->message(msg);
		};
	};
}

void MTUserList::draw(MTRect &rect)
{
	int i;
	int x = 0;
	int y = 0;
	MTBitmap *b;
	char *caption;
	int imageindex,itemflags;
	bool editable;
	MTRect cr = {0,0,width,height};
	MTRect r;
	
	if (flags & MTCF_CANTDRAW) return;
	if (&rect){
		if (!cliprect(cr,rect)) goto exit;
	};
	preparedraw(&b,x,y);
	skin->drawcontrol(this,cr,b,x,y);
	clip(cr);
	r.left = x+2;
	r.right = x+width-vs->width-2;
	r.top = y-vs->pos;
	r.bottom = r.top+itemheight;
	if (userdrawproc){
		for (i=0;i<numitems;i++){
			if (r.top>cr.top-itemheight+y){
				if (r.top>=cr.bottom+y) break;
				userdrawproc(this,i,r,b);
			};
			r.top += itemheight;
			r.bottom += itemheight;
		};
	}
	else if (getiteminfoproc){
		for (i=0;i<numitems;i++){
			if (r.top>cr.top-itemheight+y){
				if (r.top>=cr.bottom+y) break;
				caption = 0;
				imageindex = -1;
				itemflags = 0;
				editable = false;
				if (getiteminfoproc(this,i,&caption,&imageindex,&itemflags,&editable)<0) continue;
				skin->drawitem(this,i,r,b,caption,imageindex,itemflags,editable);
			};
			r.top += itemheight;
			r.bottom += itemheight;
		};
	};
	unclip();
exit:
	MTWinControl::draw(rect);
}

bool MTUserList::message(MTCMessage &msg)
{
	int old;

	switch (msg.msg){
	case MTCM_MOUSEMOVE:
		if ((MTList::message(msg)) || (msg.x>=width-vs->width)) return true;
		old = over;
		if (flags & MTCF_OVER){
			over = (msg.y+vs->pos)/itemheight;
			if ((over<0) || (over>=numitems)) over = -1;
		}
		else{
			over = -1;
		};
		if ((itemmessageproc) && (over>=0)){
			msg.y -= over*itemheight-vs->pos;
			itemmessageproc(this,over,msg);
		};
		if (over!=old){
			MTCMessage msg = {MTCM_CHANGE,0,this};
			MTRect cr = {0,0,width,height};
			MTRect r;
			if (old>=0){
				r.left = 0;
				r.right = width;
				r.top = old*itemheight-vs->pos;
				r.bottom = r.top+itemheight;
				if (cliprect(r,cr)){
					msg.dr = r;
					parent->message(msg);
				};
			};
			if (over>=0){
				r.left = 0;
				r.right = width;
				r.top = over*itemheight-vs->pos;
				r.bottom = r.top+itemheight;
				if (cliprect(r,cr)){
					msg.dr = r;
					parent->message(msg);
				};
			};
		};
		return true;
	case MTCM_MOUSEDOWN:
		if ((MTList::message(msg)) || (msg.x>=width-vs->width)) return true;
		setitem((msg.y+vs->pos)/itemheight);
		if ((itemmessageproc) && (selected>=0)){
			msg.y -= selected*itemheight-vs->pos;
			itemmessageproc(this,selected,msg);
		};
		return true;
	case MTCM_MOUSECLICK:
		if ((itemmessageproc) && (selected>=0) && (selected==over)){
			itemmessageproc(this,selected,msg);
		};
		return true;
	};
	return MTList::message(msg);
}

bool MTUserList::getiteminfo(int id,char **caption,int *imageindex,int *flags,bool *editable)
{
	if (getiteminfoproc) return (getiteminfoproc(this,id,caption,imageindex,flags,editable)>=0);
	return false;
}

int MTUserList::searchitem(const char *search,char **caption)
{
	int x,y;
	char *ccap;
	
	if (!getiteminfoproc) return -1;
	y = selected;
	if (y<0) y = 0;
	for (x=0;x<numitems;x++){
		if (getiteminfoproc(this,y,&ccap,0,0,0)>=0){
			if (strstr(ccap,search)){
				if (caption) *caption = ccap;
				return y;
			};
		};
		if (++y==numitems) y = 0;
	};
	return -1;
}

void MTUserList::setitem(int id)
{
	int old;

	if ((id<0) || (id>=numitems)) id = -1;
	if (id==selected) return;
	old = selected;
	selected = id;
	if (selected!=old){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		MTRect cr = {0,0,width,height};
		MTRect r;
		if (old>=0){
			r.left = 0;
			r.right = width;
			r.top = old*itemheight-vs->pos;
			r.bottom = r.top+itemheight;
			if (cliprect(r,cr)){
				msg.dr = r;
				parent->message(msg);
			};
		};
		if (selected>=0){
			r.left = 0;
			r.right = width;
			r.top = selected*itemheight-vs->pos;
			r.bottom = r.top+itemheight;
			if (cliprect(r,cr)){
				msg.dr = r;
				parent->message(msg);
			};
			r.left = 0;
			r.right = width;
			r.top = selected*itemheight;
			r.bottom = r.top+itemheight;
			showrect(r);
		};
	};
	MTList::setitem(id);
}

void MTUserList::setnumitems(int n)
{
	MTComboBox *combo = (MTComboBox*)owner;

	numitems = n;
	if (selected>=numitems){
		setitem(-1);
	};
	if (over>=numitems) over = -1;
	updatescroller();
	if (parent){
		MTCMessage cmsg = {MTCM_CHANGE,0,this};
		parent->message(cmsg);
	};
}

void MTUserList::updatescroller()
{
	if (!vs) return;
	vs->maxpos = numitems*itemheight;
	vs->page = (height/itemheight)*itemheight;
	vs->incr = itemheight;
	vs->setposition(vs->pos);
}
//---------------------------------------------------------------------------
// MTControl
//   MTItem
//---------------------------------------------------------------------------
MTItem::MTItem(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_ITEM,tag,p,l,t,w,h),
index(0),
imageindex(-1),
itemflags(0),
data(0)
{
	flags |= (MTCF_ACCEPTINPUT|MTCF_TRANSPARENT|MTCF_DONTSAVE);;
	caption = (char*)si->memalloc(260,MTM_ZERO);
}

MTItem::~MTItem()
{
	si->memfree(caption);
	if (parent) ((MTItemView*)parent)->removeitem(this);
}

bool MTItem::message(MTCMessage &msg)
{
	if (msg.msg==MTCM_MOUSEDOWN){
		if ((msg.buttons & DB_DOUBLE) && (flags & MTCF_SELECTED)){
			MTCMessage msg = {MTCM_ACTION,0,this,1};
			parent->message(msg);
			return true;
		}
		else{
			parent->switchflags(MTCF_SELECTED,false);
			switchflags(MTCF_SELECTED,true);
			((MTItemView*)parent)->selecteditem = this;
			((MTList*)parent)->selected = index;
			MTCMessage msg = {MTCM_ITEMSELECT,0,this};
			parent->message(msg);
			return true;
		};
	}
	else if (msg.msg==MTCM_MOUSECLICK){
		MTCMessage msg = {MTCM_ACTION,0,this,0};
		parent->message(msg);
		return true;
	};
	return MTControl::message(msg);
}

void MTItem::setcaption(const char *c)
{
	strcpy(caption,c);
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTItemView
//---------------------------------------------------------------------------
MTItemView::MTItemView(int tag,MTWinControl *p,int l,int t,int w,int h):
MTList(MTC_ITEMVIEW,tag,p,l,t,w,h),
selecteditem(0),
userdrawproc(0),
updating(false)
{
	flags |= MTCF_ACCEPTINPUT;
}

MTItemView::~MTItemView()
{
	clearitems();
}

int MTItemView::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = (flags==0)?MTWinControl::loadfromstream(f,size,flags):0;
	int nc,x,psize;
	char *caption;
	struct{
		int image;
		int flags;
		bool editable;
	} itemdata;
	
	f->read(&viewflags,4);
	f->seek(4,MTF_CURRENT);
	f->read(&nc,4);
	csize += 12;
	for (x=0;x<nc;x++){
		f->read(&psize,4);
		caption = (char*)si->memalloc(psize,0);
		f->read(caption,psize);
		f->read(&itemdata,sizeof(itemdata));
		additem(caption,itemdata.image,itemdata.flags,itemdata.editable,0);
		si->memfree(caption);
		csize += psize+16;
	};
	return csize;
}

int MTItemView::savetostream(MTFile *f,int flags)
{
	int csize = (flags==0)?MTWinControl::savetostream(f,flags):0;
	int x,l,nc,o,zero;
	
	nc = 0;
	for (x=0;x<ncontrols;x++){
		if (((controls[x]->guiid & MTC_ITEM)==MTC_ITEM) && ((controls[x]->flags & MTCF_DONTSAVE)==0)) nc++;
	};
	f->write(&viewflags,4);
	x = 0;
	f->write(&x,4);
	f->write(&nc,4);
	csize += 12;
	zero = 0;
	for (x=0;x<ncontrols;x++){
		MTListItem &cli = *(MTListItem*)controls[x];
		if (((cli.guiid & MTC_ITEM)==MTC_ITEM) && ((cli.flags & MTCF_DONTSAVE)==0)){
			l = strlen(cli.caption)+1;
			o = ((l+3)>>2)<<2;
			f->write(&l,4);
			f->write(cli.caption,l);
			f->write(&zero,o-l);
			f->write(&cli.imageindex,4);
			f->write(&cli.flags,4);
			l = (int)cli.editable;
			f->write(&l,4);
			csize += o+16;
		};
	};
	return csize;
}

#define ItemViewNP (ListNP+1)
int MTItemView::getnumproperties(int id)
{
	if (id==-1) return ItemViewNP;
	if (id<ItemViewNP) return MTList::getnumproperties(id);
	return 0;
}

bool MTItemView::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[2] = {"View","Items"};
	static int propflags[2] = {MTP_FLAGS,MTP_ITEMS};
	
	if ((id<ListNP) || ((id & 0xFF00) && ((id>>8)<ListNP))) return MTList::getpropertytype(id,name,flags);
	if (id>=ItemViewNP) return false;
	*name = propname[id-ListNP];
	flags = propflags[id-ListNP];
	return true;
}

bool MTItemView::getproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ListNP) || ((id & 0xFF00) && ((id>>8)<ListNP))) return MTList::getproperty(id,value);
	switch (id-ListNP){
	case 0:
		*(MTItemView**)value = this;
		break;
	default:
		return false;
	};
	return true;
}

bool MTItemView::setproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ListNP) || ((id & 0xFF00) && ((id>>8)<ListNP))) return MTList::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ListNP){
	case 0:
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

bool MTItemView::message(MTCMessage &msg)
{
	if ((msg.msg==MTCM_KEYDOWN) && ((msg.key==KB_RETURN) || (msg.key==KB_ESCAPE))){
		MTCMessage msg = {MTCM_ACTION,0,this};
		if (parent) return parent->message(msg);
		return true;
	};
	if (MTList::message(msg)) return true;
	if (msg.msg==MTCM_MOUSEDOWN){
		switchflags(MTCF_SELECTED,false);
		selecteditem = 0;
		return true;
	};
	return false;
}

void MTItemView::addcontrol(MTControl *control)
{
	MTWinControl::addcontrol(control);
	if ((control->guiid & MTC_ITEM)==MTC_ITEM) numitems = ncontrols-1;
}

void MTItemView::delcontrol(MTControl *control)
{
	if ((control->guiid & MTC_ITEM)==MTC_ITEM){
		numitems = ncontrols-1;
		if (control==selecteditem){
			setitem(-1);
		};
	};
	MTWinControl::delcontrol(control);
}

MTItem *MTItemView::additem(const char *caption,int image,int flags,bool editable,void *data)
{
	int top;
	MTItem *ni;
	
	if (ncontrols>0) top = controls[ncontrols-1]->top+controls[ncontrols-1]->height;
	else top = 0;
	ni = (MTItem*)gi->newcontrol(MTC_ITEM,0,this,0,top,width,16,0);
	ni->index = ncontrols-2;
	ni->imageindex = image;
	ni->itemflags = flags;
	ni->editable = editable;
	ni->data = data;
	ni->setcaption(caption);
	return ni;
}

void MTItemView::clearitems()
{
	int io;
	
	for (io=0;io<ncontrols;io++){
		if ((controls[io]->guiid & MTC_ITEM)==MTC_ITEM) break;
	};
	while (ncontrols>io) gi->delcontrol(controls[io]);
}

void MTItemView::beginupdate()
{
	updating = true;
	flags |= (MTCF_DONTDRAW|MTCF_DONTFLUSH);
}

void MTItemView::endupdate()
{
	if (updating){
		flags &= ~(MTCF_DONTDRAW|MTCF_DONTFLUSH);
		updating = false;
		if (parent){
			MTCMessage msg = {MTCM_CHANGE,0,this};
			parent->message(msg);
		};
	};
}

void MTItemView::sort(int f)
{
	int x,io;
	MTItem *s,*c;
	
	for (io=0;io<ncontrols;io++){
		if ((controls[io]->guiid & MTC_ITEM)==MTC_ITEM) break;
	};
	if (ncontrols<io+2) return;
	bool bupdating = updating;
	if (!bupdating) beginupdate();
	if (f==0) quicksort(0,0,ncontrols-io-1);
	else if (f==1){
		quicksort(io+1,0,ncontrols-io-1);
		s = (MTItem*)controls[io];
		for (x=io+1;x<ncontrols;x++){
			c = (MTItem*)controls[x];
			if (c->data!=s->data){
				if (c->index-1>s->index) quicksort(0,s->index,c->index-1);
				s = c;
			}
			else if (x==ncontrols-1){
				quicksort(0,s->index,ncontrols-io-1);
			};
		};
	};
	if (!bupdating) endupdate();
}

bool MTItemView::getiteminfo(int id,char **caption,int *imageindex,int *flags,bool *editable)
{
	int io;
	MTItem *ci;

	if (id<0) id = selected;
	for (io=0;io<ncontrols;io++){
		if ((controls[io]->guiid & MTC_ITEM)==MTC_ITEM) break;
	};
	if ((id<0) || (id>ncontrols-io-1)) return false;
	ci = (MTItem*)controls[id+io];
	if (caption) *caption = ci->caption;
	if (imageindex) *imageindex = ci->imageindex;
	if (flags) *flags = ci->flags;
	if (editable) *editable = ci->editable;
	return true;
}

MTItem* MTItemView::getitem(int id)
{
	int io;
	
	for (io=0;io<ncontrols;io++){
		if ((controls[io]->guiid & MTC_ITEM)==MTC_ITEM) break;
	};
	return (MTItem*)controls[id+io];
}

MTItem* MTItemView::getitemfromtag(int tag)
{
	int x;
	
	for (x=0;x<ncontrols;x++){
		if (((controls[x]->guiid & MTC_ITEM)==MTC_ITEM) && (controls[x]->tag==tag)) return (MTItem*)controls[x];
	};
	return 0;
}

void MTItemView::setitem(int id)
{
	MTControl *old = focused;
	int io;
	
	if (selected==id) return;
	for (io=0;io<ncontrols;io++){
		if ((controls[io]->guiid & MTC_ITEM)==MTC_ITEM) break;
	};
	switchflags(MTCF_SELECTED,false);
	if ((id>=0) && (id<ncontrols-2)){
		controls[id+io]->switchflags(MTCF_SELECTED,true);
		selecteditem = (MTListItem*)controls[id+io];
		focused = selecteditem;
	}
	else{
		selecteditem = 0;
		focused = 0;
	};
	if (old) old->switchflags(MTCF_FOCUSED,false);
	if (focused){
		focused->switchflags(MTCF_FOCUSED,true);
		showcontrol(focused);
	};
	MTList::setitem(id);
}

int MTItemView::searchitem(const char *search,char **caption)
{
	int id,io,x,y;

	id = selected;
	if (id<0) id = 0;
	for (io=0;io<ncontrols;io++){
		if ((controls[io]->guiid & MTC_ITEM)==MTC_ITEM) break;
	};
	y = io+id;
	for (x=0;x<ncontrols-io;x++){
		MTListItem &ci = *(MTListItem*)controls[y];
		if (strstr(ci.caption,search)){
			if (caption) *caption = ci.caption;
			return y-io;
		};
		if (++y==ncontrols) y = io;
	};
	return -1;
}

void MTItemView::removeitem(MTItem *item)
{
	int x,io,h;
	
	for (io=0;io<ncontrols;io++){
		if ((controls[io]->guiid & MTC_ITEM)==MTC_ITEM) break;
	};
	if (selecteditem==item){
		selecteditem = 0;
		setitem(-1);
	};
	h = item->height;
	item->flags |= MTCF_DONTDRAW;
	for (x=item->index+io;x<ncontrols;x++){
		MTListItem &cli = *(MTListItem*)controls[x];
		cli.index--;
		cli.top -= h;
	};
	if ((parent) && (!(flags & MTCF_DONTDRAW))){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}

void MTItemView::quicksort(int f,int lo,int hi)
{
	int mlo,mhi,l,io;
	MTItem *t,*mid;
	char *ext,*p;
	char midc[256];
	
	for (io=0;io<ncontrols;io++){
		if ((controls[io]->guiid & MTC_ITEM)==MTC_ITEM) break;
	};
	mlo = lo;
	mhi = hi;
	mid = (MTItem*)controls[io+(mlo+mhi)/2];
	if (f==1){
		ext = strrchr(mid->caption,'.');
		if (ext){
			l = ext-mid->caption;
			strcpy(midc,ext);
			p = strchr(midc,0);
			memcpy(p,mid->caption,l);
			p[l] = 0;
		}
		else{
			midc[0] = '.';
			midc[1] = ' ';
			l = strlen(mid->caption);
			memcpy(&midc[2],mid->caption,l);
			midc[2+l] = 0;
		};
	};
	do{
		if (f==0){
			while (strnatcmp(((MTItem*)controls[io+mlo])->caption,mid->caption)<0)
				mlo++;
			while (strnatcmp(((MTItem*)controls[io+mhi])->caption,mid->caption)>0)
				mhi--;
		}
		else if (f==1){
			while (strnatcmp(((MTItem*)controls[io+mlo])->caption,midc)<0){
				mlo++;
			};
			while (strnatcmp(((MTItem*)controls[io+mhi])->caption,midc)>0){
				mhi--;
			};
		}
		else if (f==2){
			while ((int)((MTItem*)controls[io+mlo])->data<(int)mid->data)
				mlo++;
			while ((int)((MTItem*)controls[io+mhi])->data>(int)mid->data)
				mhi--;
		};
		if (mlo<=mhi){
			if (mlo!=mhi){
				t = (MTItem*)controls[io+mlo];
				controls[io+mlo] = controls[io+mhi];
				controls[io+mhi] = t;
				((MTItem*)controls[io+mlo])->index = mlo;
				controls[io+mlo]->top = mlo*16;
				((MTItem*)controls[io+mhi])->index = mhi;
				controls[io+mhi]->top = mhi*16;
			};
			mlo++;
			mhi--;
		};
	} while (mlo<=mhi);
	if (mhi>lo) quicksort(f,lo,mhi);
	if (mlo<hi) quicksort(f,mlo,hi);
}
//---------------------------------------------------------------------------
// MTControl
//   MTListItem
//---------------------------------------------------------------------------
MTListItem::MTListItem(int tag,MTWinControl *p,int l,int t,int w,int h):
MTItem(tag,p,l,t,w,h)
{
	guiid = MTC_LISTITEM;
}

void MTListItem::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	MTRect r = {x+2,y,x+width-2,y+height};
	MTListBox &cparent = *(MTListBox*)parent;
	if (cparent.userdrawproc){
		cparent.userdrawproc(this,rect,b);
		MTControl::draw(rect);
		return;
	};
	skin->drawcontrol(this,rect,b,x,y);
	MTControl::draw(rect);
}
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTItemView
//       MTListBox
//---------------------------------------------------------------------------
MTListBox::MTListBox(int tag,MTWinControl *p,int l,int t,int w,int h):
MTItemView(tag,p,l,t,w,h)
{
	int tmp,sw;

	guiid = MTC_LISTBOX;
	gi->setcontrolname(this,"listbox");
	skin->getcontrolsize(MTC_SCROLLER,1,sw,tmp);
	vs = (MTScroller*)gi->newcontrol(MTC_SCROLLER,0,this,width-sw,0,0,height,0);
	vs->type = MTST_VBAR;
	vs->switchflags(MTCF_SYSTEM|MTCF_DONTSAVE,true);
	vs->align = MTCA_RIGHT;
	updatescroller();
}

void MTListBox::setbounds(int l,int t,int w,int h)
{
	int x;
	
	for (x=1;x<ncontrols;x++) controls[x]->width = w-vs->width;
	MTWinControl::setbounds(l,t,w,h);
	updatescroller();
}

void MTListBox::draw(MTRect &rect)
{
	MTRect cr = {0,0,width,height};
	
	if (flags & MTCF_CANTDRAW) return;
	if (&rect){
		if (!cliprect(cr,rect)) goto exit;
	};
	skin->drawcontrol(this,cr,0,0,0);
exit:
	MTWinControl::draw(rect);
}

MTItem *MTListBox::additem(const char *caption,int image,int flags,bool editable,void *data)
{
	int top;
	MTListItem *ni;
	
	if (ncontrols>1) top = controls[ncontrols-1]->top+controls[ncontrols-1]->height;
	else top = 0;
	ni = (MTListItem*)gi->newcontrol(MTC_LISTITEM,0,this,0,top,width-vs->width,16,0);
	ni->index = ncontrols-2;
	ni->imageindex = image;
	ni->itemflags = flags;
	ni->editable = editable;
	ni->data = data;
	ni->setcaption(caption);
	return (MTItem*)ni;
}

void MTListBox::clearitems()
{
	MTItemView::clearitems();
	vs->pos = 0;
	updatescroller();
}

void MTListBox::endupdate()
{
	if (!updating) return;
	MTItemView::endupdate();
	updatescroller();
}

void MTListBox::updatescroller()
{
	if (!vs) return;
	if (ncontrols>0) vs->maxpos = controls[ncontrols-1]->top+controls[ncontrols-1]->height;
	else vs->maxpos = 0;
	vs->page = (height/16)*16;
	vs->incr = 16;
	vs->setposition(vs->pos);
}
//---------------------------------------------------------------------------
// MTControl
//   MTItem
//     MTMenuItem
//---------------------------------------------------------------------------
MTMenuItem::MTMenuItem(int tag,MTWinControl *p,int l,int t,int w,int h):
MTItem(tag,p,l,t,w,h),
command(0),
submenu(0),
shortcut(0),
hotkey(0),
hotkeyoffset(-1)
{
	guiid = MTC_MENUITEM;
}

void MTMenuItem::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	MTListBox &cparent = *(MTListBox*)parent;
	if (cparent.userdrawproc){
		cparent.userdrawproc(this,rect,b);
		MTControl::draw(rect);
		return;
	};
	skin->drawcontrol(this,rect,b,x,y);
	MTControl::draw(rect);
}

bool MTMenuItem::message(MTCMessage &msg)
{
	if (((msg.msg==MTCM_MOUSECLICK) || (msg.msg==MTCM_SHORTCUT)) && (!submenu) && (caption[0]!='|')){
		MTCMessage msg = {MTCM_ACTION,0,this,0};
		parent->message(msg);
		return true;
	};
	return MTItem::message(msg);
}

void MTMenuItem::setcaption(const char *c)
{
	char *e,*e1,*e2;
	int w = 12;
	int h = height;
	MTPoint p;
	
	strcpy(caption,c);
	e = caption;
	while ((e=strchr(e,'&'))){
		e1 = e;
		e2 = e+1;
		if (*e2=='&') e++;
		else{
			hotkey = *e2;
			hotkeyoffset = e-caption;
		};
		while (*e2){
			*e1++ = *e2++;
		};
		*e1 = 0;
	};
	if ((hotkey>='a') && (hotkey<='z')) hotkey += 'A'-'a';
	if (caption[0]!='|'){
		if (((MTListBox*)parent)->viewflags & MTVF_IMAGES) w += 18;
		skin->gettextsize(this,caption,-1,&p);
		w += p.x;
	}
	else if (caption[1]==0){
		h = 6;
	};
	if (w<32) w = 32;
	if (parent->width>w) w = parent->width;
	setbounds(left,top,w,h);
	if (parent->width<w) parent->setbounds(parent->left,parent->top,w,parent->height);
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTItemView
//       MTMenu
//---------------------------------------------------------------------------
MTMenu::MTMenu(int tag,MTWinControl *p,int l,int t,int w,int h):
MTItemView(tag,p,l,t,32,16),
ib(0)
{
	guiid = MTC_MENU;
	flags |= (MTCF_RAISED|MTCF_HIDDEN);
	viewflags |= MTVF_IMAGES;
	gi->setcontrolname(this,"menu");
	mtmemzero(&area,sizeof(area));
}

MTMenu::~MTMenu()
{
	if (ib) di->delbitmap(ib);
}

void MTMenu::setbounds(int l,int t,int w,int h)
{
	int x;
	bool resize = (w!=width);
	
	MTItemView::setbounds(l,t,w,h);
	if (!resize) return;
	for (x=0;x<ncontrols;x++){
		MTMenuItem &cmi = *(MTMenuItem*)controls[x];
		cmi.setbounds(cmi.left,cmi.top,w,cmi.height);
	};
}

void MTMenu::switchflags(int f,bool set)
{
	if (f & MTCF_HIDDEN){
		if (set==false){
			if (guiprefs.transpmenus){
				flags |= MTCF_TRANSPARENT;
				ib = di->newbitmap(MTB_DRAW,width+4,height+4);
			}
			else{
				flags &= (~MTCF_TRANSPARENT);
			};
		}
		else{
			if (ib){
				di->delbitmap(ib);
				ib = 0;
			};
		};
	};
	MTWinControl::switchflags(f,set);
}

void MTMenu::draw(MTRect &rect)
{
	MTRect cr = {0,0,width,height};
	MTBitmap *ob = mb;
	int obox = box;
	int oboy = boy;
	
	if (flags & MTCF_CANTDRAW) return;
	if (ib){
		mb = ib;
		box = 2;
		boy = 2;
	};
	if (&rect){
		if (!cliprect(cr,rect)) goto exit;
	};
	skin->drawcontrol(this,cr,0,0,0);
exit:
	MTWinControl::draw(rect);
	if (ib){
		mb = ob;
		box = obox;
		boy = oboy;
		obox = 0;
		oboy = 0;
		preparedraw(&ob,obox,oboy);
		ib->blendblt(ob,obox-2,oboy-2,width+4,height+4,0,0,192);
	};
}

bool MTMenu::message(MTCMessage &msg)
{
	int x;
	char key;
	MTMenuItem *cmi;

	switch (msg.msg){
	case MTCM_ACTION:
		cmi = (MTMenuItem*)msg.ctrl;
		if (dsk) dsk->clearmenu(0);
		if ((caller) && (caller->parent)){
			caller->parent->focus(caller);
		};
		if ((cmi) && (cmi->command)) cmi->command(0,cmi,0);
		return true;
	case MTCM_CHANGE:
		if ((msg.msg==MTCM_CHANGE) && (msg.ctrl->guiid==MTC_MENUITEM) && (msg.param1 & MTCF_OVER)){
			cmi = (MTMenuItem*)msg.ctrl;
			if (cmi->flags & MTCF_OVER){ 
				if ((cmi->submenu) && (dsk)){
					MTPoint p = {width,cmi->top};
					cmi->submenu->popup(this,p);
				}
				else dsk->clearmenu(this);
			};
		};
		break;
	case MTCM_CHAR:
		key = msg.key;
		if ((key>='a') && (key<='z')) key += 'A'-'a';
		if (msg.buttons==0){
			for (x=0;x<ncontrols;x++){
				cmi = (MTMenuItem*)controls[x];
				if (cmi->hotkey==key){
					if (dsk) dsk->clearmenu(0);
					if ((cmi) && (cmi->command)) cmi->command(0,cmi,0);
					return true;
				};
			};
		};
		break;
/*	case MTCM_LEAVE:
		break;*/
	};
	return MTItemView::message(msg);
}

MTItem *MTMenu::additem(const char *caption,int image,int flags,bool editable,void *data)
{
	MTMenuItem *ni;
	int top;
	
	if (ncontrols>0) top = controls[ncontrols-1]->top+controls[ncontrols-1]->height;
	else top = 0;
	ni = (MTMenuItem*)gi->newcontrol(MTC_MENUITEM,0,this,0,top,width,16,0);
	ni->index = ncontrols-1;
	ni->imageindex = image;
	ni->itemflags = flags;
	ni->editable = editable;
	ni->data = data;
	ni->setcaption(caption);
	setbounds(left,top,width,top+ni->height);
	return (MTItem*)ni;
}

void MTMenu::removeitem(MTItem *item)
{
	int x;
	int top;
	
	MTItemView::removeitem(item);
	width = 32;
	for (x=0;x<ncontrols;x++){
		MTMenuItem &cmi = *(MTMenuItem*)controls[x];
		cmi.setcaption(cmi.caption);
	};
	if (ncontrols>0) top = controls[ncontrols-1]->top+controls[ncontrols-1]->height;
	else top = 16;
	setbounds(left,top,width,top);
}

void MTMenu::popup(MTControl *newcaller,MTPoint pos)
{
	int px,py;
	MTMenu *cm = 0;

	if ((!dsk) || (ncontrols==0)) return;
	mouse = pos;
	area.right = area.left;
	area.bottom = area.top;
	caller = newcaller;
	MTCMessage msg = {MTCM_ONPOPUP,1,this};
	caller->message(msg);
	if (msg.result==0) return;
	if (caller->guiid & 2){
		px = ((MTWinControl*)caller)->box+pos.x+2;
		py = ((MTWinControl*)caller)->boy+pos.y;
	}
	else if (caller->parent){
		px = caller->parent->box+caller->left+pos.x+2;
		py = caller->parent->boy+caller->top+pos.y;
	};
	if (px<0) px = 0;
	else if (px>dsk->width-width) px = dsk->width-width;
	if (py<0) py = 0;
	else if (py>dsk->height-height) py = dsk->height-height;
	if (caller->guiid==MTC_MENU){
		cm = (MTMenu*)caller;
		if (cm->left+cm->width-16>px){
			px = cm->left-width;
			if (px<0) px = 0;
		};
	};
	dsk->clearmenu(cm);
	setbounds(px,py,width,height);
	dsk->setmenu(this);
}

void MTMenu::popup(MTControl *newcaller,MTRect area)
{
	popup(newcaller,mouse);
	this->area = area;
}
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTUserList
//       MTFileListBox
//---------------------------------------------------------------------------
MTFileListBox::MTFileListBox(int tag,MTWinControl *p,int l,int t,int w,int h):
MTUserList(tag,p,l,t,w,h),
entries(0),
process(0)
{
	getiteminfoproc = flb_getiteminfo;
	itemmessageproc = flb_itemmessage;
	viewflags |= MTVF_IMAGES;
	guiid = MTC_FILELISTBOX;
	gi->setcontrolname(this,"file");
	path = (char*)si->memalloc(1024,0);
	setpath(0);
}

MTFileListBox::~MTFileListBox()
{
	int x;

	if (process){
		process->terminate();
	};
	if (entries){
		for (x=0;x<entries->nitems;x++) si->memfree(((MTFileEntry*)entries->d)[x].filename);
		si->arraydelete(entries);
	};
	si->memfree(path);
}

void MTFileListBox::additem(const char *c,int type,int imageindex)
{
	MTFileEntry entry;

	entry.type = type;
	entry.imageindex = imageindex;
	entry.filename = (char*)si->memalloc(strlen(c)+1,0);
	strcpy(entry.filename,c);
	entries->push(&entry);
}

int MTFileListBox::flb_natsort(MTFileEntry *a,MTFileEntry *b)
{
	int ta,tb;

	ta = (a->type & MTFA_TYPEMASK);
	tb = (b->type & MTFA_TYPEMASK);
	if (ta>tb) return 1;
	if (ta<tb) return -1;
	return strnatcmp(a->filename,b->filename);
}

void MTFileListBox::sort(int f)
{
	entries->sort((SortProc)flb_natsort);
}

void MTFileListBox::setpath(const char *p)
{
	int x;
	char *s;

//TODO Linux
	if (process){
		gi->setmouseshape(DCUR_BACKGROUND);
		process->terminate();
		process = 0;
		gi->restoremouseshape();
	};
	setitem(-1);
	over = -1;
	if (p==(char*)1) strcpy(path,"Root");
	else if (p==0) path[0] = 0;
	else if (p[1]==':') strcpy(path,p);
	else{
		if (strcmp(p,".")==0) return;
		if (strcmp(p,"..")==0){
			if (path[0]==0) return;
			if (strcmp(path,"Root")==0) path[0] = 0;
			else if ((path[1]==':') && (path[3]==0)) strcpy(path,"Root");
			else{
				s = strrchr(path,'\\');
				if (s>path+1){
					if (s[-1]==':') s++;
					*s = 0;
				}
				else path[0] = 0;
			};
		}
		else{
			s = strchr(path,0);
			if (s>path){
				if (*(s-1)!='\\'){
					*s++ = '\\';
					*s = 0;
				};
			};
			strcat(path,p);
		};
	};
	if (entries){
		for (x=0;x<entries->nitems;x++) si->memfree(((MTFileEntry*)entries->d)[x].filename);
		si->arraydelete(entries);
	};
	entries = si->arraycreate(32,sizeof(MTFileEntry));
	if (path[0]) additem("..",0,52);
	setnumitems(entries->nitems);
	if (!design){
		process = si->processcreate(flb_process,this,MTP_LOADMODULE,MTT_LOWER,this,flb_progress,true,"FileListBox");
	};
}

void MTFileListBox::setfilter(int f)
{
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}

int MTFileListBox::flb_process(MTThread *thread,void *param)
{
	MTProcess *p = (MTProcess*)thread;
	MTFileListBox &flb = *(MTFileListBox*)param;
	MTFolder *folder;
	int x,attr,image;
	const char *file,*s;

	folder = si->folderopen(flb.path);
	x = si->syscounter();
	while ((!p->terminated) && (folder->getfile(&file,&attr,0))){
		attr &= MTFA_TYPEMASK;
		if (attr==MTFA_ROOT) image = 48;
		else if (attr==MTFA_DISK) image = 49;
		else if (attr==MTFA_FOLDER) image = 50;
		else{
			image = 51;
			s = (char*)strrchr(file,'.');
			if ((s) && (stricmp(s,".mt2")==0)) image = 53;
		};
		if (attr==MTFA_DISK){
			s = (char*)strchr(file,0);
			if (s>file+3) attr |= (s[-3]<<16);
		};
		flb.additem(file,attr,image);
		if (!folder->next()) break;
		if (si->syscounter()-x>=1000){
			p->setprogress(0.5);
			x = si->syscounter();
		};
	};
	si->folderclose(folder);
	return 0;
}

void MTFileListBox::flb_progress(MTProcess *process,void *param,float p)
{
	MTFileListBox &flb = *(MTFileListBox*)param;
	if (strcmp(flb.path,"Root")){
		flb.sort(0);
	};
	flb.setnumitems(flb.entries->nitems);
	if (p<0){
		flb.process = 0;
	};
}

int MTFileListBox::flb_getiteminfo(MTUserList *list,int id,char **caption,int *imageindex,int *flags,bool *editable)
{
	MTFileListBox &flb = *(MTFileListBox*)list;
	MTFileEntry &centry = ((MTFileEntry*)flb.entries->d)[id];

	if (caption) *caption = centry.filename;
	if (imageindex) *imageindex = centry.imageindex;
	return id;
}

void MTFileListBox::flb_itemmessage(MTUserList *list,int id,MTCMessage &msg)
{
	if (msg.msg!=MTCM_MOUSECLICK) return;
	MTFileListBox &flb = *(MTFileListBox*)list;
	MTFileEntry &centry = ((MTFileEntry*)flb.entries->d)[id];
	char root[4] = {"C:\\"};
	char *s;
	
	switch (centry.type & MTFA_TYPEMASK){
	case MTFA_ROOT:
		flb.setpath((char*)1);
		break;
	case MTFA_DISK:
		s = strchr(centry.filename,0);
		if (s>centry.filename+3){
			root[0] = s[-3];
			flb.setpath(root);
		};
		break;
	case MTFA_FILE:
		break;
	default:
		flb.setpath(centry.filename);
		break;
	};
}
//---------------------------------------------------------------------------
