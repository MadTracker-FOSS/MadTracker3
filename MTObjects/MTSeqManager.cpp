//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTSeqManager.cpp 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTSeqManager.h"
#include "MTAutomation.h"
#include "../MTGUI/MTGUITools.h"

//---------------------------------------------------------------------------
const char* autoc = ("Automation");

const char* layerc = ("Layer %d");

//---------------------------------------------------------------------------
// Module editor
//---------------------------------------------------------------------------
bool MTCT ModuleEdit(MTObject* object, MTWindow* window, int flags, MTUser* user)
{
    MTCustomWinControl* data;
    MTSeqManager* sm;

    if (window->uid == (int) object)
    {
        return true;
    }

    if (window->tag != MTO_MODULE)
    {
        while(window->ncontrols > 0)
        {
            gi->delcontrol(window->controls[0]);
        }
        data = (MTCustomWinControl*) gi->newcontrol(
            MTC_CUSTOMWINCTRL, 0, window, 2, 2, window->width - 4, window->height - 4, 0
        );
        if (!data)
        {
            return false;
        }
        data->flags |= MTCF_DONTSAVE;
        data->align = MTCA_CLIENT;
        data->uid = 100;
        sm = new MTSeqManager(data);
        data->behaviours = sm;
    }
    else
    {
        data = (MTCustomWinControl*) window->getcontrolfromuid(100);
        sm = (MTSeqManager*) data->behaviours;
    };
    sm->setmodule((MTModule*) object);
    return true;
}

//---------------------------------------------------------------------------
MTSeqManager::MTSeqManager(MTCustomWinControl* control):
    MTCustomWinBehaviours(control),
    module(0),
    selecting(0),
    followsong(true),
    offsetx(0),
    offsety(0),
    zoom(0.125),
    cursor(0),
    pattx(128),
    patty(16),
    patth(20)
{
    int tmp, ah;

    skin->getcontrolsize(MTC_SCROLLER, 0, tmp, ah);
    width = parent->width;
    height = parent->height;
    hs = (MTScroller*) gi->newcontrol(MTC_SCROLLER, 0, parent, pattx, height - ah, width - pattx, 0, 0);
    vs = (MTScroller*) gi->newcontrol(MTC_SCROLLER, 0, parent, pattx - ah, patty, 0, height - patty - ah, 0);
    parent->hs = hs;
    parent->vs = vs;
    updatemetrics();
    vs->type = MTST_VBAR;
    hs->switchflags(MTCF_SYSTEM | MTCF_DONTSAVE, true);
    vs->switchflags(MTCF_SYSTEM | MTCF_DONTSAVE, true);
    hs->align = MTCA_BOTTOM;
    vs->align = MTCA_LEFT;
    hl.x = -1;
    hl.y = -1;
    nbeats = (width - pattx) * zoom;
    nlayers = (height - hs->height - patty) / patth;
    clearselection(false);
    updatescroller();
}

void MTSeqManager::ondestroy()
{
    delete this;
}

void MTSeqManager::onsetbounds(int l, int t, int w, int h)
{
    double oldoffsetx = offsetx;
    double oldnbeats = nbeats;
    int oldoffsety = offsety;
    int oldnlayers = nlayers;

    if (w > 0)
    {
        width = w;
    }
    if (h > 0)
    {
        height = h;
    }
    nbeats = (w - pattx) * zoom;
    nlayers = (h - hs->height - patty) / patth;
    updatescroller();
    if ((nbeats != oldnbeats) || (nlayers != oldnlayers))
    {
        if (nlayers != oldnlayers)
        {
            MTCMessage msg = {MTCM_CHANGE, 0, parent};
            parent->parent->message(msg);
/*
			if (offsety==oldoffsety){
				drawlayer(oldnlayers,nlayers-oldnlayers);
				drawseq(0.0,nbeats,oldnlayers,nlayers-oldnlayers);
			};
*/
        };
    };
}

bool MTSeqManager::oncheckbounds(int& l, int& t, int& w, int& h)
{
    bool ok = true;

    if (w < pattx + 64)
    {
        w = pattx + 64;
        ok = false;
    };
    if (h < patty + 64)
    {
        h = patty + 64;
        ok = false;
    };
    return ok;
}

