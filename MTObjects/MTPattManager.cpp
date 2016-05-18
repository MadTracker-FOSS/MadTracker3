//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTPattManager.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTPattEditor.h"
#include "../MTGUI/MTGUITools.h"
#include <MTXAPI/RES/MTObjectsRES.h>

//---------------------------------------------------------------------------
MTPattManager::MTPattManager(MTCustomWinControl* control):
    MTCustomWinBehaviours(control),
    hs(0),
    vs(0),
    patt(0),
    selecting(false),
    otrack(0),
    oline(0),
    pattx(0),
    patty(0),
    ccol(0),
    cpos(0),
    tw(0),
    drag(false),
    clpb(-1)
{
    int tmp, ah;

    mtmemzero(colwidth, sizeof(colwidth));
    mtmemzero(colncpos, sizeof(colncpos));
    mtmemzero(colsize, sizeof(colsize));
    width = parent->width;
    height = parent->height;
    updatemetrics();
    skin->getcontrolsize(MTC_SCROLLER, 0, tmp, ah);
    hs = (MTScroller*) gi->newcontrol(MTC_SCROLLER, 0, parent, pattx, height - ah, width - pattx, 0, 0);
    vs = (MTScroller*) gi->newcontrol(MTC_SCROLLER, 0, parent, pattx - ah, patty, 0, height - patty - ah, 0);
    parent->hs = hs;
    parent->vs = vs;
    vs->type = MTST_VBAR;
    hs->switchflags(MTCF_SYSTEM | MTCF_DONTSAVE, true);
    vs->switchflags(MTCF_SYSTEM | MTCF_DONTSAVE, true);
    hs->align = MTCA_BOTTOM;
    vs->align = MTCA_LEFT;
    updatescroller();
    hl.x = -1;
    hl.y = -1;
    cursor.x = 0;
    cursor.y = 0;
    selstart.x = -1;
    selstart.y = -1;
    selend.x = -1;
    selend.y = -1;
    m.x = -1;
}

void MTPattManager::ondestroy()
{
    delete this;
}

void MTPattManager::onsetbounds(int l, int t, int w, int h)
{
    int ontracks, onlines;

    if (w > 0)
    {
        width = w;
    }
    if (h > 0)
    {
        height = h;
    }
    ontracks = ntracks;
    onlines = nlines;
    updatemetrics(true);
    if (ntracks > ontracks)
    {
        drawtrack(ontracks, ntracks - ontracks);
        drawcells(ontracks, 0, ntracks - ontracks, nlines);
    };
    if (nlines > onlines)
    {
        drawpos(onlines, nlines - onlines);
        drawcells(0, onlines, ntracks, nlines - onlines);
    };
}

bool MTPattManager::oncheckbounds(int& l, int& t, int& w, int& h)
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

void MTPattManager::ondraw(MTRect& rect)
{
    int x, y, w, h, ch, tw;
    MTRect r;

    checkcolors();
    if (!&rect)
    {
        drawpos(0, nlines);
        drawtrack(0, ntracks);
        drawcells(0, 0, ntracks, nlines);
    }
    else
    {
        parent->clip(rect);
        ch = nlines * cellheight;
        r.left = 0;
        r.top = patty;
        r.right = pattx;
        r.bottom = patty + ch;
        if (cliprect(r, rect))
        {
            y = (r.top - patty) / cellheight;
            h = (r.bottom - patty + cellheight - 1) / cellheight;
            drawpos(y, h - y);
        };
        r.left = pattx;
        r.right = pattx + cw;
        if (rect.left >= r.left)
        {
            r.left = rect.left;
        }
        if (rect.right < r.right)
        {
            r.right = rect.right;
        }
        if (r.right > r.left)
        {
            r.left -= pattx;
            r.right -= pattx;
            x = -1;
            w = ntracks;
            tw = 0;
            for(y = otrack; y < otrack + ntracks; y++)
            {
                tw += cellwidth[y];
                if ((tw >= r.left) && (x < 0))
                {
                    x = y - otrack;
                }
                if (tw >= r.right)
                {
                    w = y - otrack;
                    break;
                };
            };
            if (x < 0)
            {
                x = 0;
            }
            if ((rect.top < patty) && (rect.bottom >= 0))
            {
                drawtrack(x, w - x + 1);
            }
            r.top = patty;
            r.bottom = patty + ch;
            if (rect.top >= r.top)
            {
                r.top = rect.top;
            }
            if (rect.bottom < r.bottom)
            {
                r.bottom = rect.bottom;
            }
            if (r.bottom > r.top)
            {
                y = (r.top - patty) / cellheight;
                h = (r.bottom - patty + cellheight - 1) / cellheight;
                drawcells(x, y, w - x + 1, h - y);
            };
        };
        parent->unclip();
    };
}

