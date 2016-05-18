//---------------------------------------------------------------------------
//
//  MadTracker Extensions Manager
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTExtensions.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifdef _WIN32
#	include <windows.h>
#else

#	include <dlfcn.h>
#	include <sys/stat.h>
#	include <dirent.h>
#	include <unistd.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include "MTExtensions.h"
#include "MTData.h"
#include "MTInterface.h"
#include "MTConsole.h"
#include <MTXAPI/RES/MT3RES.h>
#include <iostream>

//---------------------------------------------------------------------------
static const char *mt3name = {"MadTracker"};

static const int mt3version = 0x30000;

MTXKey mt3key = {0, 0, 0, 0};

int next;

MTExtension *ext[MAX_EXTENSIONS];

MT3Interface *mi;

MTSystemInterface *si;

MTObjectsInterface *oi;

MTDisplayInterface *di;

MTGUIInterface *gi;

MTAudioInterface *ai;

MTDSPInterface *dspi;

MTWindow *lastseq, *lastpw, *lastiw, *lastow, *lastaw;

//---------------------------------------------------------------------------
bool MTCT editwindowproc(MTWinControl *window, MTCMessage &msg)
{
    MTWindow *last = 0;
    int x;

    if (msg.msg == MTCM_DESTROY)
    {
        MTWinControl &cparent = *window->parent;
        for(x = 0; x < cparent.ncontrols; x++)
        {
            MTWindow &cwindow = *(MTWindow *) cparent.controls[x];
            if ((&cwindow != window) && (cwindow.guiid == MTC_WINDOW) && ((cwindow.tag & MTO_TYPEMASK) == (window->tag & MTO_TYPEMASK)))
            {
                last = (MTWindow *) cparent.controls[x];
                break;
            };
        };
        switch (window->tag & MTO_TYPEMASK)
        {
            case MTO_MODULE:
                lastseq = last;
                break;
            case MTO_PATTERN:
                lastpw = last;
                break;
            case MTO_INSTRUMENT:
                lastiw = last;
                break;
            case MTO_OSCILLATOR:
                lastow = last;
                break;
            case MTO_AUTOMATION:
                lastaw = last;
                break;
        };
    };
    return false;
}

//---------------------------------------------------------------------------
MT3Interface::MT3Interface()
{
    type = mt3type;
    key = &mt3key;
    name = mt3name;
    version = mt3version;
    _ow = 0;
    console = new MTConsole(this);
}

MT3Interface::~MT3Interface()
{
    delete console;
}

MTXInterface *MT3Interface::getinterface(int id)
{
    extern void *instance;

    switch (id)
    {
        case 0:
            return (MTXInterface *) instance;
        case systemtype:
            return (MTXInterface *) si;
        case objectstype:
            return (MTXInterface *) oi;
        case displaytype:
            return (MTXInterface *) di;
        case guitype:
            return (MTXInterface *) gi;
        case audiotype:
            return (MTXInterface *) ai;
        case dsptype:
            return (MTXInterface *) dspi;
    };
    return 0;
}

MTPreferences *MT3Interface::getprefs()
{
    return &prefs;
}

void *MT3Interface::getcurrentuser()
{
    return &cuser;
}

int MT3Interface::getnummodules()
{
    int x;
    int n = 0;

    for(x = 0; x < 16; x++)
    {
        if (module[x])
        { n++; }
    }
    return n;
}

void *MT3Interface::getmodule(int id)
{
    int x;
    int n = 0;

    for(x = 0; x < 16; x++)
    {
        if (module[x])
        {
            if (n == id)
            {
                return module[x];
            }
            else
            { n++; }
        };
    };
    return 0;
}

void MT3Interface::addmodule(void *m)
{
    int x;

    FENTER1("MT3Interface::addmodule(%.8X)", module);
    for(x = 0; x < 16; x++)
    {
        if (module[x] == 0)
        {
            module[x] = (MTModule *) m;
            ::setmodule(m);
            break;
        };
    };
    LEAVE();
}

