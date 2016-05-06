//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTOscillator.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include <math.h>
#include "MTOscillator.h"
#include <MTXAPI/RES/MTObjectsRES.h>

//---------------------------------------------------------------------------
SampleType *sampletype;

//---------------------------------------------------------------------------
SampleType::SampleType()
{
    type = MTO_MTSAMPLE;
    description = "Sample";
}

MTObject *SampleType::create(MTObject *parent, mt_int32 id, void *param)
{
    return new MTSample(parent, id);
}

//---------------------------------------------------------------------------
// MTSample functions
//---------------------------------------------------------------------------
MTSample::MTSample(MTObject *parent, mt_int32 i):
    Oscillator(parent, MTO_MTSAMPLE, i), filename(0), time(0), length(0), frequency(0), depth(0), nchannels(0), loop(0),
    loops(0), loope(0), note(60), bpm(125.0), ns(0), sl(0), file(0), fileoffset(0), peaks(0)
{
    mtmemzero(data, sizeof(data));
#	ifdef MTSYSTEM_RESOURCES
    res->loadstringf(MTT_oscillator, name, 255, id);
#	endif
}

MTSample::~MTSample()
{
    if (filename)
    { si->memfree(filename); }
    splfree();
}

int MTSample::loadfromstream(MTFile *f, int size, void *params)
{
    void *buffer;

    if ((nchannels > 2) || (nchannels == 0) || (depth == 0))
    { return 0; }
    if (!data[0])
    {
        if (!splalloc(size / (depth * nchannels)))
        { return 0; }
    };
    buffer = f->getpointer(-1, size);
    if (nchannels == 1)
    {
        memcpy(data[0], buffer, ns * depth);
        if (depth == 1)
        {
            a_delta_decode_8((char *) data[0], ns);
        }
        else
        { a_delta_decode_16((short *) data[0], ns); }
    }
    else
    {
        memcpy(data[0], buffer, ns * depth);
        memcpy(data[1], (char *) buffer + ns * depth, ns * depth);
        if (depth == 1)
        {
            a_delta_add_8((char *) data[1], (char *) data[0], ns);
            a_delta_decode_8((char *) data[0], ns);
            a_delta_decode_8((char *) data[1], ns);
        }
        else
        {
            a_delta_add_16((short *) data[1], (short *) data[0], ns);
            a_delta_decode_16((short *) data[0], ns);
            a_delta_decode_16((short *) data[1], ns);
        };
    };
    f->releasepointer(buffer);
    return size;
}

int MTSample::savetostream(MTFile *f, void *params)
{
    return 0;
}

OscillatorInstance *MTSample::createinstance(int noutputs, sample **outputs, InstrumentInstance *caller)
{
    if ((nchannels == 0) || (data[0] == 0))
    { return 0; }
    return new MTSampleInstance(this, noutputs, outputs, caller);
}

bool MTSample::changesign()
{
    int x;

    if (depth != 1)
    { return false; }
    for(x = 0; x < nchannels; x++) a_changesign((char *) data[x], ns);
    return true;
}

bool MTSample::splalloc(int nsamples)
{
    int x;

    ns = nsamples;
    for(x = 0; x < nchannels; x++)
    {
        data[x] = si->memalloc((ns + SAFE_BUFFER * 2) * depth, MTM_ZERO);
        if (!data[x])
        {
            while(--x >= 0) si->memfree((char *) data[x] - SAFE_BUFFER * depth);
            return false;
        };
        data[x] = (char *) data[x] + SAFE_BUFFER * depth;
    };
    return true;
}

bool MTSample::splrealloc(int nsamples)
{
    int x;

    ns = nsamples;
    for(x = 0; x < nchannels; x++)
    {
        data[x] = si->memrealloc((char *) data[x] - SAFE_BUFFER * depth, (ns + SAFE_BUFFER * 2) * depth);
        if (!data[x])
        { return false; }
        data[x] = (char *) data[x] + SAFE_BUFFER * depth;
    };
    return true;
}

bool MTSample::splfree()
{
    int x;

    for(x = 0; x < nchannels; x++) si->memfree((char *) data[x] - SAFE_BUFFER * depth);
    return true;
}

bool MTSample::buildpeaks(void *process)
{
    return true;
}