void MTSeqManager::ondraw(MTRect& rect)
{
    double x, w;
    int y, h;
    MTRect r = {0, 0, width, height};
    MTRect cr = {0, patty, pattx - vs->width, height - hs->height};

    if (&rect)
    {
        parent->clip(rect);
        r = rect;
    };
    parent->fillcolor(r.left, r.top, r.right - r.left, r.bottom - r.top, skin->getcolor(SC_EDIT_BACKGROUND));
    if (cliprect(cr, r))
    {
        y = (cr.top - patty) / patth;
        h = ((cr.bottom - patty + patth - 1) / patth) - y;
        drawlayer(y, h);
    };
    cr.left = pattx;
    cr.top = patty;
    cr.right = width;
    cr.bottom = height - hs->height;
    if (cliprect(cr, r))
    {
        x = (double) (cr.left - pattx) * zoom;
        y = (cr.top - patty) / patth;
        w = (double) (cr.right - cr.left) * zoom;
        h = ((cr.bottom - patty + patth - 1) / patth) - y;
        if (w > 0.0)
        {
            drawseq(x, w, y, h);
        }
    };
    if (&rect)
    {
        parent->unclip();
    };
}

bool MTSeqManager::onmessage(MTCMessage& msg)
{
    int x, layer;

    switch (msg.msg)
    {
        case MTCM_REFRESH:
            updatepos();
            return true;
        case MTCM_NOTIFY:
            updatemetrics();
            return true;
        case MTCM_MOUSEMOVE:
            if (parent->processmessage(msg))
            {
                return true;
            }
            x = msg.x;
            layer = msg.y;
            clienttosequence(x, layer);
            sethighlight(x, layer);
            return false;
        case MTCM_MOUSEDOWN:
            if (parent->processmessage(msg))
            {
                return true;
            }
            if (msg.buttons & DB_LEFT)
            {
                if (msg.x >= pattx)
                {
                    if (msg.y < patty)
                    {
                        if (module)
                        {
                            module->setpos(clienttodata(msg.x, 0));
                        }
                    }
                    else
                    {
                        x = 0;
                        if (msg.buttons & DB_CONTROL)
                        {
                            x = 1;
                        }
                        if (msg.buttons & DB_SHIFT)
                        {
                            x = 2;
                        }
                        selectsequence(hl.y, hl.x, x);
                        if ((msg.buttons & DB_DOUBLE) && (module) && (hl.x >= 0) && (hl.y >= 0))
                        {
                            x = module->sequ[hl.y][hl.x].patt;
                            if (x & 0x1000)
                            {
                                mtinterface->editobject(A(module->apatt, Automation)[x & 0xFFF], false);
                            }
                            else
                            {
                                mtinterface->editobject(A(module->patt, Pattern)[x], false);
                            }
                        };
                    };
                };
            };
            return true;
    }
    return false;
}

void* MTSeqManager::ongetoffsetrgn(int type)
{
    MTRect r = {pattx, patty, width, patty + nlayers * patth};
    void* rgn, * op;

    rgn = recttorgn(r);
    if (type == 0)
    {
        r.top = 0;
        r.bottom = patty;
        op = recttorgn(r);
        addrgn(rgn, op);
        deletergn(op);
    }
    else
    {
        r.left = 0;
        r.right = pattx - vs->width;
        op = recttorgn(r);
        addrgn(rgn, op);
        deletergn(op);
    };
    return rgn;
}

void MTSeqManager::onoffset(int ox, int oy)
{
    offsetx = hs->pos * zoom;
    offsety = vs->pos;
}

void MTSeqManager::setmodule(MTModule* newmodule)
{
    if (newmodule != module)
    {
        clearselection(false);
        module = newmodule;
        cursor = 0.0;
        setoffsetx(0);
        updatescroller();
        if (parent->parent)
        {
            MTCMessage msg = {MTCM_CHANGE, 0, parent};
            parent->parent->message(msg);
        };
    };
}