void MT3Interface::delmodule(void *m)
{
    int x;

    FENTER1("MT3Interface::delmodule(%.8X)", module);
    for(x = 0; x < 16; x++)
    {
        if (module[x] == (void *) m)
        {
            module[x] = 0;
            break;
        };
    };
    LEAVE();
}

void MT3Interface::setmodule(void *module)
{
    FENTER1("MT3Interface::setmodule(%.8X)", module);
    ::setmodule(module);
    LEAVE();
}

bool MT3Interface::addchannel()
{
    int x, i, mi, mc, ndel;
    int totchan;
    double tot, max, cpupchan;
    InstrumentInstance *c[16];

    ENTER("MT3Interface::addchannel");
    MTTRY if (output)
        { output->lock->lock(); }
        totchan = 0;
        tot = 0.0;
        max = (output) ? output->maxcpu : 0.75;
        for(x = 0; x < 16; x++)
        {
            MTModule &cmodule = *module[x];
            if ((&cmodule) && (!cmodule.lockread))
            {
                if (cmodule.cpu)
                { tot += cmodule.cpu->getcpu(0); }
                totchan += cmodule.playstatus.nchannels;
            };
        };
        cpupchan = (totchan > 0) ? tot / totchan : 0.0;
        cpupchan *= 2.0;
        mc = 0;
        mi = 256;
        ndel = 0;
        while(tot >= max)
        {
            if (cpupchan <= 0.0)
            { break; }
            mc = -1;
            for(x = 0; x < 16; x++)
            {
                MTModule &cmodule = *module[x];
                if ((&cmodule) && (!cmodule.lockread))
                {
                    c[x] = cmodule.getlessimportantchannel(&i);
                    if (i < mi)
                    { mc = x; }
                };
            };
            if (mc >= 0)
            {
                module[mc]->delchannel(c[mc]);
            }
            else
            { break; }
            tot -= cpupchan;
            if (++ndel == 8)
            { break; }
        }; MTCATCH mc = 0; MTEND
    if (output)
    { output->lock->unlock(); }
    LEAVE();
    return (mc >= 0);
}

void MT3Interface::notify(void *object, int notify, int param)
{
    int x;
    MTObject &cobject = *(MTObject *) object;
    MTWindow *window;

    FENTER3("MT3Interface::notify(%.8X,%d,%d)", object, notify, param);
#	if defined(_DEBUG) && defined(_DEBUG_NOTIFY)
    char buffer[256],action[32],type[32];
    switch (cobject.objecttype & MTO_TYPEMASK){
    case MTO_MODULE:
        strcpy(type,"module");
        break;
    case MTO_AUTOMATION:
        strcpy(type,"automation");
        break;
    case MTO_PATTERN:
        strcpy(type,"pattern");
        break;
    case MTO_INSTRUMENT:
        strcpy(type,"instrument");
        break;
    case MTO_OSCILLATOR:
        strcpy(type,"oscillator");
        break;
    case MTO_TRACK:
        strcpy(type,"track");
        break;
    };
    switch (notify){
    case MTN_NEW:
        strcpy(action,"Creating");
        break;
    case MTN_DELETE:
        strcpy(action,"Deleting");
        break;
    case MTN_MODIFY:
        strcpy(action,"Modifying");
        break;
    case MTN_LOCK:
        strcpy(action,"Locking");
        break;
    case MTN_UNLOCK:
        strcpy(action,"Unlocking");
        break;
    case MTN_RENAME:
        strcpy(action,"Renaming");
        break;
    };
    sprintf(buffer,"%%s - [Notify] %s %s #%d %s"NL,action,type,cobject.id,cobject.name);
    LOGD(buffer);
#	endif
    switch (notify)
    {
        case MTN_DELETE:
            window = 0;
            switch (cobject.objecttype & MTO_TYPEMASK)
            {
                case MTO_MODULE:
                    if (cmodule == &cobject)
                    { cmodule = 0; }
                    window = lastseq;
                    for(x = 0; x < 16; x++)
                    {
                        if (module[x] == (MTModule *) &cobject)
                        {
                            module[x] = 0;
                            break;
                        };
                    };
                    break;
                case MTO_PATTERN:
                    window = lastpw;
                    break;
                case MTO_INSTRUMENT:
                    window = lastiw;
                    break;
                case MTO_OSCILLATOR:
                    window = lastow;
                    break;
                case MTO_AUTOMATION:
                    window = lastaw;
                    break;
            };
            if ((window) && (window->uid == (int) object))
            {    //FIXME Ticks off -fpermissive. Which in this case isn't as easy to fix.
                MTCMessage msg = {MTCM_CLOSE, 0, window};
                window->message(msg);
            };
        case MTN_NEW:
            if (cobject.parent)
            { cobject.parent->setmodified(1, (cobject.objecttype & MTO_TYPEMASK) | MTOM_ADDDELETE); }
            break;
    };
    LEAVE();
}

