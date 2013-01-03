# Makefile for MadTracker 3
# Written by Ethan "flibitijibibo" Lee

# System information
UNAME = $(shell uname)
ARCH = $(shell uname -m)

# Compiler, target/file names
ifeq ($(UNAME), Darwin)
	CXX = clang++
	TARGET = osx
	LIBSUFFIX = dylib
endif
ifeq ($(UNAME), Linux)
	CXX = g++
	TARGET = x86
	LIBSUFFIX = so
endif
ifeq ($(UNAME), MinGW*)
	CXX = g++
	TARGET = exe
	LIBSUFFIX = dll
endif

# We are assuming 32-bit until a 64-bit port is made.
#ifeq ($(UNAME), Linux)
#	CXX = g++
#	ifeq ($(ARCH), x86_64)
#		TARGET = x86_64
#	else
#		TARGET = x86
#	endif
#	LIBSUFFIX = so
#endif

# Compiler flags
CFLAGS = -g -Wno-write-strings -m32

# Includes/Defines
INCLUDES = -I.. -I../include

# Dependencies
ifeq ($(UNAME), Darwin)
	# TODO:
endif
ifeq ($(UNAME), Linux)
	SDLINC = $(shell sdl-config --cflags)
	SDLLIB = $(shell sdl-config --libs) -lSDL_image
	GTKINC = $(shell pkg-config --cflags gtk+-2.0)
	GTKLIB = $(shell pkg-config --libs gtk+-2.0)
endif

# Source directories
MT3SRCDIR = ../MT3
MTAISRCDIR = ../MTAI
MTAUDIOSRCDIR = ../MTAudio
MTDSPSRCDIR = ../MTDSP
MTDISPLAYSRCDIR = ../MTDisplay
MTGUISRCDIR = ../MTGUI
MTOBJECTSSRCDIR = ../MTObjects
MTSYSTEMSRCDIR = ../MTSystem

# Source lists
MT3SRC = \
	$(MT3SRCDIR)/MT3.cpp \
	$(MT3SRCDIR)/MTConsole.cpp \
	$(MT3SRCDIR)/MTData.cpp \
	$(MT3SRCDIR)/MTExtensions.cpp \
	$(MT3SRCDIR)/MTInterface.cpp \
	$(MT3SRCDIR)/MTWC_Main.cpp \
	$(MT3SRCDIR)/MTWC_Main2.cpp
MTAISRC = \
	$(MTAISRCDIR)/MTAI_main.cpp \
	$(MTAISRCDIR)/MTAI.cpp
MTAUDIOSRC = \
	$(MTAUDIOSRCDIR)/MTAudio.cpp \
	$(MTAUDIOSRCDIR)/MTAudio1.cpp \
	$(MTAUDIOSRCDIR)/MTAudio2.cpp \
	$(MTAUDIOSRCDIR)/MTDevDSP.cpp \
	$(MTAUDIOSRCDIR)/MTDirectSound.cpp \
	$(MTAUDIOSRCDIR)/MTWaveOut.cpp
MTDSPSRC = \
	$(MTDSPSRCDIR)/MTDSP_main.cpp \
	$(MTDSPSRCDIR)/MTBufferASM.cpp \
	$(MTDSPSRCDIR)/MTCatmullASM.cpp \
	$(MTDSPSRCDIR)/MTDSP.cpp \
	$(MTDSPSRCDIR)/MTFilterASM.cpp \
	$(MTDSPSRCDIR)/MTResamplingASM.cpp \
	$(MTDSPSRCDIR)/MTShaper.cpp
MTDISPLAYSRC = \
	$(MTDISPLAYSRCDIR)/MTDisplay_main.cpp \
	$(MTDISPLAYSRCDIR)/MTDisplay.cpp \
	$(MTDISPLAYSRCDIR)/MTDisplayDevice.cpp \
	$(MTDISPLAYSRCDIR)/MTSDLBitmap.cpp \
	$(MTDISPLAYSRCDIR)/MTSDLDevice.cpp