void MTSeqManager::setcursor(double value)
{
    double oldcursor = cursor;
    MTRect r1, r2;
    MTCMessage msg = {MTCM_CHANGE, 0, parent};
    MTWinControl* cparent = parent->parent;

    if ((value != cursor) && (cparent))
    {
        cursor = value;
        r1.top = r2.top = patty;
        r1.bottom = r2.bottom = r1.top + nlayers * patth;
        r1.left = datatoclient(oldcursor, 0);
        r2.left = datatoclient(cursor, 0);
        if (r1.left != r2.left)
        {
            r1.right = r1.left + 1;
            r1.left--;
            r2.right = r2.left + 1;
            r2.left--;
            if ((r2.left > r1.right + 16) || (r2.right < r1.left - 16))
            {
                msg.dr = r1;
                cparent->message(msg);
                msg.dr = r2;
                cparent->message(msg);
            }
            else if (r2.left >= r1.left)
            {
                r1.right = r2.right;
                msg.dr = r1;
                cparent->message(msg);
            }
            else
            {
                r1.left = r2.left;
                msg.dr = r1;
                cparent->message(msg);
            };
        };
        if (followsong)
        {
            if (cursor < offsetx)
            {
                setoffsetx(cursor - nbeats / 2);
            }
            if (cursor > offsetx + nbeats - nbeats / 8)
            {
                setoffsetx(cursor - nbeats / 8);
            }
        };
    };
}

void MTSeqManager::setoffsetx(double value)
{
    if (value != offsetx)
    {
        hs->setposition(value / zoom);
    }
}

void MTSeqManager::setoffsety(int value)
{
    if (value != offsety)
    {
        vs->setposition(value);
    }
}

void MTSeqManager::setzoom(int value)
{
    zoom = value;
    nbeats = (width - pattx) * zoom;
    updatescroller();
    if (parent->parent)
    {
        MTCMessage msg = {MTCM_CHANGE, 0, parent};
        parent->parent->message(msg);
    };
}

void MTSeqManager::sethighlight(int x, int layer)
{
    MTPoint oldh = hl;
    MTCMessage msg = {MTCM_CHANGE, 0, parent};

    if ((!module) || (!parent))
    {
        return;
    }
    if ((x != hl.x) || (layer != hl.y))
    {
        hl.x = x;
        hl.y = layer;
        if (oldh.x >= 0)
        {
            sequencetorect(oldh.y, oldh.x, msg.dr);
            parent->parent->message(msg);
        };
        if ((x >= 0) && (layer >= 0) && (layer < MAX_LAYERS))
        {
            sequencetorect(layer, x, msg.dr);
            parent->parent->message(msg);
        }
        else
        {
            hl.x = -1;
            hl.y = -1;
        };
    };
}

void MTSeqManager::updatepos()
{
    if (module)
    {
        if ((vs->slide) || (hs->slide))
        {
            return;
        }
        setcursor(module->playstatus.pos);
    };
}

double MTSeqManager::clienttodata(int x, int* layer)
{
    double beat;

    x -= pattx;
    if ((x < 0) || (x >= width - pattx))
    {
        return -1.0;
    }
    beat = ((double) x * zoom) + offsetx;
    if (layer)
    {
        *layer = (*layer - patty) / patth;
        if ((*layer < 0) || (*layer >= nlayers))
        {
            *layer = -1;
            return -1.0;
        };
        *layer += offsety;
    };
    return beat;
}

int MTSeqManager::datatoclient(double x, int* layer)
{
    int line;

    if (x < 0.0)
    {
        return -1;
    }
    line = (int) ((x - offsetx) / zoom);
    if ((line < 0) || (line >= width - pattx))
    {
        return -1;
    }
    line += pattx;
    if (layer)
    {
        *layer -= offsety;
        if ((*layer < 0) || (*layer >= nlayers))
        {
            *layer = -1;
            return -1;
        };
        *layer = *layer * patth + patty;
    };
    return line;
}

void MTSeqManager::clienttosequence(int& x, int& layer)
{
    x -= pattx;
    layer -= patty;
    if ((!module) || (layer < 0) || (x < 0) || (x >= width - pattx))
    {
        goto error;
    }
    layer /= patth;
    if (layer >= nlayers)
    {
        goto error;
    }
    x = module->getsequence(layer + offsety, (double) x * zoom + offsetx, 0);
    layer += offsety;
    return;
    error:
    x = -1;
    layer = -1;
}

