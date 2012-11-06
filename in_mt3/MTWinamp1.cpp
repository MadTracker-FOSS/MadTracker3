//---------------------------------------------------------------------------
//
//	MadTracker 3 Winamp/XMMS Plugin
//
//		Platforms:	All
//		Processors: x86
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTWinamp1.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include "in_mt3.h"
#include "MTPluginInterface.h"
#ifndef _WIN32
#	include <stdio.h>
#	include <stdlib.h>
#endif
//---------------------------------------------------------------------------
WaveDevice wadevice;
WaveOutput waoutput;
MTModule *module = 0;
char lastfile[1024] = "";
#ifdef _WIN32
	int paused = 0;
	int maxlatency;
#endif
MTThread *playerthread;
bool builtin = true;
//---------------------------------------------------------------------------
void MTCT ri16(sample *source,void *dest,int count,int nchannels,int channel)
{
	static const sample f32767 = 32767.0;

#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			push	ebx
			sub		esp,4
			mov		ecx,count
			mov		esi,source
			test	ecx,ecx
			mov		edi,dest
			jz		mexit
			mov		ebx,nchannels
			mov		eax,channel
			sal		ebx,1
			sal		eax,1
			sub		edi,ebx
			add		edi,eax
			ALIGN	8
		render:
			fld		a_sample ptr [esi]
			fmul	f32767
			fistp	dword ptr [esp]
			add		edi,ebx
			mov		eax,[esp]
			add		esi,s_sample
			cmp		eax,32767
			jle		lsatok
			mov		eax,32767
			jmp		satok
		lsatok:
			cmp		eax,-32768
			jge		satok
			mov		eax,-32768
		satok:
			dec		ecx
			mov		[edi],ax
			jnz		render
		mexit:
			add		esp,4
			pop		ebx
			pop		edi
			pop		esi
		};
#	else
		asm ("\
			mov		%[channel],%%eax\n\
			subl	$4,%%esp\n\
			testl	%%ecx,%%ecx\n\
			jz		_mexit2\n\
			sall	$1,%%ebx\n\
			sall	$1,%%eax\n\
			subl	%%ebx,%%edi\n\
			addl	%%eax,%%edi\n\
			.align	8\n\
		_render2:\n\
			fld"spls"	(%%esi)\n\
			fmul	%[f32767]\n\
			fistpl	(%%esp)\n\
			addl	%%ebx,%%edi\n\
			movl	(%%esp),%%eax\n\
			addl	$"spll",%%esi\n\
			cmpl	$32767,%%eax\n\
			jle		_lsatok2\n\
			movl	$32767,%%eax\n\
			jmp		_satok2\n\
		_lsatok2:\n\
			cmpl	$-32768,%%eax\n\
			jge		_satok2\n\
			movl	$-32768,%%eax\n\
		_satok2:\n\
			decl	%%ecx\n\
			movw	%%ax,(%%edi)\n\
			jnz		_render2\n\
		_mexit2:\n\
			addl	$4,%%esp\n\
			"
			:
			:[nchannels]"b"(nchannels),[count]"c"(count),[source]"S"(source),[dest]"D"(dest),[f32767]"m"(f32767),[channel]"m"(channel)
			:"eax"
			);
#	endif
}