bool MTPattManager::onmessage(MTCMessage& msg)
{
    int x, c, n, coffset;
    unsigned char* cdata;
    MTPoint mp = {msg.x, msg.y};
    MTPoint mp2;

    if (!patt)
    {
        return false;
    }
    c = clienttodata(mp);
    coffset = 0;
    switch (msg.msg)
    {
        case MTCM_MOUSEDOWN:
            m.x = msg.x;
            m.y = msg.y;
            if (msg.y < patty)
            {
                if (mp.x < 0)
                {
                    return true;
                }
                int bh;
                skin->getcontrolsize(MTC_STATUS, 0, x, bh);
                mp2 = mp;
                datatoclient(mp2);
                mp2.x = msg.x - mp2.x;
                if ((mp2.x >= tw) && (mp2.x < tw + bh * 3 + 6))
                {
                    if (mp2.x < tw + bh + 2)
                    {
                        patt->tracks[mp.x].on ^= true;
                    }
                    else if (mp2.x < tw + bh * 2 + 4)
                    {
                        patt->tracks[mp.x].solo ^= true;
                    }
                    else
                    {
                        patt->tracks[mp.x].rec ^= true;
                    }
                    if (parent->parent)
                    {
                        MTCMessage msg = {MTCM_CHANGE, 0, parent};
                        gettrackrect(mp.x - otrack, 1, msg.dr);
                        parent->parent->message(msg);
                    };
                };
                return true;
            };
            if (msg.x >= pattx)
            {
                if ((mp.x < 0) || (mp.y < 0))
                {
                    if ((mp.x >= 0) && (msg.buttons & DB_DOUBLE))
                    {
                        mp.y = 0;
                        setselstart(mp);
                        mp.y = patt->nlines - 1;
                        setselend(mp);
                    };
                    return false;
                };
                for(x = 0; x < mp.x; x++)
                {
                    TrackInfo& ti = patt->tracks[x];
                    for(n = 0; n < ti.ncolumns; n++)
                    {
                        coffset += colsize[x][n];
                    }
                };
                cdata = patt->data + mp.y * patt->linesize + coffset;
                if (c >= 0)
                {
                    mp2 = mp;
                    datatoclient(mp2);
                    msg.x -= mp2.x;
                    msg.y -= mp2.y;
                    patt->tracks[mp.x].cols[c].handler->onmessage(this, msg, -1, cdata);
                    msg.x += mp2.x;
                    msg.y += mp2.y;
                };
            };
            break;
        case MTCM_MOUSEUP:
            if (!drag)
            {
                if ((msg.x >= pattx) && (mp.x >= 0) && (mp.y >= 0))
                {
                    ccol = 0;
                    setcursor(mp.y, mp.x, 0);
                };
            };
            m.x = -1;
            drag = false;
            break;
        case MTCM_MOUSEMOVE:
            if (parent->processmessage(msg))
            {
                return true;
            }
            if ((!hs->slide) && (!vs->slide))
            {
                if ((!drag) && (m.x >= 0) && (gi->isdragged(msg.p, m)))
                {
                    drag = true;
                    setselstart(mp);
                }
                else if (drag)
                {
                    setselend(mp);
                };
                mp.x -= otrack;
                mp.y -= oline;
                sethighlight(mp);
            };
            break;
        case MTCM_KEYDOWN:
            if (!patt)
            {
                return false;
            }
            if ((msg.s) && (msg.s->group == peksgroup))
            {
                switch (msg.s->user)
                {
                    case MTT_nextline:
                        setcursor(cursor.y + 1, cursor.x, cpos);
                        return true;
                    case MTT_prevline:
                        setcursor(cursor.y - 1, cursor.x, cpos);
                        return true;
                    case MTT_nexttrack:
                        setcursor(cursor.y, cursor.x + 1, cpos);
                        return true;
                    case MTT_prevtrack:
                        setcursor(cursor.y, cursor.x - 1, cpos);
                        return true;
                    case MTT_firstline:
                        setcursor(0, cursor.x, cpos);
                        return true;
                    case MTT_lastline:
                        setcursor(patt->nlines - 1, cursor.x, cpos);
                        return true;
                    case MTT_nextbeat:
                        setcursor(cursor.y + patt->lpb, cursor.x, cpos);
                        return true;
                    case MTT_prevbeat:
                        setcursor(cursor.y - patt->lpb, cursor.x, cpos);
                        return true;
                    case MTT_right:
                        setcursor(cursor.y, cursor.x, cpos + 1);
                        return true;
                    case MTT_left:
                        setcursor(cursor.y, cursor.x, cpos - 1);
                        return true;
                };
            };
        case MTCM_CHAR:
            for(x = 0; x < cursor.x; x++)
            {
                TrackInfo& ti = patt->tracks[x];
                for(n = 0; n < ti.ncolumns; n++)
                {
                    coffset += colsize[x][n];
                }
            };
            for(n = 0; n < ccol; n++)
            {
                coffset += colsize[cursor.x][n];
            }
            cdata = patt->data + cursor.y * patt->linesize + coffset;
            patt->tracks[cursor.x].cols[ccol].handler->onmessage(this, msg, cpos, cdata);
            break;
        case MTCM_NOTIFY:
            updatemetrics();
            return true;
    };
    return false;
}