void MTSeqManager::selectsequence(int layer, int s, int how)
{
    int x, y;
    MTCMessage msg = {MTCM_CHANGE, 0, parent};

    if (!module)
    {
        return;
    }
    switch (how)
    {
// Replace selection
        case 0:
            for(y = 0; y < MAX_LAYERS; y++)
            {
                for(x = 0; x < MAX_SEQUENCES; x++)
                {
                    module->sequ[y][x].flags &= (~SF_SELECTED);
                };
            };
            if ((layer >= 0) && (s >= 0))
            {
                module->sequ[layer][s].flags |= SF_SELECTED;
            }
            break;
// Toggle selection
        case 1:
            if ((layer >= 0) && (s >= 0))
            {
                module->sequ[layer][s].flags ^= SF_SELECTED;
            }
            sequencetorect(layer, s, msg.dr);
            break;
// Area selection
        case 2:
            for(y = 0; y < MAX_LAYERS; y++)
            {
                for(x = 0; x < MAX_SEQUENCES; x++)
                {
                    module->sequ[y][x].flags &= (~SF_SELECTED);
                };
            };
            if ((layer >= 0) && (s >= 0))
            {

            };
            break;
    };
    if (parent->parent)
    {
        parent->parent->message(msg);
    };
}

void MTSeqManager::clearselection(bool refresh)
{
    int x, y;

    if (module)
    {
        for(y = 0; y < MAX_LAYERS; y++)
        {
            for(x = 0; x < MAX_SEQUENCES; x++)
            {
                module->sequ[y][x].flags &= (~SF_SELECTED);
            };
        };
    };
    if (parent->parent)
    {
        MTCMessage msg = {MTCM_CHANGE, 0, parent};
        parent->parent->message(msg);
    };
}

void MTSeqManager::deletesequence(int layer, int s, bool adapt)
{
    int x;
    double l, ss, se;

    if ((layer < 0) || (layer >= MAX_LAYERS))
    {
        return;
    }
    ss = (module->sequ[layer][s].pos - offsetx) / zoom;
    se = module->sequ[layer][s].length / zoom;
    if (adapt)
    {
        l = module->sequ[layer][s].length;
        for(x = s; x < MAX_SEQUENCES - 1; x++)
        {
            module->sequ[layer][x] = module->sequ[layer][x + 1];
            module->sequ[layer][x].pos -= l;
        };
    }
    else
    {
        for(x = s; x < MAX_SEQUENCES - 1; x++)
        {
            module->sequ[layer][x] = module->sequ[layer][x + 1];
        }
    };
    memset(&module->sequ[layer][MAX_SEQUENCES - 1], 0, sizeof(Sequence));
    module->nsequ[layer]--;
    module->updatelength();
    if (adapt)
    {
        drawseq(0, 0, layer - offsety, 1);
    }
    else
    {
        drawseq(ss, se, layer - offsety, 1);
    };
}

void MTSeqManager::updatemetrics()
{
    int tmp, ah;
    MTPoint ts;

    skin->getcontrolsize(MTC_SCROLLER, 0, tmp, ah);
    parent->open(0);
    parent->setfont(skin->getfont(1));
    parent->gettextsize(autoc, -1, &ts);
    parent->close(0);
    pattx = ts.x + 8 + ah;
    parent->flags |= MTCF_DONTRESIZE;
    hs->setbounds(pattx, height - ah, width - pattx, hs->height);
    vs->setbounds(pattx - ah, patty, vs->width, height - patty - ah);
    parent->flags &= (~MTCF_DONTRESIZE);
}

void MTSeqManager::updatescroller()
{
    hs->os = 1;
    vs->os = patth;
    if (module)
    {
        hs->maxpos = module->loope / zoom;
        hs->incr = 4;
        hs->page = (nbeats / zoom) * 0.75;
        if (hs->page < hs->maxpos)
        {
            hs->maxpos += (nbeats / zoom) * 0.5;
        };
        if (offsetx / zoom + hs->page > hs->maxpos)
        {
            setoffsetx((hs->maxpos - hs->page) * zoom);
        };
        vs->maxpos = MAX_LAYERS;
        vs->incr = 1;
        vs->page = nlayers;
        if (offsety + vs->page > vs->maxpos)
        {
            setoffsety(vs->maxpos - vs->page);
        };
    }
    else
    {
        hs->maxpos = 0;
        hs->incr = 0;
        hs->page = 0;
        hs->setposition(0);
        vs->maxpos = 0;
        vs->incr = 0;
        vs->page = 0;
        vs->setposition(0);
    };
}

