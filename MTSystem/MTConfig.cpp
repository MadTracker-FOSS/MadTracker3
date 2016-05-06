//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTConfig.cpp 76 2005-08-28 20:43:23Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MTConfig.h"

//---------------------------------------------------------------------------
struct NP
{
    int line;
    int nlines;
    char *buf;
};

//---------------------------------------------------------------------------
MTConfigFile *mtconfigfind(const char *filename)
{
    char path[512];

    if (mtfileexists((char *) filename))
    { return new MTConfigFile(filename); }
    MTPreferences *prefs;
    prefs = (MTPreferences *) mtinterface->getprefs();
    if (!prefs)
    { return 0; }
    strcpy(path, prefs->syspath[SP_CONFIG]);
    strcat(path, filename);
    return mtconfigopen(path);
}

MTConfigFile *mtconfigopen(const char *filename)
{
    MTConfigFile *cf = new MTConfigFile(filename);
    if (cf->loaded())
    { return cf; }
    delete cf;
    return 0;
}

void mtconfigclose(MTConfigFile *file)
{
    delete file;
}

//---------------------------------------------------------------------------
MTConfigFile::MTConfigFile(const char *filename):
    sectionpos(-1), sectionline(-1), sectionnp(0), cpos(0), cline(0), cnp(0), np(0)
{
    f = mtfileopen((char *) filename, MTF_READ | MTF_WRITE | MTF_SHAREREAD | MTF_CREATE);
}

MTConfigFile::~MTConfigFile()
{
    int x, l;
    void *buf;
    MTFile *tmpf;
    char line[256];

    if (!f)
    { return; }
    if (np)
    {
        tmpf = mttempfile(MTF_READ | MTF_WRITE);
        f->seek(0, MTF_BEGIN);
        cline = 0;
        for(x = 0; x < np->nitems; x++)
        {
            NP &mnp = ((NP *) np->d)[x];
            while(cline < mnp.line)
            {
                if (f->eof())
                { goto skip; }
                if (f->readln(line, 256))
                {
                    tmpf->write(line, strlen(line));
                };
                tmpf->write(NL, sizeof(NL) - 1);
                cline++;
            };
            skip:
            if (cline == mnp.line)
            {
                tmpf->write(mnp.buf, strlen(mnp.buf));
                tmpf->write(NL, sizeof(NL) - 1);
                while(cline < mnp.line + mnp.nlines)
                {
                    if (f->eof())
                    { goto skip2; }
                    f->readln(line, 256);
                    cline++;
                };
            };
            skip2:
            mtmemfree(mnp.buf);
        };
        l = f->length() - f->pos();
        buf = mtmemalloc(l);
        f->read(buf, l);
        tmpf->write(buf, l);
        mtmemfree(buf);
        tmpf->seek(0, MTF_BEGIN);
        f->seek(0, MTF_BEGIN);
        l = tmpf->length();
        buf = mtmemalloc(l);
        tmpf->read(buf, l);
        mtfileclose(tmpf);
        f->seteof();
        f->write(buf, l);
        mtmemfree(buf);
        delete np;
    };
    mtfileclose(f);
}

void MTConfigFile::clear()
{
    int x;

    if (!f)
    { return; }
    f->seek(0, MTF_BEGIN);
    f->seteof();
    sectionpos = sectionline = -1;
    cpos = cline = 0;
    sectionnp = cnp = 0;
    if (np)
    {
        for(x = 0; x < np->nitems; x++)
        {
            mtmemfree(((NP *) np->d)[x].buf);
        };
        delete np;
        np = 0;
    };
}