bool MT3Interface::editobject(void *object, bool newwindow)
{
    MTObject &cobject = *(MTObject *) object;
    MTWindow *window;
    int type;
    bool created = false;

    if ((!object) || (!gi))
    { return false; }
    FENTER2("MT3Interface::editobject(%.8X,%d)", object, newwindow);
    switch (cobject.objecttype & MTO_TYPEMASK)
    {
        case MTO_MODULE:
            setmodule((MTModule *) object);
            window = lastseq;
            type = MTW_sequencer;
            break;
        case MTO_PATTERN:
            window = lastpw;
            type = MTW_pattern;
            break;
        case MTO_INSTRUMENT:
            window = lastiw;
            type = MTW_instrument;
            break;
        case MTO_OSCILLATOR:
            window = lastow;
            type = MTW_oscillator;
            break;
        case MTO_AUTOMATION:
            window = lastaw;
            type = 10000;
            break;
        default:
            LEAVE();
            return oi->editobject(&cobject, 0);
    };
    if (!window)
    {
        window = gi->loadwindow(mtres, type, mtdsk, false);
        if (!window)
        {
            LEAVE();
            return false;
        };
        window->messageproc = editwindowproc;
        window->flags |= MTCF_FREEONCLOSE;
        MTTabControl *tabs = (MTTabControl *) window->getcontrolfromuid(1);
        if ((tabs) && (tabs->guiid == MTC_TABCONTROL))
        { tabs->setautohidetabs(true); }
        created = true;
    };
    if (oi->editobject(&cobject, window))
    {
        switch (cobject.objecttype & MTO_TYPEMASK)
        {
            case MTO_MODULE:
                lastseq = window;
                break;
            case MTO_PATTERN:
                lastpw = window;
                break;
            case MTO_INSTRUMENT:
                lastiw = window;
                break;
            case MTO_OSCILLATOR:
                lastow = window;
                break;
            case MTO_AUTOMATION:
                lastaw = window;
                break;
        };
        window->uid = (int) object;
        window->tag = cobject.objecttype;
        window->parent->bringtofront(window);
        window->switchflags(MTCF_HIDDEN, false);
        mtdsk->focus(window);
        LEAVE();
        return true;
    }
    else
    {
        if (created)
        { gi->deletewindow(window); }
        LEAVE();
        return false;
    };
}

void *MT3Interface::getconf(const char *name, bool user)
{
    _MTConf *cconf = 0;
    char buf[1024];

    strcpy(buf, prefs.syspath[(user) ? SP_USERCONFIG : SP_CONFIG]);
    strcat(buf, name);
    strcat(buf, ".conf");
    if (confs)
    {
        _MTConf *econf;
        cconf = (_MTConf *) confs->getitem(buf);
        if (cconf)
        {
            if (si->sync_inc(&cconf->refcount) <= 0)
            {
                LOGD("%s - [MT3] INFO: Synchronization trigger 1 in getconf()"
                         NL);
                si->sync_dec(&cconf->refcount);
                cconf = 0;
            };
        };
        confs->reset();
        while((econf = (_MTConf *) confs->next()))
        {
            if ((econf->refcount == 0) && (econf->lastuse < si->syscounter() - 60000))
            {
                if (si->sync_inc(&econf->refcount) < 0)
                {
                    si->configclose(econf->conf);
                    confs->delitemfromid(confs->getitemid(econf), true);
                }
                else
                {
                    LOGD("%s - [MT3] INFO: Synchronization trigger 2 in getconf()"
                             NL);
                    si->sync_dec(&econf->refcount);
                };
            };
        };
    }
    else
    {
        return si->configopen(buf);
    };
    if (cconf == 0)
    {
        MTConfigFile *conf = si->configopen(buf);
        if (conf)
        {
            cconf = mtnew(_MTConf);
            cconf->refcount = 1;
            cconf->conf = conf;
            confs->additem(buf, cconf);
        }
        else
        { return 0; }
    };
    cconf->lastuse = si->syscounter();
    return cconf->conf;
}