void MTSeqManager::drawlayer(int sy, int sh)
{
    int ah = vs->width;
    int x;
    MTRect cr;
    char layers[64];

    if (sy + offsety >= MAX_LAYERS)
    {
        return;
    }
    if (sy < 0)
    {
        if (sy + sh >= 0)
        {
            sh += sy;
            sy = 0;
        }
        else
        {
            return;
        }
    };
    if (sy + sh > nlayers)
    {
        if (sy >= nlayers)
        {
            return;
        }
        else
        {
            sh = nlayers - sy;
        }
    };
    parent->fillcolor(0, sy * patth + patty, pattx - ah, sh * patth, skin->getcolor(SC_BACKGROUND));
    cr.left = 4;
    cr.right = pattx - ah - 4;
    sy += offsety;
    if (parent->open(0))
    {
        parent->settextcolor(skin->getcolor(SC_TEXT_NORMAL));
        parent->setfont(skin->getfont(1));
        if (!sy)
        {
            cr.top = patty;
            cr.bottom = patty + patth - 1;
            parent->drawtext(autoc, -1, cr, DTXT_VCENTER);
        };
        for(x = sy; x < sy + sh; x++)
        {
            if (x >= MAX_LAYERS)
            {
                break;
            }
            cr.top = patty + (x - offsety) * patth;
            cr.bottom = cr.top + patth - 1;
            if (x)
            {
                sprintf(layers, layerc, x);
                parent->drawtext(layers, -1, cr, DTXT_VCENTER);
            };
            parent->setpen(skin->getcolor(SC_CTRL_L));
            parent->moveto(0, cr.top);
            parent->lineto(pattx - ah, cr.top);
            parent->setpen(skin->getcolor(SC_CTRL_S));
            parent->moveto(0, cr.bottom);
            parent->lineto(pattx - ah, cr.bottom);
        };
        parent->close(0);
    };
}

void MTSeqManager::drawseqp(int layer, int s, double sl, double el)
{
    int w, y, sx, id;
    double cl, lng, stl, nl;
    unsigned short p, t;
    Pattern* cpatt;
    Automation* capatt;
    char* pattname;
    unsigned char f;
    char pattid[8];
    MTRect cr, br;
    MTPoint pt;
    int bx = 0;
    int by = 0;
    MTBitmap* b;

    if (s < 0)
    {
        return;
    }
    if ((sl == 0) && (el == 0))
    {
        sl = offsetx;
        el = nbeats + offsetx;
    };
    p = module->sequ[layer][s].patt;
    f = module->sequ[layer][s].flags;
    t = p >> 12;
    p &= 0xFFF;
//	if (p>=MAX_PATTS) return;
    cl = module->sequ[layer][s].pos;
    lng = module->sequ[layer][s].length;
    if (lng <= 0)
    {
        return;
    }
    stl = cl;
    nl = lng;
    y = patty + patth * (layer - offsety);
    switch (t)
    {
        case 0:
            if (!module->patt->a[p])
            {
                return;
            }
            cpatt = A(module->patt, Pattern)[p];
            nl = cpatt->nbeats;
            id = cpatt->id;
            pattname = cpatt->name;
            break;
        case 1:
            capatt = A(module->apatt, Automation)[p];
            if (!capatt)
            {
                return;
            }
            id = capatt->id;
            pattname = capatt->name;
            break;
        default:
            return;
    };
    if (el >= cl + lng)
    {
        el = cl + lng - zoom;
    }
    if (sl < cl)
    {
        sl = cl;
    }
    cr.left = (int) ((sl - offsetx) / zoom) + pattx;
    cr.right = (int) ((el - offsetx + zoom) / zoom) + pattx;
    cr.top = y;
    cr.bottom = y + patth;
    parent->clip(cr);
    sx = (int) ((stl - offsetx) / zoom) + pattx;
    w = (int) (lng / zoom);
    parent->preparedraw(&b, bx, by);
    if (f & SF_DISABLED)
    {
        f = 3;
    }
    else if ((hl.x == s) && (hl.y == layer))
    {
        f = 1;
    }
    else if (f & SF_SELECTED)
    {
        f = 2;
    }
    else
    {
        f = 0;
    }
    br.left = sx + bx;
    br.top = y + by;
    br.right = br.left + w;
    br.bottom = br.top + patth;
    skin->drawcontrol(MTC_SEQUENCE, f, br, b, 0, 0);
    if (parent->open(0))
    {
        if ((nl > 0) && (w >= 16) && (w - (int) ((lng / nl) / zoom) > 1))
        {
            while(lng > 0)
            {
                if (nl > lng)
                {
                    nl = lng;
                }
                if (cl != stl)
                {
                    sx = (int) ((cl - offsetx) / zoom) + pattx;
                    parent->setpen(skin->getcolor(SC_CTRL_S));
                    parent->moveto(sx - 1, y + 2);
                    parent->lineto(sx - 1, y + patth - 3);
                    parent->setpen(skin->getcolor(SC_CTRL_L));
                    parent->moveto(sx, y + 2);
                    parent->lineto(sx, y + patth - 3);
                };
                cl += nl;
                lng -= nl;
            };
        };
        sx = (int) ((stl - offsetx) / zoom) + pattx;
        parent->settextcolor(skin->getcolor(SC_CTRL_S));
        parent->setfont(skin->getfont(1));
        if (w >= 16)
        {
            if ((parent->gettextsize(pattname, -1, &pt)) && (pt.x <= w - 16))
            {
                cr.left = sx + 8;
                cr.top = y;
                cr.right = sx + w - 8;
                cr.bottom = y + 15;
                parent->drawtext(pattname, -1, cr, DTXT_VCENTER);
            }
            else
            {
                mtmemzero(pattid, sizeof(pattid));
                sprintf(pattid, "%d", id);
                if ((parent->gettextsize(pattid, -1, &pt)) && (pt.x <= w - 16))
                {
                    cr.left = sx + 8;
                    cr.top = y;
                    cr.right = sx + w - 8;
                    cr.bottom = y + 15;
                    parent->drawtext(pattid, -1, cr, DTXT_VCENTER);
                };
            };
        };
        parent->close(0);
    };
    parent->unclip();
}