bool MTConfigFile::setsection(const char *name)
{
    int x;
    char *s, *e;
    char line[256];

    if (!f)
    { return false; }
    sectionpos = sectionline = -1;
    cpos = cline = cnp = 0;
    f->seek(0, MTF_BEGIN);
    while(!f->eof())
    {
        skip:
        if (np)
        {
            for(x = cnp; x < np->nitems; x++)
            {
                NP &mnp = ((NP *) np->d)[x];
                if (mnp.line == cline)
                {
                    x = mnp.nlines;
                    while(x-- > 0)
                    {
                        if (f->eof())
                        { goto exit; }
                        f->readln(line, 256);
                        cline++;
                    };
                    cnp++;
                    goto skip;
                };
            };
        };
        cline++;
        if (!f->readln(line, 256))
        { continue; }
        if ((line[0] == '#') || (line[0] == ';'))
        { continue; }
        s = strchr(line, '[');
        if (!s)
        { continue; }
        s++;
        e = strchr(s, ']');
        if (!e)
        { continue; }
        *e = 0;
        if (strcmp(s, name) == 0)
        {
            sectionpos = cpos = f->pos();
            sectionline = cline;
            sectionnp = cnp;
            return true;
        };
    };
    exit:
    cpos = f->pos();
    return false;
}

bool MTConfigFile::getparameter(const char *paramname, void *value, int desiredtype, int size)
{
    char *p, *v, *e;
    int x, tmpi;
    unsigned int tmpl;
    double tmpd;
    char delim;
    char line[256];

    if ((!f) || (sectionpos < 0))
    { return false; }
    f->seek(sectionpos, MTF_BEGIN);
    cpos = sectionpos;
    cline = sectionline;
    cnp = sectionnp;
    cont:
    while(!f->eof())
    {
        if (np)
        {
            for(x = cnp; x < np->nitems; x++)
            {
                NP &mnp = ((NP *) np->d)[x];
                if (mnp.line == cline)
                {
                    x = mnp.nlines;
                    while(x-- > 0)
                    {
                        if (f->eof())
                        { goto exit; }
                        f->readln(line, 256);
                        cline++;
                    };
                    strncpy(line, mnp.buf, 256);
                    cnp++;
                    goto skip;
                };
            };
        };
        cline++;
        if (!f->readln(line, 256))
        { continue; }
        skip:
        if (line[0] == '#')
        { continue; }
        if (strchr(line, '['))
        { return false; }
        p = line;
        while((*p == ' ') || (*p == '\t'))
        {
            p++;
            if (*p == 0)
            { goto cont; }
        };
        v = p;
        while((*v != ' ') && (*v != '\t') && (*v != '=') && (*v != ':'))
        {
            v++;
            if (*v == 0)
            { goto cont; }
        };
        *v++ = 0;
        if (strcmp(p, paramname))
        { continue; }
        while((*v == ' ') || (*v == '\t') || (*v == ':') || (*v == '='))
        {
            v++;
            if (*v == 0)
            { return false; }
        };
        if ((desiredtype == MTCT_STRING) || (desiredtype == MTCT_BINARY))
        {
            delim = *v++;
            if ((delim != '\'') && (delim != '"'))
            { return false; }
            e = v;
            while((*e) && (*e != delim)) e++;
            *e = 0;
            goto ok;
        };
        e = v;
        while((*e) && (*e != ' ') && (*e != '\t')) e++;
        *e = 0;
        goto ok;
    };
    exit:
    return false;
    ok:
    switch (desiredtype)
    {
        case MTCT_SINTEGER:
            tmpi = atoi(v);
            if (size == sizeof(char))
            {
                *(char *) value = (char) tmpi;
            }
            else if (size == sizeof(short))
            {
                *(short *) value = (short) tmpi;
            }
            else if (size == sizeof(int))
            {
                *(int *) value = tmpi;
            }
            else
            { return false; }
            break;
        case MTCT_UINTEGER:
            tmpl = atol(v);
            if (size == sizeof(char))
            {
                *(unsigned char *) value = (unsigned char) tmpl;
            }
            else if (size == sizeof(short))
            {
                *(unsigned short *) value = (unsigned short) tmpl;
            }
            else if (size == sizeof(int))
            {
                *(unsigned int *) value = tmpl;
            }
            else
            { return false; }
            break;
        case MTCT_FLOAT:
            tmpd = atof(v);
            if (size == sizeof(float))
            {
                *(float *) value = (float) tmpd;
            }
            else if (size == sizeof(double))
            {
                *(double *) value = tmpd;
            }
            else
            { return false; }
            break;
        case MTCT_BOOLEAN:
            if ((v[0] == '1') || (v[0] == 'y') || (v[0] == 'Y') || (v[0] == 't') || (v[0] == 'T'))
            {
                *(bool *) value = true;
            }
            else
            { *(bool *) value = false; }
            break;
        case MTCT_STRING:
            strncpy((char *) value, v, size);
            break;
        case MTCT_BINARY:
            strncpy((char *) value, v, size);
            break;
    };
    return true;
}