MTGUISRC = \
	$(MTGUISRCDIR)/MTButton.cpp \
	$(MTGUISRCDIR)/MTCheckBox.cpp \
	$(MTGUISRCDIR)/MTControl.cpp \
	$(MTGUISRCDIR)/MTCustControls.cpp \
	$(MTGUISRCDIR)/MTDesktop.cpp \
	$(MTGUISRCDIR)/MTEdit.cpp \
	$(MTGUISRCDIR)/MTGUI.cpp \
	$(MTGUISRCDIR)/MTGUI1.cpp \
	$(MTGUISRCDIR)/MTGUITools.cpp \
	$(MTGUISRCDIR)/MTItems.cpp \
	$(MTGUISRCDIR)/MTLabel.cpp \
	$(MTGUISRCDIR)/MTOSSkin.cpp \
	$(MTGUISRCDIR)/MTOSWindow.cpp \
	$(MTGUISRCDIR)/MTPanel.cpp \
	$(MTGUISRCDIR)/MTProgress.cpp \
	$(MTGUISRCDIR)/MTSign.cpp \
	$(MTGUISRCDIR)/MTSkin.cpp \
	$(MTGUISRCDIR)/MTSysControls.cpp \
	$(MTGUISRCDIR)/MTTabControl.cpp \
	$(MTGUISRCDIR)/MTToolBar.cpp \
	$(MTGUISRCDIR)/MTVisual.cpp \
	$(MTGUISRCDIR)/MTWinControl.cpp \
	$(MTGUISRCDIR)/MTWindow.cpp
MTOBJECTSSRC = \
	$(MTOBJECTSSRCDIR)/MTAutomation.cpp \
	$(MTOBJECTSSRCDIR)/MTColumns.cpp \
	$(MTOBJECTSSRCDIR)/MTDelay.cpp \
	$(MTOBJECTSSRCDIR)/MTEffect.cpp \
	$(MTOBJECTSSRCDIR)/MTFilter.cpp \
	$(MTOBJECTSSRCDIR)/MTGenerator.cpp \
	$(MTOBJECTSSRCDIR)/MTGraphs.cpp \
	$(MTOBJECTSSRCDIR)/MTIOInstrument.cpp \
	$(MTOBJECTSSRCDIR)/MTIOModule_MT2.cpp \
	$(MTOBJECTSSRCDIR)/MTIOOscillator.cpp \
	$(MTOBJECTSSRCDIR)/MTInstrEditor.cpp \
	$(MTOBJECTSSRCDIR)/MTInstrument.cpp \
	$(MTOBJECTSSRCDIR)/MTModule.cpp \
	$(MTOBJECTSSRCDIR)/MTObject.cpp \
	$(MTOBJECTSSRCDIR)/MTObjects.cpp \
	$(MTOBJECTSSRCDIR)/MTObjects1.cpp \
	$(MTOBJECTSSRCDIR)/MTObjects2.cpp \
	$(MTOBJECTSSRCDIR)/MTObjectsASM.cpp \
	$(MTOBJECTSSRCDIR)/MTOscillator.cpp \
	$(MTOBJECTSSRCDIR)/MTPattEditor.cpp \
	$(MTOBJECTSSRCDIR)/MTPattManager.cpp \
	$(MTOBJECTSSRCDIR)/MTPattern.cpp \
	$(MTOBJECTSSRCDIR)/MTSeqManager.cpp \
	$(MTOBJECTSSRCDIR)/MTTest.cpp \
	$(MTOBJECTSSRCDIR)/MTTrack.cpp \
	$(MTOBJECTSSRCDIR)/MTXMCommands.cpp