int MTCT PlayerThread(MTThread *thread,void *param)
{
	int l,t;
	short *buf;
	const int bufsamples = 576*2;

	buf = (short*)si->memalloc(sizeof(short)*bufsamples,MTM_ZERO);
	si->setprivatedata(0,si->memalloc(sizeof(sample)*PRIVATE_BUFFER,MTM_ZERO));
	module->setpos(0);
	module->play(PLAY_SONG);
	while (!thread->terminated){
#		ifdef _WIN32
			if (module->playstatus.flags==PLAY_STOP){
				mod.outMod->CanWrite();
				if (mod.outMod->IsPlaying()==0){
					PostMessage(mod.hMainWindow,WM_USER+2,0,0);
					break;
				};
				si->syswait(10);
				continue;
			};
			if (mod.outMod->CanWrite()<(bufsamples<<(mod.dsp_isactive()?1:0))){
				si->syswait(50);
				continue;
			};
			waoutput.playlng = bufsamples/2;
			mtmemzero(wadevice.master->buffer[0],waoutput.playlng*sizeof(sample));
			mtmemzero(wadevice.master->buffer[1],waoutput.playlng*sizeof(sample));
			MTTRY
				module->process(&waoutput);
			MTCATCH
				module->play(PLAY_STOP);
			MTEND
			ri16(wadevice.master->buffer[0],buf,waoutput.playlng,2,0);
			ri16(wadevice.master->buffer[1],buf,waoutput.playlng,2,1);

			t = mod.outMod->GetWrittenTime();
			mod.SAAddPCMData(buf,2,16,t);
			mod.VSAAddPCMData(buf,2,16,t);
			l = mod.dsp_dosamples(buf,waoutput.playlng,16,2,44100)*4;
			mod.outMod->Write((char*)buf,l);
#		else
			if (module->playstatus.flags==PLAY_STOP){
				si->syswait(50);
				continue;
			};
			l = mod.output->buffer_free();
			if (l<bufsamples*2){
				si->syswait(50);
				continue;
			};
			waoutput.playlng = bufsamples/2;
			mtmemzero(wadevice.master->buffer[0],waoutput.playlng*sizeof(sample));
			mtmemzero(wadevice.master->buffer[1],waoutput.playlng*sizeof(sample));
			module->process(&waoutput);
			ri16(wadevice.master->buffer[0],buf,waoutput.playlng,2,0);
			ri16(wadevice.master->buffer[1],buf,waoutput.playlng,2,1);

			mod.add_vis_pcm(mod.output->written_time(),FMT_S16_LE,2,bufsamples*2,buf);
			mod.output->write_audio(buf,bufsamples*2);
#		endif
	};
	si->memfree(si->getprivatedata(0));
	si->memfree(buf);
	playerthread = 0;
	return 0;
}
//---------------------------------------------------------------------------
// Common
//---------------------------------------------------------------------------
void init()
{
#ifdef _WIN32
	wadevice.name = "Winamp";
#else
	wadevice.name = "XMMS";
#endif
	wadevice.nchannels = 2;
	wadevice.bits = 16;
	wadevice.datasamples = 1024;
	waoutput.ndevices = 1;
	waoutput.device[0] = &wadevice;
	waoutput.frequency = 44100;
	waoutput.buffersamples = 1024;
	mtinterface = new MTPluginInterface();
	if (loadExtensions()){
		builtin = false;
		if (!initSystem()){
#			ifdef _WIN32
				MessageBox(0,"Cannot initialize the MTSystem extension!","System Error",MB_ICONEXCLAMATION|MB_OK);
#			else
				fputs("Cannot initialize the MTSystem extension!"NL,stderr);
#			endif
			return;
		};
		ai = new MTPluginDevice();
		initExtensions();
		startExtensions();
	}
	else{
		si = new MTSystemInterface();
		dspi = new MTDSPInterface();
		oi = new MTObjectsInterface();
		ai = new MTPluginDevice();
		si->init();
		dspi->init();
		oi->init();
		si->start();
		dspi->start();
		oi->start();
	};
	waoutput.lock = si->lockcreate();
	wadevice.master = (Track*)oi->newobject(MTO_TRACK,0,0);
}

void quit()
{
	if (playerthread) playerthread->terminate();
	if (module){
		oi->deleteobject(module);
		module = 0;
	};
	oi->deleteobject(wadevice.master);
	si->lockdelete(waoutput.lock);
	if (!builtin){
		stopExtensions();
		uninitExtensions();
		delete ai;
		unloadExtensions();
		uninitSystem();
		unloadExtensions();
	}
	else{
		oi->stop();
		dspi->stop();
		si->stop();
		oi->uninit();
		dspi->uninit();
		si->uninit();
		delete ai;
		delete oi;
		delete dspi;
		delete si;
	};
	delete mtinterface;
}

