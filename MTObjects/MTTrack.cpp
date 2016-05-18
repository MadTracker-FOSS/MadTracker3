//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTTrack.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include "MTObject.h"
#include <MTXAPI/RES/MTObjectsRES.h>

//---------------------------------------------------------------------------
// Track functions
//---------------------------------------------------------------------------
Track::Track(MTObject* parent, int i, int sub):
    Node(parent, MTO_TRACK, i),
    muted(false),
    solo(false),
    empty(true),
    vol(1.0),
    panx(0.0),
    pany(0.0),
    panz(0.0),
    nsamples(0),
    offset(0)
{
    int x, n;

    ninputs = (i >= MAX_TRACKS) ? 2 : 0;
    noutputs = 2;
    mtmemzero(buffer, sizeof(buffer));
    if (parent)
    {
        if (id < MAX_TRACKS)
        {
#			ifdef MTSYSTEM_RESOURCES
            res->loadstringf(MTT_track, name, 255, id + 1);
#			endif
            Node* dest = A(module->master, Track)[0];
            n = dest->noutputs;
            for(x = 0; x < noutputs; x++)
            {
                outputs[x].s = x;
                outputs[x].d = x % n;
                outputs[x].m = 1.0;
                outputs[x].n = dest;
            };
        }
        else
        {
            vol = 0.5;
#			ifdef MTSYSTEM_RESOURCES
            res->loadstringf(MTT_master, name, 255, id - MAX_TRACKS + 1);
#			endif
            if (output->ndevices)
            {
                Node* dest = output->device[0]->master;
                n = dest->noutputs;
                for(x = 0; x < noutputs; x++)
                {
                    outputs[x].s = x;
                    outputs[x].d = x % n;
                    outputs[x].m = 1.0;
                    outputs[x].n = dest;
                };
            };
        };
    }
    else
    {
        strncpy(name, output->device[id]->name, 255);
        noutputs = output->device[id]->nchannels;
    };
    alloc();
}

Track::~Track()
{
    if (module)
    {
        if (id < MAX_TRACKS)
        {
            module->trk->a[id] = 0;
        }
        else
        {
            module->master->a[id - MAX_TRACKS] = 0;
        }
    };
    free();
}

int Track::process(int ooffset, int ioffset, int count, bool& silence)
{
    return 0;
}

void Track::alloc()
{
    int x;

    if (!output->ndevices)
    {
        return;
    }
    lock(MTOL_LOCK, true);
    if (buffer[0])
    {
        free();
    }
    nsamples = output->buffersamples * 2;
    if (module)
    {
        module->needupdaterouting();
        if (id < MAX_TRACKS)
        {
            nsamples = output->buffersamples * 2;
        }
        else
        {
            nsamples = output->device[0]->master->nsamples;
        }
    }
    else
    {
        nsamples = output->device[id]->datasamples;
    }
    for(x = 0; x < noutputs; x++)
    {
        buffer[x] = (sample*) si->memalloc(nsamples * sizeof(sample), MTM_ZERO);
    };
    lock(MTOL_LOCK, false);
}

void Track::free()
{
    int x;
    sample* old[8];

    lock(MTOL_LOCK, true);
    if (module)
    {
        module->needupdaterouting();
    };
    for(x = 0; x < noutputs; x++)
    {
        old[x] = buffer[x];
        buffer[x] = 0;
    };
    nsamples = 0;
    for(x = 0; x < noutputs; x++)
    {
        si->memfree(old[x]);
    }
    lock(MTOL_LOCK, false);
}
//---------------------------------------------------------------------------