//---------------------------------------------------------------------------
MTSampleInstance::MTSampleInstance(Oscillator *p, int no, sample **o, InstrumentInstance *caller):
    OscillatorInstance(p, no, o, caller), flags(0), volvarlng(0), panvarlng(0)
{
    int x;
    MTSample &cparent = *(MTSample *) parent;

    mtmemzero(status, sizeof(status));
    volvar = panxvar = panyvar = panzvar = 0.0;
    if (cparent.nchannels > noutputs)
    {
        nstatus = cparent.nchannels;
        divider = cparent.nchannels / no;
    }
    else
    {
        nstatus = noutputs;
        divider = 1;
    };
    if (cparent.depth == 1)
    {
        divider *= 128;
    }
    else if (cparent.depth == 2)
    { divider *= 32768; }
    for(x = 0; x < nstatus; x++)
    {
        status[x].sc = x % cparent.nchannels;
        status[x].tc = x % noutputs;
        status[x].multiplier = volume / divider;
        status[x].vol = volume * status[x].multiplier;
        status[x].pitch = pitch;
    };
//	a_floattofixed(pitch,pitchi,pitchd);
}

bool MTSampleInstance::seek(double offset, int origin, int units)
{
    int x;
    int offseti;
    unsigned int offsetd;
    bool reverse;

    if ((units & 0xF) == MTIS_BEATS)
    {
        offset *= module->playstatus.spb;
    }
    else if ((units & 0xF) == MTIS_MS)
    { offset *= (double) module->playstatus.coutput->frequency / 1000; }
    if ((units & MTIS_INNER) == 0)
    { offset *= pitch; }
    reverse = a_floattofixed(offset, offseti, offsetd);
    switch (origin)
    {
        case MTIS_BEGIN:
            if (reverse)
            { return false; }
            for(x = 0; x < nstatus; x++)
            {
                status[x].posi = offseti;
                status[x].posd = offsetd;
            };
            break;
        case MTIS_CURRENT:
            for(x = 0; x < nstatus; x++)
            {
                a_calcposition(status[x].posi, status[x].posd, offseti, offsetd, 1, reverse);
            };
            break;
        case MTIS_END:
            if ((!reverse) && ((offseti | offsetd) != 0))
            { return false; }
            for(x = 0; x < nstatus; x++)
            {
                status[x].posi = ((MTSample *) parent)->ns;
                status[x].posd = 0;
                a_calcposition(status[x].posi, status[x].posd, offseti, offsetd, 1, reverse);
            };
            break;
    };
    return true;
}