bool MTSeqManager::sequencetorect(int layer, int s, MTRect& r)
{
    unsigned short p, t;
    double cl, lng;
    int bx = 0;
    int by = 0;

    if ((s < 0) || (layer < 0))
    {
        return false;
    }
    p = module->sequ[layer][s].patt;
    t = p >> 12;
    p &= 0xFFF;
//	if (p>=MAX_PATTS) return false;
    cl = module->sequ[layer][s].pos;
    lng = module->sequ[layer][s].length;
    if (lng <= 0)
    {
        return false;
    }
    r.left = (int) ((cl - offsetx) / zoom) + pattx;
    r.right = (int) ((cl + lng - offsetx + zoom) / zoom) + pattx;
    r.top = patty + patth * (layer - offsety);
    r.bottom = r.top + patth;
    return true;
}

void MTSeqManager::drawseq(double sx, double sw, int sy, int sh)
{
    int x, y;
    int sseq, eseq;
    double sl, el;

    if ((sx == 0) && (sw == 0))
    {
        sw = nbeats;
    }
    if (sx < 0)
    {
        if (sx + sw >= 0)
        {
            sw += sx;
            sx = 0;
        }
        else
        {
            return;
        }
    };
    if (sx + sw > nbeats)
    {
        if (sx >= nbeats)
        {
            return;
        }
        else
        {
            sw = nbeats - sx;
        }
    };
    if (sy < 0)
    {
        if (sy + sh >= 0)
        {
            sh += sy;
            sy = 0;
        }
        else
        {
            return;
        }
    };
    if (sy + sh > nlayers)
    {
        if (sy >= nlayers)
        {
            return;
        }
        else
        {
            sh = nlayers - sy;
        }
    };
//	parent->fillcolor(sx/zoom+pattx,sy*patth+patty,sw/zoom,sh*patth,skin->getcolor(SC_EDIT_BACKGROUND));
    if (module)
    {
        sl = sx + offsetx;
        el = sx + sw + offsetx;
        for(x = offsety + sy; x < offsety + sy + sh; x++)
        {
            sseq = module->getsequence(x, sl, 1);
            eseq = module->getsequence(x, el, 2);
            if (eseq < 0)
            {
                continue;
            }
            if (sseq < 0)
            {
                sseq = eseq;
            }
            for(y = sseq; y <= eseq; y++)
            {
                drawseqp(x, y, sl, el);
            }
        };
        if ((cursor >= sl) && (cursor < el))
        {
            parent->open(0);
            parent->setbrush(skin->getcolor(SC_CURSOR));
            parent->fillrect(pattx + ((cursor - offsetx) / zoom), patty + sy * patth, 1, sh * patth, MTBM_INVERT);
            parent->close(0);
        };
    };
}
//---------------------------------------------------------------------------