void MT3Interface::releaseconf(void *conf)
{
    if (confs)
    {
        MTConfigFile &cconf = *(MTConfigFile *) conf;
        _MTConf *econf;
        econf = (_MTConf *) confs->getitem((char *) cconf.getfilename());
        if (econf)
        {
            si->sync_dec(&econf->refcount);
        }
        else
        { si->configclose((MTConfigFile *) conf); }
    }
    else
    {
        si->configclose((MTConfigFile *) conf);
    };
}

int MT3Interface::addrefreshproc(RefreshProc proc, void *param)
{
    static int rpid;

    if (!refreshprocs)
    { return -1; }
    FENTER2("MT3Interface::addrefreshproc(%.8X,%.8X)", proc, param);
    RefreshStruct *rs = mtnew(RefreshStruct);
    rs->id = rpid;
    rs->proc = proc;
    rs->param = param;
    refreshprocs->push(rs);
    si->memfree(rs);
    LEAVE();
    return rpid++;
}

void MT3Interface::delrefreshproc(int id)
{
    int x = 0;
    RefreshStruct *rs;

    FENTER1("MT3Interface::delrefreshproc(%d)", id);
    refreshprocs->reset();
    while((rs = (RefreshStruct *) refreshprocs->next()))
    {
        if (rs->id == id)
        {
            refreshprocs->delitems(x, 1);
            refreshprocs->remove(rs);
            LEAVE();
            return;
        };
        x++;
    };
    LOGD("%s - [MT3] WARNING: Procedure not found!"
             NL);
    LEAVE();
}

char *MT3Interface::getextension(void *ptr)
{
    int x;
    char *e;

    for(x = 0; x < next; x++)
    {
        MTExtension &cext = *ext[x];
        if ((ptr >= cext.start) && (ptr < cext.end))
        {
            e = strrchr(cext.filename, '/');
            if (!e)
            { return cext.filename; }
            return e + 1;
        };
    };
    return 0;
}

bool MT3Interface::canclose()
{
    int x, res;
    bool modified = false;

    if (candesign)
    {
        for(x = 0; x < mtdsk->ncontrols; x++)
        {
            MTWindow &cwnd = *(MTWindow *) mtdsk->controls[x];
            if (cwnd.guiid != MTC_WINDOW)
            { continue; }
            if (cwnd.modified)
            {
                modified = true;
                break;
            };
        };
        if (modified)
        {
            res = si->dialog("Save last UI changes?", "", MTD_YESNOCANCEL, MTD_QUESTION, 10000);
            if (res == MTDR_TIMEOUT)
            { res = 0; }
            if ((res == MTDR_CANCEL) || (res == 2))
            { return false; }
            if (res == 0)
            {
                gi->setmouseshape(DCUR_WORKING);
                for(x = 0; x < mtdsk->ncontrols; x++)
                {
                    MTWindow &cwnd = *(MTWindow *) mtdsk->controls[x];
                    if (cwnd.guiid != MTC_WINDOW)
                    { continue; }
                    if (cwnd.modified)
                    { gi->savewindow(&cwnd); }
                };
                gi->restoremouseshape();
            };
        };
    };
    return true;
}

void *MT3Interface::getconsole()
{
    return console;
}

int MT3Interface::processinput(const char *input)
{
    char *param;
    char command[256];

    mtmemzero(command, sizeof(command));
    strncpy(command, input, sizeof(command) - 1);
    param = strchr(command, ' ');
    if (param)
    {
        *param++ = 0;
    };
    if ((stricmp(command, "open") == 0) || (stricmp(command, "load") == 0))
    {
        if (param)
        { loadmodule(param); }
        return 1;
    }
    else if ((stricmp(command, "exit") == 0) || (stricmp(command, "quit") == 0))
    {
#		ifdef _WIN32
        PostQuitMessage(0);
#		else
        extern bool running;
        running = false;
#		endif
        return 1;
    }
    else
    {
        return 0;
    };
}