bool MTSampleInstance::process(int offset, int count, bool &silence)
{
    MTSample &cparent = *(MTSample *) parent;
    int x, e, ls, le;
    int ccount, coffset, cmax;
    int flags;
    int pitchi, dposi;
    unsigned int pitchd, dposd;

    silence = true;
    if (!dspi)
    { return false; }
//	m = cparent.depth-1;
    flags = cparent.depth - 1;
    if (cparent.loop)
    {
        ls = cparent.loops;
        le = cparent.loope;
        if ((ls >= le) || (ls < 0) || (le > cparent.ns))
        {
            ls = 0;
            le = cparent.ns;
        };
    }
    else
    {
        ls = 0;
        le = cparent.ns;
    };
    if (volvarlng)
    {
        x = volvarlng;
        if (x > count)
        { x = count; }
        volume += volvar * x;
        volvarlng -= x;
    };
    if (panvarlng)
    {
        x = panvarlng;
        if (x > count)
        { x = count; }
        panx += panxvar * x;
        pany += panyvar * x;
        panz += panzvar * x;
        panvarlng -= x;
    };
//	if (ai->recording) ai->debugpoint(module->beatstosamples(module->playstatus.pos)-1,"B");
    e = 0;
    for(x = 0; x < nstatus; x++)
    {
        ChannelStatus &cstatus = status[x];
        coffset = offset;
        cmax = count;
        while(cmax > 0)
        {
            if (cparent.loop == 1)
            {
                if (cstatus.reverse)
                {
                    if (cstatus.posi <= ls)
                    {
                        cstatus.posi = le - (ls - cstatus.posi) % (le - ls);
                    };
                }
                else
                {
                    if (cstatus.posi >= le)
                    {
//						if (ai->recording) ai->debugpoint(module->beatstosamples(module->playstatus.pos)+count-cmax,"LE");
                        cstatus.posi = (cstatus.posi - ls) % (le - ls) + ls;
                    };
                };
            }
            else if (cparent.loop == 2)
            {
                if (cstatus.reverse)
                {
                    if (cstatus.posi <= ls)
                    {
                        cstatus.reverse = false;
                        cstatus.posi = ls * 2 - cstatus.posi;
                        cstatus.posd = ~cstatus.posd;
                    };
                }
                else
                {
                    if (cstatus.posi >= le)
                    {
                        cstatus.reverse = true;
                        cstatus.posi = le * 2 - cstatus.posi;
                        cstatus.posd = ~cstatus.posd;
                    };
                };
            };
            cstatus.pitch = pitch;
            dposi = cstatus.posi;
            dposd = cstatus.posd;
            a_floattofixed(cstatus.pitch, pitchi, pitchd);
            a_calcposition(dposi, dposd, pitchi, pitchd, cmax, cstatus.reverse);
            if (cstatus.reverse)
            {
                if (dposi <= ls)
                {
//				if (cstatus.posi-(cmax+1)*pitch<ls){
                    ccount = (int) ceil((cstatus.posi - ls) / pitch);
                    dposi = cstatus.posi;
                    dposd = cstatus.posd;
                    a_calcposition(dposi, dposd, pitchi, pitchd, ccount, cstatus.reverse);
                    e++;
                }
                else
                {
                    ccount = cmax;
                };
            }
            else
            {
                if (dposi >= le)
                {
//				if (cstatus.posi+(cmax+1)*pitch>le){
                    ccount = (int) ceil((le - cstatus.posi) / pitch);
                    dposi = cstatus.posi;
                    dposd = cstatus.posd;
                    a_calcposition(dposi, dposd, pitchi, pitchd, ccount, cstatus.reverse);
                    e++;
                }
                else
                {
                    ccount = cmax;
                };
            };
            if (ccount <= 0)
            { break; }
            if ((fabs(cstatus.vol) < VOLUME_THRESOLD) && ((cstatus.volvarlng == 0) || (cstatus.volvar == 0.0)))
            {
                cstatus.posi = dposi;
                cstatus.posd = dposd;
            }
            else
            {
                if ((cstatus.volvarlng != 0) && (cstatus.volvarlng < ccount))
                {
                    dspi->resample[flags](outputs[cstatus.tc] + coffset, (char *) (cparent.data[cstatus.sc]) + (cstatus.posi << flags), cstatus.volvarlng, cstatus);

                    cstatus.vol += cstatus.volvar * cstatus.volvarlng;
                    coffset += cstatus.volvarlng;
                    cmax -= cstatus.volvarlng;
                    ccount -= cstatus.volvarlng;

                    a_floattofixed(cstatus.pitch, pitchi, pitchd);
                    a_calcposition(cstatus.posi, cstatus.posd, pitchi, pitchd, cstatus.volvarlng, cstatus.reverse);

                    cstatus.volvarlng = 0;
                };
                dspi->resample[flags](outputs[cstatus.tc] + coffset, (char *) (cparent.data[cstatus.sc]) + (cstatus.posi << flags), ccount, cstatus);
                if (cstatus.volvarlng >= ccount)
                {
                    cstatus.vol += cstatus.volvar * ccount;
                    cstatus.volvarlng -= ccount;
                };
                cstatus.posi = dposi;
                cstatus.posd = dposd;
//				dspi->resample[flags](outputs[cstatus.tc]+coffset,(sample*)cparent.data[cstatus.sc]+cstatus.posi,ccount,cstatus);
                silence = false;
            };
            if (!cparent.loop)
            { break; }
            coffset += ccount;
            cmax -= ccount;
        };
    };
    if (cparent.loop)
    { return true; }
    return (e != nstatus);
}

void MTSampleInstance::setnote(double n)
{
    int x;
    MTSample &cparent = *(MTSample *) parent;
    bool reverse;

    note = n;
    if (cparent.flags & SF_SYNCHRONIZED)
    {
        pitch = ((double) cparent.frequency / (double) output->frequency) * (module->playstatus.bpm / cparent.bpm);
//		a_floattofixed(pitch,pitchi,pitchd);
        return;
    };
    reverse = (n < 0);
    if (n < 0)
    {
        reverse = true;
        n = -n;
    }
    else
    {
        reverse = false;
    };
    pitch = ((double) cparent.frequency / (double) output->frequency) * pow(1.0594630943592952645618252949463, n - ((MTSample *) parent)->note);
//	a_floattofixed(pitch,pitchi,pitchd);
    for(x = 0; x < nstatus; x++)
    {
        status[x].reverse ^= reverse;
    };
}