bool MTConfigFile::createsection(const char *name)
{
    int x;
    char *s, *e;
    char line[256];

    if (!f)
    { return false; }
    line[0] = 0;
    sectionpos = sectionline = -1;
    cpos = cline = 0;
    cnp = sectionnp = 0;
    f->seek(0, MTF_BEGIN);
    while(!f->eof())
    {
        skip:
        if (np)
        {
            for(x = cnp; x < np->nitems; x++)
            {
                NP &mnp = ((NP *) np->d)[x];
                if (mnp.line == cline)
                {
                    x = mnp.nlines;
                    while(x-- > 0)
                    {
                        if (f->eof())
                        { goto exit; }
                        f->readln(line, 256);
                        cline++;
                    };
                    cnp++;
                    goto skip;
                };
            };
        };
        cline++;
        if (!f->readln(line, 256))
        { continue; }
        if (line[0] == '#')
        { continue; }
        s = strchr(line, '[');
        if (!s)
        { continue; }
        s++;
        e = strchr(s, ']');
        if (!e)
        { continue; }
        *e = 0;
        if (strcmp(s, name) == 0)
        {
            sectionpos = cpos = f->pos();
            sectionline = cline;
            sectionnp = cnp;
            return true;
        };
    };
    exit:
    if ((line[0] != 0) && (f->pos() > 0))
    { f->write(NL, sizeof(NL) - 1); }
    sprintf(line, "[%s]"NL, name);
    f->write(line, strlen(line));
    sectionpos = cpos = f->pos();
    sectionline = cline;
    sectionnp = cnp;
    return true;
}