//---------------------------------------------------------------------------
MTExtension *addextension()
{
    if (next == MAX_EXTENSIONS)
    { return 0; }
    ext[next] = (MTExtension *) calloc(1, sizeof(MTExtension));
    ext[next]->id = next;
    return ext[next++];
}

void delextension(MTExtension *dext)
{
    ext[dext->id] = ext[next - 1];
    ext[dext->id]->id = dext->id;
    ext[--next] = 0;
    free(dext);
}

//FIXME This fails (and thus, segfaults) in the current build system.
// The directory "Extensions" does not exist after the build.
bool loadExtension(const char *file)
{
    MTXMainCall *mtxmain;
    MTXInterfaces *xis;
    int x;

#	ifdef _WIN32
    HINSTANCE hi = LoadLibrary(file);
#	else
    void *hi = dlopen(file, RTLD_LAZY);
#	endif
    if (hi)
    {
#		ifdef _WIN32
        mtxmain = (MTXMainCall*)GetProcAddress(hi,"MTXMain");
#		else
        mtxmain = (MTXMainCall *) dlsym(hi, "MTXMain");
        // dlsym is from POSIX header dlfcn.h, which deals with dynamic linking.
        // what the shitnuggets.
#		endif
        if (mtxmain)
        {
            MTExtension *cext = addextension();
            cext->library = (void *) hi;
            cext->filename = (char *) malloc(strlen(file) + 1);
            strcpy(cext->filename, file);
            xis = mtxmain(mi);
            cext->i = xis;
            cext->loaded = true;
            cext->system = true;
            cext->start = hi;
            cext->end = hi;
            for(x = 0; x < xis->ninterfaces; x++)
            {
                MTXInterface *xi = xis->interfaces[x];
                switch (xi->type)
                {
                    case systemtype:
                        si = (MTSystemInterface *) xi;
                        break;
                    case objectstype:
                        oi = (MTObjectsInterface *) xi;
                        break;
                    case displaytype:
                        di = (MTDisplayInterface *) xi;
                        break;
                    case guitype:
                        gi = (MTGUIInterface *) xi;
                        break;
                    case audiotype:
                        ai = (MTAudioInterface *) xi;
                        break;
                    case dsptype:
                        dspi = (MTDSPInterface *) xi;
                        break;
                    default:
                        cext->system = false;
                        break;
                };
            };
            return true;
        }
        else
        {
            char buf[1024];
#			ifdef _WIN32
            sprintf(buf,"Cannot initialize %s!"NL"Error %d",file,GetLastError());
            FreeLibrary(hi);
            MessageBox(0,buf,"Error",MB_ICONERROR|MB_OK);
#			else
            sprintf(buf, "Cannot initialize %s!"NL"%s", file, dlerror());
            dlclose(hi);
            fputs(buf, stderr);
            fputs(NL, stderr);
#			endif
        };
    }
    else
    {
        char buf[1024];
#		ifdef _WIN32
        sprintf(buf,"Cannot load %s!"NL"Error %d",file,GetLastError());
        MessageBox(0,buf,"Error",MB_ICONERROR|MB_OK);
#		else
        sprintf(buf, "Cannot load %s!"NL"%s", file, dlerror());
        fputs(buf, stderr);
        fputs(NL, stderr);
#		endif
    };
    return false;
}