void MTSampleInstance::setvolume(double vs, int steps, int curve)
{
    int x;
    double dvol;
    float dpanx, dpany, dpanz;

    if (steps < 0)
    { return; }
    if (steps)
    {
        volvar = (vs - volume) / steps;
        if ((panvarlng > 1) && (panvarlng < steps))
        { steps = panvarlng; }
    }
    else
    {
        volvar = 0.0;
        volume = vs;
        if (!panvarlng)
        {
            for(x = 0; x < nstatus; x++)
            {
                status[x].vol = volume * status[x].multiplier;
            };
            volvarlng = 0;
            return;
        }
        else
        {
            steps = panvarlng;
        };
    };
    if (panvarlng)
    {
        if (steps)
        {
            if (panvarlng > steps)
            {
                panvarlng = steps;
            };
        }
        else
        {
            for(x = 0; x < nstatus; x++)
            {
                status[x].vol = volume * status[x].multiplier;
            };
        };
        dpanx = panx + panxvar * panvarlng;
        dpany = pany + panyvar * panvarlng;
        dpanz = panz + panzvar * panvarlng;
        switch (noutputs)
        {
            case 1:
                for(x = 0; x < nstatus; x++)
                {
                    status[x].multiplier = 1.0 / divider;
                };
                break;
            case 2:
                for(x = 0; x < nstatus; x++)
                {
                    if ((x & 1) == 0)
                    {    // Left
                        status[x].multiplier = (1.0 - dpanx) / divider;
                    }
                    else
                    {    // Right
                        status[x].multiplier = (1.0 + dpanx) / divider;
                    };
                };
                break;
            case 4:
                for(x = 0; x < nstatus; x++)
                {
                    if ((x & 1) == 0)
                    {    // Left
                        status[x].multiplier = (1.0 - dpanx) / divider;
                    }
                    else
                    {    // Right
                        status[x].multiplier = (1.0 + dpanx) / divider;
                    };
                    if ((x & 2) == 0)
                    {    // Front
                        status[x].multiplier *= (1.0 + dpany);
                    }
                    else
                    {    // Rear
                        status[x].multiplier *= (1.0 - dpany);
                    };
                };
                break;
            case 8:
                for(x = 0; x < nstatus; x++)
                {
                    if ((x & 1) == 0)
                    {    // Left
                        status[x].multiplier = (1.0 - dpanx) / divider;
                    }
                    else
                    {    // Right
                        status[x].multiplier = (1.0 + dpanx) / divider;
                    };
                    if ((x & 2) == 0)
                    {    // Front
                        status[x].multiplier *= (1.0 + dpany);
                    }
                    else
                    {    // Rear
                        status[x].multiplier *= (1.0 - dpany);
                    };
                    if ((x & 4) == 0)
                    {    // Top
                        status[x].multiplier *= (1.0 + dpanz);
                    }
                    else
                    {    // Bottom
                        status[x].multiplier *= (1.0 - dpanz);
                    };
                };
                break;
        };
    };
    volvarlng = steps;
    dvol = volume + volvar * volvarlng;
    for(x = 0; x < nstatus; x++)
    {
        ChannelStatus &cstatus = status[x];
        cstatus.volvar = (dvol * cstatus.multiplier - cstatus.vol) / steps;
//		cstatus.volvar = volvar*cstatus.multiplier;
        cstatus.volvarlng = steps;
    };
}