void* MTPattManager::ongetoffsetrgn(int type)
{
    MTRect r = {pattx, patty, pattx + cw, patty + nlines * cellheight};
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

void MTPattManager::onoffset(int ox, int oy)
{
    MTPoint p = {-1, -1};

    sethighlight(p);
    otrack = hs->pos;
    oline = vs->pos - ((nlines - 1) >> 1);
}

void MTPattManager::setpattern(MTPattern* newpatt)
{
    int t, x, n;

    patt = newpatt;
    selecting = false;
    if (patt)
    {
        x = 0;
        for(t = 0; t < patt->ntracks; t++)
        {
            TrackInfo& ti = patt->tracks[t];
            for(n = 0; n < ti.ncolumns; n++)
            {
                colwidth[t][n] = ti.cols[n].handler->getwidth(skin->fontwidth);
                colncpos[t][n] = ti.cols[n].handler->ncpos;
                colsize[t][n] = ti.cols[n].handler->nbytes;
                x += colwidth[t][n];
            };
        };
    };
    updatemetrics();
    if (parent->parent)
    {
        updatescroller();
        MTCMessage msg = {MTCM_CHANGE, 0, parent};
        parent->parent->message(msg);
    };
}

void MTPattManager::setcursor(int l, int t, int p)
{
    MTPoint oldc = cursor;
    MTCMessage msg = {MTCM_CHANGE, 0, parent};

    if ((cursor.x != t) || (cursor.y != l) || (cpos != p))
    {
        if (cpos != p)
        {
            if (p < 0)
            {
                if (ccol-- == 0)
                {
                    t--;
                    while(t < 0)
                    {
                        t += patt->ntracks;
                    }
                    ccol = patt->tracks[t].ncolumns - 1;
                };
                cpos = colncpos[t][ccol] - 1;
            }
            else if (p >= colncpos[t][ccol])
            {
                cpos = 0;
                if (++ccol == patt->tracks[cursor.x].ncolumns)
                {
                    t++;
                    if (t >= patt->ntracks)
                    {
                        t %= patt->ntracks;
                    }
                    ccol = 0;
                };
            }
            else
            {
                cpos = p;
            };
        };
        while(t < 0)
        {
            t += patt->ntracks;
        }
        if (t >= patt->ntracks)
        {
            t %= patt->ntracks;
        }
        while(l < 0)
        {
            l += patt->nlines;
        }
        if (l >= patt->nlines)
        {
            l %= patt->nlines;
        }
        cursor.x = t;
        cursor.y = l;
        if (parent->parent)
        {
            getcellsrect(oldc.x - otrack, oldc.y - oline, 1, 1, msg.dr);
            parent->parent->message(msg);
        };
        if (t < otrack)
        {
            setotrack(t);
        }
        else if (t >= otrack + ntracks)
        {
            setotrack(t - ntracks + 1);
        }
        if (l < oline)
        {
            setoline(l);
        }
        else if (l >= oline + nlines)
        {
            setoline(l - nlines + 1);
        }
        if (parent->parent)
        {
            getcellsrect(cursor.x - otrack, cursor.y - oline, 1, 1, msg.dr);
            parent->parent->message(msg);
        };
    };
}

void MTPattManager::setotrack(int value)
{
    if (patt)
    {
        if (value > patt->ntracks - ntracks)
        {
            value = patt->ntracks - ntracks;
        }
        if (value < 0)
        {
            value = 0;
        }
    }
    else
    {
        value = 0;
    };
    hs->setposition(value);
}

void MTPattManager::setoline(int value)
{
    int nu = (nlines - 1) >> 1;

    if (patt)
    {
        if (value > patt->nlines - 1 - nu)
        {
            value = patt->nlines - 1 - nu;
        }
        if (value < -nu)
        {
            value = -nu;
        }
        vs->setposition(value + nu);
    }
    else
    {
        vs->setposition(0);
    };
}

void MTPattManager::sethighlight(MTPoint value)
{
    MTPoint oldh = hl;

    if ((value.x != hl.x) || (value.y != hl.y))
    {
        hl = value;
        if (parent->parent)
        {
            MTCMessage msg = {MTCM_CHANGE, 0, parent};
            if (oldh.x >= 0)
            {
                getcellsrect(oldh.x, oldh.y, 1, 1, msg.dr);
                parent->parent->message(msg);
            };
            getcellsrect(hl.x, hl.y, 1, 1, msg.dr);
            parent->parent->message(msg);
        };
    };
}

void MTPattManager::getsel(MTRect& sel)
{
    MTRect r = {selstart.x, selstart.y, selend.x, selend.y};

    minmax(r.left, r.right);
    minmax(r.top, r.bottom);
    sel = r;
}

void MTPattManager::setsel(MTRect value)
{
    MTPoint s, e;

    if (value.left < 0)
    {
        value.left = 0;
    }
    else if (value.left >= patt->ntracks)
    {
        value.left = patt->ntracks - 1;
    }
    if (value.top < 0)
    {
        value.top = 0;
    }
    else if (value.top >= patt->nlines)
    {
        value.top = patt->nlines - 1;
    }
    if (value.right < 0)
    {
        value.right = 0;
    }
    else if (value.right >= patt->ntracks)
    {
        value.right = patt->ntracks - 1;
    }
    if (value.bottom < 0)
    {
        value.bottom = 0;
    }
    else if (value.bottom >= patt->nlines)
    {
        value.bottom = patt->nlines - 1;
    }
    s = selstart;
    e = selend;
    pminmax(s, e);
    selstart.x = value.left;
    selstart.y = value.top;
    selend.x = value.right;
    selend.y = value.bottom;
    if (parent->parent)
    {
        MTCMessage msg = {MTCM_CHANGE, 0, parent};
        getcellsrect(s.x - otrack, s.y - oline, e.x - s.x + 1, e.y - s.y + 1, msg.dr);
        parent->parent->message(msg);
        getcellsrect(
            value.left - otrack, value.top - oline, value.right - value.left + 1, value.bottom - value.top + 1, msg.dr
        );
        parent->parent->message(msg);
    };
}

void MTPattManager::clearsel()
{
    selstart.x = -1;
    selstart.y = -1;
    selend.x = -1;
    selend.y = -1;
    if (parent->parent)
    {
        MTCMessage msg = {MTCM_CHANGE, 0, parent};
        parent->parent->message(msg);
    };
}

void MTPattManager::setselstart(MTPoint value)
{
    MTPoint s, e;

    if (!patt)
    {
        return;
    }
    if (value.x < 0)
    {
        value.x = 0;
    }
    else if (value.x >= patt->ntracks)
    {
        value.x = patt->ntracks - 1;
    }
    if (value.y < 0)
    {
        value.y = 0;
    }
    else if (value.y >= patt->nlines)
    {
        value.y = patt->nlines - 1;
    }
    s = selstart;
    e = selend;
    pminmax(s, e);
    selstart = value;
    selend = value;
    if (parent->parent)
    {
        MTCMessage msg = {MTCM_CHANGE, 0, parent};
        getcellsrect(s.x - otrack, s.y - oline, e.x - s.x + 1, e.y - s.y + 1, msg.dr);
        parent->parent->message(msg);
        getcellsrect(value.x - otrack, value.y - oline, 1, 1, msg.dr);
        parent->parent->message(msg);
    };
}

void MTPattManager::setselend(MTPoint value)
{
    MTPoint s, e;
    MTPoint ns, ne;
    int x, y, minx, maxx, miny, maxy;
    bool in1, in2;

    if (!patt)
    {
        return;
    }
    if (value.x < 0)
    {
        value.x = 0;
    }
    else if (value.x >= patt->ntracks)
    {
        value.x = patt->ntracks - 1;
    }
    if (value.y < 0)
    {
        value.y = 0;
    }
    else if (value.y >= patt->nlines)
    {
        value.y = patt->nlines - 1;
    }
    if (selecting)
    {
        if (value.x < otrack)
        {
            setotrack(value.x);
        }
        else if (value.x >= ntracks + otrack)
        {
            setotrack(value.x - ntracks + 1);
        }
        if (value.y < oline)
        {
            setoline(value.y);
        }
        else if (value.y >= nlines + oline)
        {
            setoline(value.y - nlines + 1);
        }
    };
    s = selstart;
    e = selend;
    ns = selstart;
    ne = value;
    selend = value;
    pminmax(s, e);
    pminmax(ns, ne);
    minx = s.x;
    if (ns.x < minx)
    {
        minx = ns.x;
    }
    maxx = e.x;
    if (ne.x > maxx)
    {
        maxx = ne.x;
    }
    miny = s.y;
    if (ns.y < miny)
    {
        miny = ns.y;
    }
    maxy = e.y;
    if (ne.y > maxy)
    {
        maxy = ne.y;
    }
    if (parent->parent)
    {
        MTCMessage msg = {MTCM_CHANGE, 0, parent};
        for(y = miny; y <= maxy; y++)
        {
            for(x = minx; x <= maxx; x++)
            {
                in1 = ((x >= s.x) && (x <= e.x) && (y >= s.y) && (y <= e.y));
                in2 = ((x >= ns.x) && (x <= ne.x) && (y >= ns.y) && (y <= ne.y));
                if (in1 != in2)
                {
                    getcellsrect(x - otrack, y - oline, 1, 1, msg.dr);
                    parent->parent->message(msg);
                };
            };
        };
    };
}

int MTPattManager::clienttodata(MTPoint& pos)
{
    int x;
    int c = 0;

    if (patt)
    {
        pos.x = pos.x - pattx;
        if (pos.x < 0)
        {
            pos.x = -1;
        }
        else
        {
            x = pos.x;
            pos.x = otrack;
            for(pos.x = otrack; pos.x < patt->ntracks; pos.x++)
            {
                if (x < cellwidth[pos.x])
                {
                    TrackInfo& ti = patt->tracks[pos.x];
                    for(c = 0; (c < ti.ncolumns) && (x >= colwidth[pos.x][c]); c++)
                    {
                        x -= colwidth[pos.x][c];
                    };
                    if (c == ti.ncolumns)
                    {
                        c = -1;
                    }
                    break;
                };
                x -= cellwidth[pos.x];
            };
            if (pos.x >= patt->ntracks)
            {
                pos.x = -1;
            }
        };
        pos.y = pos.y - patty;
        if (pos.y < 0)
        {
            pos.y = -1;
        }
        else
        {
            pos.y = (pos.y / cellheight) + oline;
            if (pos.y >= patt->nlines)
            {
                pos.y = -1;
            }
        };
        return c;
    }
    else
    {
        pos.x = -1;
        pos.y = -1;
        return -1;
    };
}

void MTPattManager::datatoclient(MTPoint& pos)
{
    int x, t;

    if (patt)
    {
        x = pos.x;
        pos.x = pattx;
        for(t = otrack; t < otrack + x; t++)
        {
            pos.x += cellwidth[t];
        }
        pos.y = (pos.y - oline) * cellheight + patty;
    }
    else
    {
        pos.x = -1;
        pos.y = -1;
    };
}

void MTPattManager::stepit()
{
    setcursor(cursor.y + 1, cursor.x, cpos);
}

void MTPattManager::updatemetrics(bool passive)
{
    int fw = skin->fontwidth;
    int ah, bh;
    int t, x, w;

    skin->getcontrolsize(MTC_SCROLLER, 0, x, ah);
    skin->getcontrolsize(MTC_STATUS, 0, x, bh);
    pattx = (parent) ? fw * 3 + ah : 0;
    if ((parent) && (parent->open(0)))
    {
        parent->setfont(skin->getfont(1));
        patty = bh + parent->gettextheight() + 8;
        parent->close(0);
    }
    else
    {
        patty = 0;
    };
    cellheight = skin->fontheight;
    if (patt)
    {
        for(t = 0; t < patt->ntracks; t++)
        {
            cellwidth[t] = 4;
            for(x = 0; x < patt->tracks[t].ncolumns; x++)
            {
                cellwidth[t] += colwidth[t][x] + fw / 2;
            };
        };
    }
    else
    {
        mtmemzero(cellwidth, sizeof(cellwidth));
    };
    ntracks = 0;
    w = width - pattx;
    cw = 0;
    if (patt)
    {
        for(x = otrack; x < patt->ntracks; x++)
        {
            if (w < cellwidth[x])
            {
                break;
            }
            ntracks++;
            w -= cellwidth[x];
            cw += cellwidth[x];
        };
    };
    if (parent)
    {
        nlines = (height - ah - patty) / cellheight;
    }
    else
    {
        nlines = (height - patty) / cellheight;
    };
    updatescroller(passive);
}

void MTPattManager::updatescroller(bool passive)
{
    int nu = (nlines - 1) >> 1;

    if ((!hs) || (!vs))
    {
        return;
    }
    hs->os = (ntracks > 0) ? cw / ntracks : 0;
    vs->os = cellheight;
    hs->page = ntracks;
    vs->page = nlines;
    if (patt)
    {
        hs->maxpos = patt->ntracks;
        vs->maxpos = patt->nlines + nlines - 1;
    }
    else
    {
        hs->maxpos = 0;
        vs->maxpos = 0;
    };
    if (passive)
    {
        hs->pos = otrack;
        vs->pos = oline + nu;
    }
    else
    {
        hs->setposition(otrack);
        vs->setposition(oline + nu);
    };
}

void MTPattManager::drawpos(int l, int h)
{
    int y, z, nx;
    bool zeroes = objectsprefs.showzeroes;
    bool hex = objectsprefs.hexadecimal;
    int bx = 0;
    int by, ah;
    int state = 0;
    MTBitmap* b;

    if (!patt)
    {
        return;
    }
    if (l < 0)
    {
        if (l + h > 0)
        {
            h += l;
            l = 0;
        }
        else
        {
            return;
        }
    };
    if (l + h > nlines)
    {
        if (l >= nlines)
        {
            return;
        }
        else
        {
            h = nlines - l;
        }
    };

    by = patty + l * cellheight;
    parent->preparedraw(&b, bx, by);
    skin->getcontrolsize(MTC_SCROLLER, 0, z, ah);
    b->fill(bx, by, pattx - ah, h * cellheight, skin->getcolor(SC_PATT_BACK1));
    if (nlines > patt->nlines - oline)
    {
        y = patt->nlines - oline;
        if (l + h > y)
        {
            if (l >= y)
            {
                return;
            }
            else
            {
                h = y - l;
            }
        };
    };
    z = l + oline;
    if (patt->data)
    {
        for(y = 0; y < h; y++)
        {
            if ((l + y >= nlines) || (z >= patt->nlines))
            {
                break;
            }
            if (z >= 0)
            {
                b->fill(bx, by, pattx - ah, cellheight, colors[0][z % patt->lpb]);
                if (z % patt->lpb == 0)
                {
                    state = 2;
                }
                else
                {
                    state = 0;
                };
                nx = bx;
                if (hex)
                {
                    skin->drawhex(z, zeroes, 3, b, nx, by, skin->getcolor(SC_PATT_TEXT1 + state));
                }
                else
                {
                    skin->drawdec(z, zeroes, 3, b, nx, by, skin->getcolor(SC_PATT_TEXT1 + state));
                }
            };
            z++;
            by += cellheight;
        };
    };
}

void MTPattManager::drawtrack(int t, int w)
{
    int x, y, ol, l, r;
    int bw, bh;
    MTRect cr;
    char* end;
    char nullname = 0;
    int bx, by, w2;
    MTBitmap* b;
    MTPoint pt[3];
    char tracks[4];

    if (t < 0)
    {
        if (t + w > 0)
        {
            w += t;
            t = 0;
        }
        else
        {
            return;
        }
    };
    if (t + w > ntracks)
    {
        if (t >= ntracks)
        {
            return;
        }
        else
        {
            w = ntracks - t;
        }
    };
    bx = pattx;
    w2 = 0;
    for(x = otrack; x < otrack + t; x++)
    {
        bx += cellwidth[x];
    };
    for(; x < otrack + t + w; x++)
    {
        w2 += cellwidth[x];
    };
    by = 0;
    parent->preparedraw(&b, bx, by);
    b->fill(bx, by, w2, patty, skin->getcolor(SC_BACKGROUND));
    if (patt)
    {
        if (t < -otrack)
        {
            if (t + w > -otrack)
            {
                w += t + otrack;
                t = -otrack;
            }
            else
            {
                return;
            }
        };
        x = patt->ntracks - otrack;
        if (t + w > x)
        {
            if (t >= x)
            {
                return;
            }
            else
            {
                w = x - t;
            }
        };
    };
    skin->getcontrolsize(MTC_STATUS, 0, bw, bh);
    y = by;
    b->open(0);
    b->setpen(skin->getcolor(SC_CTRL_S));
    y += patty - 2;
    b->moveto(bx, y);
    b->lineto(bx + w2, y);
    b->setpen(skin->getcolor(SC_CTRL_DS));
    y++;
    b->moveto(bx, y);
    b->lineto(bx + w2, y);
    b->setfont(skin->getfont(1));
    tw = (b->getcharwidth('0') + 2) * 2 + 4;
    b->close(0);
    if (patt)
    {
        ol = bx;
        for(x = 0, y = otrack + t; x < w; x++, y++)
        {
            l = ol;
            ol += cellwidth[y];
            r = ol;
            end = tracks;
            if (y < 9)
            {
                *(end++) = y + '1';
            }
            else
            {
                *(end++) = ((y + 1) / 10) + '0';
                *(end++) = ((y + 1) % 10) + '0';
            };
            *end = 0;
            parent->open(0);
            cr.top = by + 2;
            cr.bottom = cr.top + bh;
            cr.left = l + 2;
            cr.right = l + tw;
            b->settextcolor(skin->getcolor(SC_TEXT_NORMAL));
            b->drawtext(tracks, -1, cr, DTXT_CENTER | DTXT_VCENTER);
            end = &nullname;
            if (patt->parent)
            {
                MTModule& cmodule = *(MTModule*) patt->parent;
                if (A(cmodule.trk, Track)[patt->tracks[y].id])
                {
                    end = A(cmodule.trk, Track)[patt->tracks[y].id]->name;
                };
            };
            cr.left = l + 16;
            cr.right = r - 16;
            cr.top = by + bh + 4;
            cr.bottom = by + patty - 4;
            b->drawtext(end, -1, cr, DTXT_VCENTER);
            pt[1].x = l + 10;
            pt[1].y = (cr.top + cr.bottom) / 2;
            pt[0].x = l + 6;
            pt[0].y = pt[1].y - 4;
            pt[2].x = l + 6;
            pt[2].y = pt[1].y + 4;
            b->setbrush(skin->getcolor(SC_TEXT_NORMAL));
            b->setpen(skin->getcolor(SC_TEXT_NORMAL));
            b->polygon(pt, 3);
            b->setpen(skin->getcolor(SC_CTRL_L));
            b->moveto(l, by);
            b->lineto(l, by + patty - 1);
            b->setpen(skin->getcolor(SC_CTRL_S));
            b->moveto(r - 1, by);
            b->lineto(r - 1, by + patty - 2);
            b->close(0);
            l += tw;
            skin->drawcontrol(MTC_STATUS, 0, NORECT, b, l, by + 2, (patt->tracks[y].on) ? 1 : 0);
            l += bh + 2;
            skin->drawcontrol(MTC_STATUS, 1, NORECT, b, l, by + 2, (patt->tracks[y].solo) ? 1 : 0);
            l += bh + 2;
            skin->drawcontrol(MTC_STATUS, 2, NORECT, b, l, by + 2, (patt->tracks[y].rec) ? 1 : 0);
            l += bh + 2;
            if (r >= l + bw + 4)
            {
                skin->drawcontrol(MTC_STATUS, 3, NORECT, b, r - bw - 4, by + 2, 0);
            };
        };
    };
}

void MTPattManager::drawcells(int t, int l, int w, int h)
{
    int x, y, n, ct, w2, coffset;
    int px, py;
    int bx, by;
    int fw = skin->fontwidth / 2;
    MTBitmap* b;
    MTRect r, r2;
    ColumnDrawState cstate;

    if (t < 0)
    {
        if (t + w > 0)
        {
            w += t;
            t = 0;
        }
        else
        {
            return;
        }
    };
    if (l < 0)
    {
        if (l + h > 0)
        {
            h += l;
            l = 0;
        }
        else
        {
            return;
        }
    };
    if (t + w > ntracks)
    {
        if (t >= ntracks)
        {
            return;
        }
        else
        {
            w = ntracks - t;
        }
    };
    if (l + h > nlines)
    {
        if (l >= nlines)
        {
            return;
        }
        else
        {
            h = nlines - l;
        }
    };
    bx = pattx;
    w2 = 0;
    for(x = otrack; x < otrack + t; x++)
    {
        bx += cellwidth[x];
    };
    for(; x < otrack + t + w; x++)
    {
        w2 += cellwidth[x];
    };
    parent->fillcolor(bx, patty + l * cellheight, w2, h * cellheight, skin->getcolor(SC_PATT_BACK1));
    if (patt)
    {
        if (t < -otrack)
        {
            if (t + w > -otrack)
            {
                w += t + otrack;
                t = -otrack;
            }
            else
            {
                return;
            }
        };
        if (l < -oline)
        {
            if (l + h > -oline)
            {
                h += l + oline;
                l = -oline;
            }
            else
            {
                return;
            }
        };
        x = patt->ntracks - otrack;
        y = patt->nlines - oline;
        if (t + w > x)
        {
            if (t >= x)
            {
                return;
            }
            else
            {
                w = x - t;
            }
        };
        if (l + h > y)
        {
            if (l >= y)
            {
                return;
            }
            else
            {
                h = y - l;
            }
        };
    };
    by = patty + l * cellheight;
    parent->preparedraw(&b, bx, by);
    px = bx;
    py = by;
    if ((patt) && (patt->data))
    {
        r.top = py;
        r.bottom = py + skin->fontheight;
        cstate.flags = CDS_SELECTED;
        cstate.lpb = patt->lpb;
        cstate.line = l + oline;
        coffset = 0;
        for(x = 0; x < t + otrack; x++)
        {
            TrackInfo& ti = patt->tracks[x];
            for(n = 0; n < ti.ncolumns; n++)
            {
                coffset += colsize[x][n];
            }
        };
        getsel(r2);
        r2.left -= otrack + t;
        r2.top -= oline + l;
        r2.right -= otrack + t;
        r2.bottom -= oline + l;
        for(y = 0; y < h; y++)
        {
            r.left = r.right = px + 2;
            if ((l + y >= nlines) || (l + y + oline >= patt->nlines))
            {
                break;
            }
            unsigned char* cdata = patt->data + (y + l + oline) * patt->linesize + coffset;
            for(x = 0, ct = t + otrack; x < w; x++, ct++)
            {
                TrackInfo& ti = patt->tracks[ct];
                if ((t + x >= ntracks) || (t + x + otrack >= patt->ntracks))
                {
                    break;
                }
                cstate.flags &= (~CDS_SELECTED);
                if ((x >= r2.left) && (x <= r2.right) && (y >= r2.top) && (y <= r2.bottom))
                {
                    cstate.flags |= CDS_SELECTED;
                    b->fill(
                        r.right - 2,
                        r.top,
                        cellwidth[t + x + otrack],
                        r.bottom - r.top,
                        colors[1][(l + y + oline) % patt->lpb]
                    );
                }
                else
                {
                    b->fill(
                        r.right - 2,
                        r.top,
                        cellwidth[t + x + otrack],
                        r.bottom - r.top,
                        colors[0][(l + y + oline) % patt->lpb]
                    );
                };
                for(n = 0; n < ti.ncolumns; n++)
                {
                    r.left = r.right;
                    r.right += colwidth[t + x + otrack][n];
                    if ((x + t + otrack == cursor.x) && (y + l + oline == cursor.y) && (ccol == n))
                    {
                        cstate.cursor = cpos;
                    }
                    else
                    {
                        cstate.cursor = -1;
                    }
                    ti.cols[n].handler->drawcolumn(b, r, cdata, cstate);
                    cdata += colsize[ct][n];
                    r.right += fw;
                };
                r.left = r.right;
                r.right += 4;
            };
            r.top = r.bottom;
            r.bottom += skin->fontheight;
            cstate.line++;
        };
        r2.left = t;
        r2.top = l;
        r2.right = t + w - 1;
        r2.bottom = l + h - 1;
        if (pointinrect(hl, r2))
        {
            px = 0;
            for(x = otrack; x < otrack + hl.x; x++)
            {
                px += cellwidth[x];
            };
            parent->open(0);
            parent->setpen(skin->getcolor(SC_CURSOR));
            parent->setbrush(-1);
            parent->rectangle(pattx + px, patty + hl.y * cellheight, cellwidth[x], cellheight);
            parent->close(0);
        };
    };
}

void MTPattManager::getposrect(int l, int h, MTRect& r)
{
    r.left = 0;
    r.top = patty + l * cellheight;
    r.right = pattx - vs->width;
    r.bottom = r.top + h * cellheight;
}

void MTPattManager::gettrackrect(int t, int w, MTRect& r)
{
    int x;

    r.left = pattx;
    for(x = otrack; x < otrack + t; x++)
    {
        r.left += cellwidth[x];
    }
    r.right = r.left;
    for(x = otrack + t; x < otrack + t + w; x++)
    {
        r.right += cellwidth[x];
    }
    r.top = 0;
    r.bottom = patty;
}

void MTPattManager::getcellsrect(int t, int l, int w, int h, MTRect& r)
{
    int x;

    r.left = pattx;
    for(x = otrack; x < otrack + t; x++)
    {
        r.left += cellwidth[x];
    }
    r.right = r.left;
    for(; x < otrack + t + w; x++)
    {
        r.right += cellwidth[x];
    }
    r.top = patty + l * cellheight;
    r.bottom = r.top + h * cellheight;
}

void MTPattManager::checkcolors()
{
    int x;

    if ((!patt) || (patt->lpb == clpb))
    {
        return;
    }
    clpb = patt->lpb;
    colors[0][0] = skin->getcolor(SC_PATT_BACK3);
    colors[0][1] = skin->getcolor(SC_PATT_BACK1);
    colors[1][0] = skin->getcolor(SC_PATT_BACK4);
    colors[1][1] = skin->getcolor(SC_PATT_BACK2);
    if (clpb < 4)
    {
        for(x = 2; x < clpb; x++)
        {
            colors[0][x] = colors[0][1];
            colors[1][x] = colors[1][1];
        };
        return;
    };
    switch (clpb)
    {
        case 4:
            colors[0][2] = calccolor(colors[0][1], colors[0][0], 0.5);
            colors[1][2] = calccolor(colors[1][1], colors[1][0], 0.5);
            colors[0][3] = colors[0][1];
            colors[1][3] = colors[1][1];
            break;
        case 6:
            colors[0][2] = calccolor(colors[0][1], colors[0][0], 0.5);
            colors[1][2] = calccolor(colors[1][1], colors[1][0], 0.5);
            colors[0][3] = colors[0][1];
            colors[1][3] = colors[1][1];
            colors[0][4] = colors[0][2];
            colors[1][4] = colors[1][2];
            colors[0][5] = colors[0][1];
            colors[1][5] = colors[1][1];
            break;
        case 8:
            colors[0][2] = calccolor(colors[0][1], colors[0][0], 0.3);
            colors[1][2] = calccolor(colors[1][1], colors[1][0], 0.3);
            colors[0][3] = colors[0][1];
            colors[1][3] = colors[1][1];
            colors[0][4] = calccolor(colors[0][1], colors[0][0], 0.5);
            colors[1][4] = calccolor(colors[1][1], colors[1][0], 0.5);
            colors[0][5] = colors[0][1];
            colors[1][5] = colors[1][1];
            colors[0][6] = colors[0][2];
            colors[1][6] = colors[1][2];
            colors[0][7] = colors[0][1];
            colors[1][7] = colors[1][1];
            break;
        case 12:
            colors[0][2] = calccolor(colors[0][1], colors[0][0], 0.4);
            colors[1][2] = calccolor(colors[1][1], colors[1][0], 0.4);
            colors[0][3] = colors[0][1];
            colors[1][3] = colors[1][1];
            colors[0][4] = colors[0][2];
            colors[1][4] = colors[1][2];
            colors[0][5] = colors[0][1];
            colors[1][5] = colors[1][1];
            colors[0][6] = calccolor(colors[0][1], colors[0][0], 0.6);
            colors[1][6] = calccolor(colors[1][1], colors[1][0], 0.6);
            colors[0][7] = colors[0][1];
            colors[1][7] = colors[1][1];
            colors[0][8] = colors[0][2];
            colors[1][8] = colors[1][2];
            colors[0][9] = colors[0][1];
            colors[1][9] = colors[1][1];
            colors[0][10] = colors[0][2];
            colors[1][10] = colors[1][2];
            colors[0][11] = colors[0][1];
            colors[1][11] = colors[1][1];
            break;
        default:
            for(x = 2; x < clpb; x += 2)
            {
                colors[0][x] = calccolor(colors[0][1], colors[0][0], (float) x / clpb);
                colors[1][x] = calccolor(colors[1][1], colors[1][0], (float) x / clpb);
                colors[0][x + 1] = calccolor(colors[0][1], colors[0][0], (float) (x - 1) / clpb);
                colors[1][x + 1] = calccolor(colors[1][1], colors[1][0], (float) (x - 1) / clpb);
            };
            break;
    };
}
//---------------------------------------------------------------------------