//TODO Use platform-independant mechanisms in this function. Maybe boost::filesystem, maybe std::fstream is already enough.
void loadDirectory(const char *dir)
{
    char find[512];
    char *e, *ext;
#ifdef _WIN32
    WIN32_FIND_DATA fd;
    HANDLE fh;
    bool ok;
#else
    DIR *d;
    struct dirent *de;
    struct stat s;
#endif

    strcpy(find, dir);
    e = strchr(find, 0);
#	ifdef _WIN32
    strcat(find,"*.*");
    fh = FindFirstFile(find,&fd);
    ok = (fh!=INVALID_HANDLE_VALUE);
    while (ok){
        if (fd.cFileName[0]!='.'){
            strcpy(e,fd.cFileName);
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                strcat(e,"\\");
                SetCurrentDirectory(find);
                loadDirectory(find);
            }
            else{
                ext = strrchr(fd.cFileName,'.');
                if (stricmp(ext,".mtx")==0){
                    loadExtension(find);
                };
            };
        };
        ok = FindNextFile(fh,&fd)!=0;
    };
    FindClose(fh);
#	else
    if (*(e - 1) != '/')
    {
        *e++ = '/';
        *e = 0;
    };
    d = opendir(find);
    std::clog << "d: " << d << "\n";
    while((de = readdir(d)))
    {    //FIXME readdir segfaults here if d is null.
        if (de->d_name[0] != '.')
        {
            strcpy(e, de->d_name);
            stat(find, &s); // see POSIX header stat.h (that one's documented at least :-) )
            if ((s.st_mode & S_IFMT) == S_IFDIR)
            {
                chdir(find); // POSIX header unistd.h
                loadDirectory(find); // fuck.
            }
            else
            {
                ext = strrchr(de->d_name, '.'); // find last '.' in d_name
                if (stricmp(ext, ".mtx") == 0) // defined as strcasecmp.
                { loadExtension(find); }
            };
        };
    };
    closedir(d);
#	endif
}

void loadExtensions()
{
#ifdef _WIN32
    int oldmode;
#endif

    mi = new MT3Interface();
#	ifdef _WIN32
    oldmode = SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
    SetCurrentDirectory(prefs.syspath[SP_EXTENSIONS]);
#	else
    chdir(prefs.syspath[SP_EXTENSIONS]);
#	endif
    loadDirectory(prefs.syspath[SP_EXTENSIONS]);
#	ifdef _WIN32
    SetErrorMode(oldmode);
    SetCurrentDirectory(prefs.syspath[SP_ROOT]);
#	else
    chdir(prefs.syspath[SP_ROOT]);
#	endif
}

void unloadExtensions()
{
    while(next > 0)
    {
#		ifdef _WIN32
        FreeLibrary((HINSTANCE)ext[0]->library);
#		else
        dlclose(ext[0]->library);
#		endif
        free(ext[0]->filename);
        delextension(ext[0]);
    };
    if (mi)
    {
        delete mi;
        mi = 0;
    };
}