bool MTConfigFile::setparameter(const char *paramname, void *value, int type, int size)
{
    char *p, *v;
    int x, l, lastline;
    NP *rnp = 0;
    char delim;
    bool found = false;
    char fmt[256];
    char line[256];

    if ((!f) || (sectionpos < 0))
    { return false; }
    f->seek(sectionpos, MTF_BEGIN);
    cpos = sectionpos;
    cline = lastline = sectionline;
    cnp = sectionnp;
    fmt[0] = 0;
    v = 0;
    cont:
    while(!f->eof())
    {
        v = 0;
        if (np)
        {
            rnp = 0;
            for(x = cnp; x < np->nitems; x++)
            {
                NP &mnp = ((NP *) np->d)[x];
                if (mnp.line == cline)
                {
                    x = mnp.nlines;
                    while(x-- > 0)
                    {
                        if (f->eof())
                        { goto exit; }
                        if (f->readln(line, 256))
                        {
                            lastline = ++cline;
                        }
                        else
                        { cline++; }
                    };
                    strncpy(line, mnp.buf, 256);
                    cnp++;
                    rnp = &mnp;
                    goto skip;
                };
            };
        };
        cline++;
        if (!f->readln(line, 256))
        { continue; }
        skip:
        if (line[0] == '#')
        { continue; }
        if (strchr(line, '['))
        { goto exit; }
        lastline = cline;
        p = line;
        while((*p == ' ') || (*p == '\t'))
        {
            p++;
            if (*p == 0)
            { goto cont; }
        };
        v = p;
        while((*v != ' ') && (*v != '\t') && (*v != '=') && (*v != ':'))
        {
            v++;
            if (*v == 0)
            { goto cont; }
        };
        if (strncmp(p, paramname, v - p))
        {
            v = 0;
            continue;
        };
        found = true;
        while((*v == ' ') || (*v == '\t') || (*v == ':') || (*v == '='))
        {
            v++;
            if (*v == 0)
            { goto exit; }
        };
        l = v - line;
        memcpy(fmt, line, l);
        fmt[l] = 0;
        if ((*v == '\'') || (*v == '\"'))
        {
            delim = *v++;
            while((*v) && (*v != delim)) v++;
            if (*v == delim)
            { v++; }
        }
        else
        {
            while((*v) && (*v != ' ') && (*v != '\t')) v++;
        };
        break;
    };
    exit:
    cpos = f->pos();
    if (fmt[0] == 0)
    {
        strcpy(fmt, paramname);
        strcat(fmt, "\t");
        if (strlen(paramname) < 8)
        { strcat(fmt, "\t"); }
    };
    switch (type)
    {
        case MTCT_SINTEGER:
            strcat(fmt, "%d");
            if ((v) && (*v))
            { strcat(fmt, v); }
            if (size == sizeof(char))
            {
                sprintf(line, fmt, *(char *) value);
            }
            else if (size == sizeof(short))
            {
                sprintf(line, fmt, *(short *) value);
            }
            else if (size == sizeof(int))
            { sprintf(line, fmt, *(int *) value); }
            break;
        case MTCT_UINTEGER:
            strcat(fmt, "%u");
            if ((v) && (*v))
            { strcat(fmt, v); }
            if (size == sizeof(char))
            {
                sprintf(line, fmt, *(unsigned char *) value);
            }
            else if (size == sizeof(short))
            {
                sprintf(line, fmt, *(unsigned short *) value);
            }
            else if (size == sizeof(int))
            { sprintf(line, fmt, *(unsigned int *) value); }
            break;
        case MTCT_FLOAT:
            strcat(fmt, "%f");
            if ((v) && (*v))
            { strcat(fmt, v); }
            if (size == sizeof(float))
            {
                sprintf(line, fmt, *(float *) value);
            }
            else if (size == sizeof(double))
            { sprintf(line, fmt, *(double *) value); }
            break;
        case MTCT_BOOLEAN:
            strcat(fmt, "%s");
            if ((v) && (*v))
            { strcat(fmt, v); }
            sprintf(line, fmt, (*(bool *) value) ? "yes" : "no");
            break;
        case MTCT_STRING:
        case MTCT_BINARY:
            strcat(fmt, "\"%s\"");
            if ((v) && (*v))
            { strcat(fmt, v); }
            sprintf(line, fmt, (char *) value);
            break;
    };
    if (rnp)
    {
        mtmemfree(rnp->buf);
        rnp->buf = (char *) mtmemalloc(strlen(line) + 1);
        strcpy(rnp->buf, line);
    }
    else
    {
        if ((!found) && (cpos == f->length()))
        {
            f->write(line, strlen(line));
            f->write(NL, sizeof(NL) - 1);
        }
        else
        {
            if (!np)
            { np = new MTArray(4, sizeof(NP)); }
            np->additems(cnp, 1);
            NP &mnp = ((NP *) np->d)[cnp];
            mnp.nlines = (found) ? 1 : 0;
            mnp.line = lastline - mnp.nlines;
            mnp.buf = (char *) mtmemalloc(strlen(line) + 1);
            strcpy(mnp.buf, line);
        };
    };
    return true;
}

const char *MTConfigFile::getfilename()
{
    return (f) ? f->url : "";
}

bool MTConfigFile::loaded()
{
    return (f != 0);
}
//---------------------------------------------------------------------------