int isourfile(char *fn) 
{ 
	MTFile *f;
	mt_uint32 id;
	
	f = si->fileopen(fn,MTF_READ);
	if (f){
		f->read(&id,4);
		si->fileclose(f);
		if (id==0x3032544D) return 1;
	};
	return 0;
}
#ifdef _WIN32
//---------------------------------------------------------------------------
// Winamp only
//---------------------------------------------------------------------------
void config(HWND hwndParent)
{

}

void about(HWND hwndParent)
{
	MessageBox(hwndParent,"MadTracker 2.5 Player Winamp Plugin by Yannick Delwiche\r\n\r\n  http://www.madtracker.org","About MadTracker 2.5 Player",MB_OK);
}

int play(char *fn)
{
	FENTER1("play(%s)",fn);
	if (strcmp(lastfile,fn)){
		strcpy(lastfile,fn);
		if (module){
			module->play(PLAY_STOP);
			if (playerthread) playerthread->terminate();
			oi->deleteobject(module);
		};
		module = (MTModule*)oi->newobject(MTO_MODULE,0,0,0,true);
		if (!module){
			LOGD("%s - [in_mt3] Cannot create module object!"NL);
			LEAVE();
			return 1;
		};
		if (!oi->loadobject(module,lastfile,0)){
			oi->deleteobject(module);
			module = 0;
		}
		else module->lock(MTOL_LOCK,false);
	};
	if (!module){
		LOGD("%s - [in_mt3] No module!"NL);
		LEAVE();
		return -1;
	};
	maxlatency = mod.outMod->Open(44100,2,16,-1,-1);
	mod.SetInfo(module->ntracks*1000,44100/1000,2,1);
	mod.SAVSAInit(maxlatency,44100);
	mod.VSASetInfo(44100,2);
	mod.outMod->SetVolume(-666);
	if (!playerthread) playerthread = si->threadcreate(PlayerThread,true,true,module,MTT_HIGH);
	LEAVE();
	return 0;
}

void stop()
{
	ENTER("stop()");
	mod.outMod->Close();
	mod.SAVSADeInit();
	if (playerthread) playerthread->terminate();
	if (module)	module->play(PLAY_STOP);
	LEAVE();
}

void seek(int time_in_ms)
{
	if (!module) return;
	module->setpos(time_in_ms*module->playstatus.bpm/(1000*60));
	mod.outMod->Flush(time_in_ms);
}

void pause()
{
	paused = 1;
	mod.outMod->Pause(1);
}

void unpause()
{
	paused = 0;
	mod.outMod->Pause(0);
}

int ispaused()
{
	return paused;
}

int getlength()
{
	if (!module) return -1000;
	return 1000*60*module->loope/module->playstatus.bpm;
}

int getoutputtime()
{
	return mod.outMod->GetOutputTime();
}

void setvolume(int volume)
{
	mod.outMod->SetVolume(volume);
}

void setpan(int pan)
{
	mod.outMod->SetPan(pan);
}

void getfileinfo(char *filename,char *title,int *length_in_ms)
{
	MTMiniConfig *data;
	double bpm,beats;

	FENTER1("getfileinfo(%s)",filename);
	if ((filename==0) || (filename[0]==0)){
		if (module){
			module->getdisplayname(title,256);
		};
	}
	else{
		data = si->miniconfigcreate();
		if (oi->infoobject(data,filename,0)){
			data->getparameter("title",title,MTCT_STRING,256);
			bpm = 125.0;
			beats = 0.0;
			data->getparameter("bpm",&bpm,MTCT_FLOAT,sizeof(bpm));
			data->getparameter("beats",&beats,MTCT_FLOAT,sizeof(beats));
			*length_in_ms = 1000*60*beats/bpm;
		};
		si->miniconfigdelete(data);
	};
	LEAVE();
}

int infoDlg(char *fn,HWND hwnd)
{
	return 0;
}

void eq_set(int on,char data[10],int preamp) 
{

}
#else
//---------------------------------------------------------------------------
// XMMS only
//---------------------------------------------------------------------------
void config()
{

}