bool initExtensions()
{
    int x, y, displayok, audiook;
    bool ok = true;
    bool ok2;
    bool ok3 = true;
    bool required = false;
    char *error, *e;
    static const char *sok = {"OK --"NL};
    static const char *sfailed = {"FAILED --"NL};

    ENTER("initExtensions");
    displayok = audiook = 0;
    error = (char *) si->memalloc(1024, 0);
    strcpy(error, "The following extensions could not be initialized:"NL);
    LOGD("Found extensions:"
             NL);
    for(x = 0; x < next; x++)
    {
        MTExtension &cext = *ext[x];
        e = strrchr(cext.filename, '/');
        if (!e)
        { e = strrchr(cext.filename, '\\'); }
        if (!e)
        {
            e = cext.filename;
        }
        else
        { e++; }
        si->getlibmemoryrange(cext.start, 0, &cext.start, &y);
        cext.end = (char *) cext.start + y;
        FLOG3("%s (%.8X - %.8X):"
                  NL, e, cext.start, cext.end);
        for(y = 0; y < cext.i->ninterfaces; y++)
        {
            int v = cext.i->interfaces[y]->version;
            LOG("  ");
            LOG(cext.i->interfaces[y]->name);
            FLOG3(" %d.%d.%d"
                      NL, v >> 16, (v >> 8) & 0xFF, v & 0xFF);
        };
    };

    LOGD("%s - Initializing extensions..."
             NL);

    ok2 = false;
    if (di)
    {
        MTTRY ok2 = di->init(); MTCATCH MTEND
    };
    if (!ok2)
    {
        ok = false;
        required = true;
        strcat(error, "MTXDisplay (*)"NL);
    }
    else
    { displayok++; }
    LOG("-- Display ");
    LOG((ok2) ? sok : sfailed);

    ok2 = false;
    if (gi)
    {
        MTTRY ok2 = gi->init(); MTCATCH MTEND
    };
    if (!ok2)
    {
        ok = false;
        required = true;
        strcat(error, "MTXGUI (*)"NL);
    }
    else
    { displayok++; }
    LOG("-- GUI ");
    LOG((ok2) ? sok : sfailed);

    if ((displayok != 2) || (!initInterface()))
    {
        displayok = 0;
        LOGD("%s - [MT3] ERROR: Cannot initialize the interface!"
                 NL);
#		ifdef _WIN32
        si->dialog("Cannot initialize the interface!","MadTracker",MTD_OK,MTD_ERROR,0);
        if ((!ok) || (!ok3)){
            strcat(error,"(*) = Required extensions"NL);
            if (required) strcat(error,"One or more required extensions could not be found or initialized!"NL"You should re-install MadTracker.");
            si->dialog(error,"MadTracker",MTD_OK,(required)?(MTD_ERROR|MTD_MODAL):MTD_EXCLAMATION,0);
        };
        si->memfree(error);
        LEAVE();
        return false;
#		endif
    };
    if (displayok == 2)
    { ok &= startInterface(); }

#	ifdef _WIN32
    if (exitasap) goto start;
#	endif

    ok2 = false;
    if (oi)
    {
        MTTRY ok2 = oi->init(); MTCATCH MTEND
    };
    if (!ok2)
    {
        ok = false;
        required = true;
        strcat(error, "MTXObjects (*)"NL);
    };
    LOG("-- Objects ");
    LOG((ok2) ? sok : sfailed);

    ok2 = false;
    if (ai)
    {
        MTTRY ok2 = ai->init(); MTCATCH MTEND
    };
    if (!ok2)
    {
        strcat(error, "MTXAudio"NL);
    }
    else
    { audiook++; }
    LOG("-- Audio ");
    LOG((ok2) ? sok : sfailed);

    ok2 = false;
    if (ai)
    {
        MTTRY ok2 = dspi->init(); MTCATCH MTEND
    };
    if (!ok2)
    {
        strcat(error, "MTXDSP"NL);
    }
    else
    { audiook++; }
    LOG("-- DSP ");
    LOG((ok2) ? sok : sfailed);

    if (audiook == 2)
    {
        output = ai->getoutput();
        ai->activatedevices();
    };

    ok &= ok2;

    for(x = 0; x < next; x++)
    {
        MTExtension &cext = *ext[x];
        if (!cext.system)
        {
            for(y = 0; y < cext.i->ninterfaces; y++)
            {
                ok2 = false;
                MTTRY ok2 = cext.i->interfaces[y]->init(); MTCATCH MTEND
                if (!ok2)
                {
                    ok3 = false;
                    e = strrchr(cext.filename, '\\');
                    if (!e)
                    { e = strrchr(cext.filename, '/'); }
                    if (e)
                    {
                        e++;
                        strcat(error, e);
                        strcat(error, ": ");
                        strcat(error, cext.i->interfaces[y]->name);
                        strcat(error, NL);
                    };
                };
            };
        };
    };
#ifdef _WIN32
    start:
#endif
    if ((!ok) || (!ok3))
    {
        strcat(error, "(*) = Required extensions"NL);
        if (required)
        { strcat(error, "One or more required extensions could not be found or initialized!"NL"You should re-install MadTracker."); }
        si->dialog(error, "MadTracker", MTD_OK, (required) ? (MTD_ERROR | MTD_MODAL) : MTD_EXCLAMATION, 0);
    };
    si->memfree(error);

    LEAVE();
    return ok;
}

bool startExtensions()
{
    int x, y;

    LOGD("%s - Starting extensions..."
             NL);
    for(x = 0; x < next; x++)
    {
        MTExtension &cext = *ext[x];
        for(y = 0; y < cext.i->ninterfaces; y++)
        {
            if ((cext.i->interfaces[y]->status & MTX_INITIALIZED) == 0)
            { continue; }
            if (cext.i->interfaces[y] == gi)
            { continue; }
            MTTRY cext.i->interfaces[y]->start(); MTCATCH FLOGD1("%s - ERROR: Exception while starting '%s'!"
                                                                     NL, cext.i->interfaces[y]->name); MTEND
        };
    };

    showInterface();
    LEAVE();
    return true;
}