void MTSampleInstance::setpanning(float xs, float ys, float zs, int steps, int curve)
{
    int x;
    double dvol;
    float dpanx, dpany, dpanz;
    bool surround = false;

    if (steps < 0)
    { return; }
    MTSample &cparent = *(MTSample *) parent;
    if (xs < -1.0)
    {
        xs = 0.0;
        surround = true;
    };
    if (ys < -1.0)
    {
        ys = 0.0;
        surround = true;
    };
    if (zs < -1.0)
    {
        zs = 0.0;
        surround = true;
    };
    if (steps)
    {
        panxvar = (xs - panx) / steps;
        panyvar = (ys - pany) / steps;
        panzvar = (zs - panz) / steps;
        if ((volvarlng > 1) && (volvarlng < steps))
        {
            steps = volvarlng;
            dpanx = panx + panxvar * steps;
            dpany = pany + panyvar * steps;
            dpanz = panz + panzvar * steps;
        }
        else
        {
            dpanx = xs;
            dpany = ys;
            dpanz = zs;
        };
    }
    else
    {
        panxvar = panyvar = panzvar = 0.0;
        panx = dpanx = xs;
        pany = dpany = ys;
        panz = dpanz = zs;
    };
    switch (noutputs)
    {
        case 1:
            for(x = 0; x < nstatus; x++)
            {
                status[x].multiplier = 1.0 / divider;
            };
            break;
        case 2:
            for(x = 0; x < nstatus; x++)
            {
                if ((x & 1) == 0)
                {    // Left
                    status[x].multiplier = (1.0 - dpanx) / divider;
                }
                else
                {    // Right
                    status[x].multiplier = (1.0 + dpanx) / divider;
                };
            };
            break;
        case 4:
            for(x = 0; x < nstatus; x++)
            {
                if ((x & 1) == 0)
                {    // Left
                    status[x].multiplier = (1.0 - dpanx) / divider;
                }
                else
                {    // Right
                    status[x].multiplier = (1.0 + dpanx) / divider;
                };
                if ((x & 2) == 0)
                {    // Front
                    status[x].multiplier *= (1.0 + dpany);
                }
                else
                {    // Rear
                    status[x].multiplier *= (1.0 - dpany);
                };
            };
            break;
        case 8:
            for(x = 0; x < nstatus; x++)
            {
                if ((x & 1) == 0)
                {    // Left
                    status[x].multiplier = (1.0 - dpanx) / divider;
                }
                else
                {    // Right
                    status[x].multiplier = (1.0 + dpanx) / divider;
                };
                if ((x & 2) == 0)
                {    // Front
                    status[x].multiplier *= (1.0 + dpany);
                }
                else
                {    // Rear
                    status[x].multiplier *= (1.0 - dpany);
                };
                if ((x & 4) == 0)
                {    // Top
                    status[x].multiplier *= (1.0 + dpanz);
                }
                else
                {    // Bottom
                    status[x].multiplier *= (1.0 - dpanz);
                };
            };
            break;
    };
    if (surround)
    {
        for(x = 1; x < nstatus; x += 2)
        {
            status[x].multiplier = -status[x].multiplier;
        };
    };
    if ((!steps) && (!volvarlng))
    {
        for(x = 0; x < nstatus; x++)
        {
            status[x].vol = volume * status[x].multiplier;
        };
        panvarlng = 0;
        return;
    };
    dvol = volume;
    if (volvarlng)
    {
        if (steps)
        {
            if (volvarlng > steps)
            { volvarlng = steps; }
        }
        else
        {
            steps = volvarlng;
            for(x = 0; x < nstatus; x++)
            {
                status[x].vol = volume * status[x].multiplier;
            };
        };
        dvol = volume + volvar * volvarlng;
    };
    panvarlng = steps;
    for(x = 0; x < nstatus; x++)
    {
        ChannelStatus &cstatus = status[x];
        cstatus.volvar = (dvol * cstatus.multiplier - cstatus.vol) / steps;
        cstatus.volvarlng = steps;
    };
}

double MTSampleInstance::getnote()
{
    return note;
}

double MTSampleInstance::getvolume(int steps)
{
    if (!steps)
    { return volume; }
    if (steps > volvarlng)
    { steps = volvarlng; }
    return volume + volvar * steps;
}

void MTSampleInstance::getpanning(float *x, float *y, float *z, int steps)
{
    if (!steps)
    {
        if (x)
        { *x = panx; }
        if (y)
        { *y = pany; }
        if (z)
        { *z = panz; }
        return;
    };
    if (steps > panvarlng)
    { steps = panvarlng; }
    if (x)
    { *x = panx + panxvar * steps; }
    if (y)
    { *y = pany + panyvar * steps; }
    if (z)
    { *z = panz + panzvar * steps; }
}

float MTSampleInstance::getimportance()
{
    return 0.5;
}
//---------------------------------------------------------------------------