MTSYSTEMSRC = \
	$(MTSYSTEMSRCDIR)/MTSystem_main.cpp \
	$(MTSYSTEMSRCDIR)/MTBase64.cpp \
	$(MTSYSTEMSRCDIR)/MTConfig.cpp \
	$(MTSYSTEMSRCDIR)/MTFile.cpp \
	$(MTSYSTEMSRCDIR)/MTHTTPFile.cpp \
	$(MTSYSTEMSRCDIR)/MTInternet.cpp \
	$(MTSYSTEMSRCDIR)/MTKernel.cpp \
	$(MTSYSTEMSRCDIR)/MTLocalFile.cpp \
	$(MTSYSTEMSRCDIR)/MTMD5.cpp \
	$(MTSYSTEMSRCDIR)/MTMemoryFile.cpp \
	$(MTSYSTEMSRCDIR)/MTMiniConfig.cpp \
	$(MTSYSTEMSRCDIR)/MTResources.cpp \
	$(MTSYSTEMSRCDIR)/MTSocket.cpp \
	$(MTSYSTEMSRCDIR)/MTStructures.cpp \
	$(MTSYSTEMSRCDIR)/MTSystem.cpp \
	$(MTSYSTEMSRCDIR)/strnatcmp.cpp

# Object code lists
MT3OBJ = $(MT3SRC:%.cpp=%.o)
MTAIOBJ = $(MTAISRC:%.cpp=%.o)
MTAUDIOOBJ = $(MTAUDIOSRC:%.cpp=%.o)
MTDSPOBJ = $(MTDSPSRC:%.cpp=%.o)
MTDISPLAYOBJ = $(MTDISPLAYSRC:%.cpp=%.o)
MTGUIOBJ = $(MTGUISRC:%.cpp=%.o)
MTOBJECTSOBJ = $(MTOBJECTSSRC:%.cpp=%.o)
MTSYSTEMOBJ = $(MTSYSTEMSRC:%.cpp=%.o)

# Targets

all: mt3 ai audio dsp display gui objects system

mt3: $(MT3OBJ)
	$(CXX) $(CFLAGS) -ldl -o $(TARGET)/madtracker.$(TARGET) $(MT3OBJ)

ai: $(MTAIOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtai.$(LIBSUFFIX) $(MTAIOBJ)

audio: $(MTAUDIOOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtaudio.$(LIBSUFFIX) $(MTAUDIOOBJ)

dsp: $(MTDSPOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtdsp.$(LIBSUFFIX) $(MTDSPOBJ)

display: $(MTDISPLAYOBJ)
	$(CXX) $(CFLAGS) $(SDLLIB) -fpic -fPIC -shared -o $(TARGET)/mtdisplay.$(LIBSUFFIX) $(MTDISPLAYOBJ)

gui: $(MTGUIOBJ)
	$(CXX) $(CFLAGS) $(GTKLIB) -fpic -fPIC -shared -o $(TARGET)/mtgui.$(LIBSUFFIX) $(MTGUIOBJ)

objects: $(MTOBJECTSOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtobjects.$(LIBSUFFIX) $(MTOBJECTSOBJ)

system: $(MTSYSTEMOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtsystem.$(LIBSUFFIX) $(MTSYSTEMOBJ)

$(MT3SRCDIR)/%.o: $(MT3SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< $(INCLUDES) $(DEFINES)

$(MTDISPLAYSRCDIR)/%.o: $(MTDISPLAYSRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $(SDLINC) -c -o $@ $< $(INCLUDES) $(DEFINES)

$(MTGUISRCDIR)/%.o: $(MTGUISRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $(GTKINC) -c -o $@ $< $(INCLUDES) $(DEFINES)

%.o: %.cpp
	$(CXX) $(CFLAGS) -fPIC -c -o $@ $< $(INCLUDES) $(DEFINES)

clean:
	rm -f $(MT3OBJ) $(MTAIOBJ) $(MTAUDIOOBJ) $(MTDSPOBJ) $(MTDISPLAYOBJ) $(MTGUIOBJ) $(MTOBJECTSOBJ) $(MTSYSTEMOBJ) $(TARGET)/madtracker.$(TARGET) $(TARGET)/mt*.$(LIBSUFFIX)