void about()
{

}

void play(char *fn)
{
	char title[256];

	FENTER1("play(%s)",fn);
	if (strcmp(lastfile,fn)){
		strcpy(lastfile,fn);
		if (module){
			if (playerthread) playerthread->terminate();
			oi->deleteobject(module);
		};
		module = (MTModule*)oi->newobject(MTO_MODULE,0,0,0,true);
		if (!module){
			LOGD("%s - [in_mt3] Could not create module object!"NL);
			mod.set_info_text("Could not create module!");
			return;
		};
		if (!oi->loadobject(module,lastfile,0)){
			oi->deleteobject(module);
			module = 0;
		};
		module->lock(MTOL_LOCK,false);
	};
	if (!module){
		LOGD("%s - [in_mt3] No module!"NL);
		mod.set_info_text("Could not load module!");
		return;
	};
	module->getdisplayname(title,sizeof(title));
	mod.output->open_audio(FMT_S16_NE,44100,2);
	mod.set_info(title,1000*60*module->loope/module->playstatus.bpm,module->ntracks*1000,44100,2);
	if (!playerthread) playerthread = si->threadcreate(PlayerThread,true,true,module,MTT_HIGH);
	LEAVE();
}

void stop()
{
	ENTER("stop()");
	mod.output->close_audio();
	if (playerthread) playerthread->terminate();
	if (module) module->play(PLAY_STOP);
	LEAVE();
}

void seek(int time)
{
	if (!module) return;
	module->setpos(time*module->playstatus.bpm/60);
	mod.output->flush(time*1000);
}

void pause_unpause(short paused)
{
	mod.output->pause(paused);
}

int getoutputtime()
{
	return mod.output->output_time();
}

void getfileinfo(char *filename,char **title,int *length_in_ms)
{
	MTMiniConfig *data;
	double bpm,beats;

	FENTER1("getfileinfo(%s)",filename);
	if ((filename==0) || (filename[0]==0)){
		if (module){
			*title = (char*)malloc(256);
			module->getdisplayname(*title,256);
		};
	}
	else{
		data = si->miniconfigcreate();
		if (oi->infoobject(data,filename,0)){
			*title = (char*)malloc(256);
			data->getparameter("title",*title,MTCT_STRING,256);
			bpm = 125.0;
			beats = 0.0;
			data->getparameter("bpm",&bpm,MTCT_FLOAT,sizeof(bpm));
			data->getparameter("beats",&beats,MTCT_FLOAT,sizeof(beats));
			*length_in_ms = 1000*60*beats/bpm;
		};
		si->miniconfigdelete(data);
	};
	LEAVE();
}

void file_info_box(char *filename)
{

}
#endif
//---------------------------------------------------------------------------
extern "C"
{
#ifdef _WIN32
__declspec(dllexport) In_Module* winampGetInModule2()
{
	return &mod;
}
#else
InputPlugin *get_iplugin_info(void)
{
	return &mod;
}
#endif
}
//---------------------------------------------------------------------------
#ifdef _WIN32
In_Module mod = 
{
	IN_VER,
	"MadTracker 2.5 Player",
	0,
	0,
	"mt2\0MadTracker 2 Module\0",
	1,
	1,
	config,
	about,
	init,
	quit,
	getfileinfo,
	infoDlg,
	isourfile,
	play,
	pause,
	unpause,
	ispaused,
	stop,
	getlength,
	getoutputtime,
	seek,
	setvolume,
	setpan,
	0,0,0,0,0,0,0,0,0,
	0,0,
	eq_set,
	0,
	0,
};
#else
InputPlugin mod = 
{
	0,
	0,
	"MadTracker 2.5 Player",
	init,
	about,
	config,
	isourfile,
	0,
	play,
	stop,
	pause_unpause,
	seek,
	0,
	getoutputtime,
	0,
	0,
	quit,
	0,
	0,
	0,
	0,
	getfileinfo,
	file_info_box,
	0,
};
#endif
//---------------------------------------------------------------------------