void stopExtensions()
{
    int x, y;

    ENTER("stopExtensions");
    LOGD("%s - Stopping current modules..."
             NL);
    for(x = 0; x < 16; x++)
    {
        if (module[x])
        { module[x]->play(PLAY_STOP); }
    };
    LOGD("%s - Stopping extensions..."
             NL);
    for(x = next - 1; x >= 0; x--)
    {
        MTExtension &cext = *ext[x];
        for(y = cext.i->ninterfaces - 1; y >= 0; y--)
        {
            MTXInterface *ci = cext.i->interfaces[y];
            if ((ci == gi) || (ci == si))
            { continue; }
            MTTRY ci->stop(); MTCATCH LOGD("%s - ERROR: Exception while stopping '");
                LOG(cext.i->interfaces[y]->name);
                LOG("'!"
                        NL); MTEND
        };
    };
    LOGD("%s - Stopping interface..."
             NL);
    stopInterface();
    LEAVE();
}

void uninitExtensions()
{
    int x, y;

    ENTER("uninitExtensions");
    LOGD("%s - Uninitializing extensions..."
             NL);
    for(x = next - 1; x >= 0; x--)
    {
        MTExtension &cext = *ext[x];
        if (!cext.system)
        {
            for(y = cext.i->ninterfaces - 1; y >= 0; y--)
            {
                MTTRY if (cext.i->interfaces[y]->status & MTX_INITIALIZED)
                    {
                        cext.i->interfaces[y]->uninit();
                    }; MTCATCH LOGD("%s - ERROR: Exception while uninitializing '");
                    LOG(cext.i->interfaces[y]->name);
                    LOG("'!"
                            NL); MTEND
            };
        };
    };
    MTTRY if ((dspi) && (dspi->status & MTX_INITIALIZED))
        { dspi->uninit(); }MTCATCH LOGD("%s - ERROR: Exception while uninitializing MTDSP!"
                                            NL); MTEND
    MTTRY if ((ai) && (ai->status & MTX_INITIALIZED))
        { ai->uninit(); }MTCATCH LOGD("%s - ERROR: Exception while uninitializing MTAudio!"
                                          NL); MTEND
    LOGD("%s - Uninitializing interface..."
             NL);
    uninitInterface();
    MTTRY if ((gi) && (gi->status & MTX_INITIALIZED))
        { gi->uninit(); }MTCATCH LOGD("%s - ERROR: Exception while uninitializing MTGUI!"
                                          NL); MTEND
    MTTRY if ((di) && (di->status & MTX_INITIALIZED))
        { di->uninit(); }MTCATCH LOGD("%s - ERROR: Exception while uninitializing MTDisplay!"
                                          NL); MTEND
    MTTRY if ((oi) && (oi->status & MTX_INITIALIZED))
        { oi->uninit(); }MTCATCH LOGD("%s - ERROR: Exception while uninitializing MTObjects!"
                                          NL); MTEND
    LEAVE();
}

bool initSystem()
{
    try
    {
        if (si)
        { return si->init(); }
    }
    catch(...)
    {
#		ifdef _WIN32
        MessageBox(0,"Exception while initializing MTSystem!","System Error",MB_ICONERROR|MB_OK);
#		else
        fputs("Exception while initializing MTSystem!"NL, stderr);
#		endif
    };
    return false;
}

void uninitSystem()
{
    try
    {
        if ((si) && (si->status & MTX_INITIALIZED))
        { si->uninit(); }
    }
    catch(...)
    {
#		ifdef _WIN32
        MessageBox(0,"Exception while uninitializing MTSystem!","System Error",MB_ICONERROR|MB_OK);
#		else
        fputs("Exception while uninitializing MTSystem!"NL, stderr);
#		endif
    };
}
//---------------------------------------------------------------------------
