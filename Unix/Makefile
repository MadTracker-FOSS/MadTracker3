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
# TODO: Yeah, let's assume 32-bit for now.
CFLAGS = -O2 -Wno-write-strings -m32

# Dynamic Libraries
GTK2LIB = `pkg-config --libs gtk+-2.0`
XML2LIB = `pkg-config --libs libxml-2.0`

# Includes
INCLUDES = -I..
INCLUDES += -I../Headers
INCLUDES += -I../MT3
INCLUDES += -I../MTAI
INCLUDES += -I../MTAudio
INCLUDES += -I../MTDSP
INCLUDES += -I../MTDisplay
INCLUDES += -I../MTGUI
INCLUDES += -I../MTObjects
INCLUDES += -I../MTSystem
INCLUDES += `pkg-config --cflags gtk+-2.0 libxml-2.0`

# Defines
DEFINES = -DMTSYSTEM_EXPORTS

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
	$(MT3SRCDIR)/MTConsole.cpp \
	$(MT3SRCDIR)/MTInterface.cpp \
	$(MT3SRCDIR)/MTWC_Main.cpp \
	$(MT3SRCDIR)/MTWC_Main2.cpp \
	$(MT3SRCDIR)/MTData.cpp \
	$(MT3SRCDIR)/MTExtensions.cpp \
	$(MT3SRCDIR)/MT3.cpp
MTAISRC = \
	$(MTAISRCDIR)/MTAI1.cpp \
	$(MTAISRCDIR)/MTAI.cpp
MTAUDIOSRC = \
	$(MTAUDIOSRCDIR)/MTAudio1.cpp \
	$(MTAUDIOSRCDIR)/MTAudio.cpp \
	$(MTAUDIOSRCDIR)/MTAudio2.cpp \
	$(MTAUDIOSRCDIR)/MTDirectSound.cpp \
	$(MTAUDIOSRCDIR)/MTDevDSP.cpp \
	$(MTAUDIOSRCDIR)/MTWaveOut.cpp
MTDSPSRC = \
	$(MTDSPSRCDIR)/MTCatmullASM.cpp \
	$(MTDSPSRCDIR)/MTBufferASM.cpp \
	$(MTDSPSRCDIR)/MTResamplingASM.cpp \
	$(MTDSPSRCDIR)/MTDSP.cpp \
	$(MTDSPSRCDIR)/MTFilterASM.cpp \
	$(MTDSPSRCDIR)/MTDSP1.cpp \
	$(MTDSPSRCDIR)/MTShaper.cpp
MTDISPLAYSRC = \
	$(MTDISPLAYSRCDIR)/MTDXBitmap.cpp \
	$(MTDISPLAYSRCDIR)/MTDXDevice.cpp \
	$(MTDISPLAYSRCDIR)/MTDisplayMMX.cpp \
	$(MTDISPLAYSRCDIR)/MTDecode.cpp \
	$(MTDISPLAYSRCDIR)/MTGDIDevice.cpp \
	$(MTDISPLAYSRCDIR)/MTDisplayASM.cpp \
	$(MTDISPLAYSRCDIR)/MTGDIBitmap.cpp \
	$(MTDISPLAYSRCDIR)/MTDisplayDevice.cpp \
	$(MTDISPLAYSRCDIR)/MTDisplay.cpp \
	$(MTDISPLAYSRCDIR)/MTDisplay1.cpp
MTGUISRC = \
	$(MTGUISRCDIR)/MTOSSkin.cpp \
	$(MTGUISRCDIR)/MTItems.cpp \
	$(MTGUISRCDIR)/MTControl.cpp \
	$(MTGUISRCDIR)/MTGUITools.cpp \
	$(MTGUISRCDIR)/MTButton.cpp \
	$(MTGUISRCDIR)/MTSkin.cpp \
	$(MTGUISRCDIR)/MTGUI.cpp \
	$(MTGUISRCDIR)/MTLabel.cpp \
	$(MTGUISRCDIR)/MTVisual.cpp \
	$(MTGUISRCDIR)/MTEdit.cpp \
	$(MTGUISRCDIR)/MTGUI1.cpp \
	$(MTGUISRCDIR)/MTWinControl.cpp \
	$(MTGUISRCDIR)/MTSign.cpp \
	$(MTGUISRCDIR)/MTWindow.cpp \
	$(MTGUISRCDIR)/MTTabControl.cpp \
	$(MTGUISRCDIR)/MTSysControls.cpp \
	$(MTGUISRCDIR)/MTCustControls.cpp \
	$(MTGUISRCDIR)/MTCheckBox.cpp \
	$(MTGUISRCDIR)/MTDesktop.cpp \
	$(MTGUISRCDIR)/MTOSWindow.cpp \
	$(MTGUISRCDIR)/MTProgress.cpp \
	$(MTGUISRCDIR)/MTToolBar.cpp \
	$(MTGUISRCDIR)/MTPanel.cpp
MTOBJECTSSRC = \
	$(MTOBJECTSSRCDIR)/MTTest.cpp \
	$(MTOBJECTSSRCDIR)/MTInstrument.cpp \
	$(MTOBJECTSSRCDIR)/MTObjectsASM.cpp \
	$(MTOBJECTSSRCDIR)/MTIOInstrument.cpp \
	$(MTOBJECTSSRCDIR)/MTObjects2.cpp \
	$(MTOBJECTSSRCDIR)/MTGenerator.cpp \
	$(MTOBJECTSSRCDIR)/MTXMCommands.cpp \
	$(MTOBJECTSSRCDIR)/MTFilter.cpp \
	$(MTOBJECTSSRCDIR)/MTOscillator.cpp \
	$(MTOBJECTSSRCDIR)/MTEffect.cpp \
	$(MTOBJECTSSRCDIR)/MTObjects1.cpp \
	$(MTOBJECTSSRCDIR)/MTIOModule_MT2.cpp \
	$(MTOBJECTSSRCDIR)/MTGraphs.cpp \
	$(MTOBJECTSSRCDIR)/MTModule.cpp \
	$(MTOBJECTSSRCDIR)/MTSeqManager.cpp \
	$(MTOBJECTSSRCDIR)/MTAutomation.cpp \
	$(MTOBJECTSSRCDIR)/MTPattern.cpp \
	$(MTOBJECTSSRCDIR)/MTDelay.cpp \
	$(MTOBJECTSSRCDIR)/MTTrack.cpp \
	$(MTOBJECTSSRCDIR)/MTIOOscillator.cpp \
	$(MTOBJECTSSRCDIR)/MTColumns.cpp \
	$(MTOBJECTSSRCDIR)/MTObject.cpp \
	$(MTOBJECTSSRCDIR)/MTInstrEditor.cpp \
	$(MTOBJECTSSRCDIR)/MTPattEditor.cpp \
	$(MTOBJECTSSRCDIR)/MTObjects.cpp \
	$(MTOBJECTSSRCDIR)/MTPattManager.cpp
MTSYSTEMSRC = \
	$(MTSYSTEMSRCDIR)/MTStructures.cpp \
	$(MTSYSTEMSRCDIR)/MTKernel.cpp \
	$(MTSYSTEMSRCDIR)/MTFile.cpp \
	$(MTSYSTEMSRCDIR)/MTXML.cpp \
	$(MTSYSTEMSRCDIR)/MTConfig.cpp \
	$(MTSYSTEMSRCDIR)/MTSystem.cpp \
	$(MTSYSTEMSRCDIR)/MTSocket.cpp \
	$(MTSYSTEMSRCDIR)/MTInternet.cpp \
	$(MTSYSTEMSRCDIR)/strnatcmp.cpp \
	$(MTSYSTEMSRCDIR)/MTLocalFile.cpp \
	$(MTSYSTEMSRCDIR)/MTMemoryFile.cpp \
	$(MTSYSTEMSRCDIR)/MTHTTPFile.cpp \
	$(MTSYSTEMSRCDIR)/MTBase64.cpp \
	$(MTSYSTEMSRCDIR)/MTMiniConfig.cpp \
	$(MTSYSTEMSRCDIR)/MTResources.cpp \
	$(MTSYSTEMSRCDIR)/MTSystem1.cpp \
	$(MTSYSTEMSRCDIR)/MTMD5.cpp

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

all: mt3 audio dsp display gui objects system

mt3: $(MT3OBJ)
	$(CXX) $(CFLAGS) -ldl -o $(TARGET)/madtracker.$(TARGET) $(MT3OBJ)

ai: $(MTAIOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtai.$(LIBSUFFIX) $(MTAIOBJ)

audio: $(MTAUDIOOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtaudio.$(LIBSUFFIX) $(MTAUDIOOBJ)

dsp: $(MTDSPOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtdsp.$(LIBSUFFIX) $(MTDSPOBJ)

display: $(MTDISPLAYOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtdisplay.$(LIBSUFFIX) $(MTDISPLAYOBJ)

gui: $(MTGUIOBJ)
	$(CXX) $(CFLAGS) $(GTK2LIB) -fpic -fPIC -shared -o $(TARGET)/mtgui.$(LIBSUFFIX) $(MTGUIOBJ)

objects: $(MTOBJECTSOBJ)
	$(CXX) $(CFLAGS) -fpic -fPIC -shared -o $(TARGET)/mtobjects.$(LIBSUFFIX) $(MTOBJECTSOBJ)

system: $(MTSYSTEMOBJ)
	$(CXX) $(CFLAGS) $(XML2LIB) -fpic -fPIC -shared -o $(TARGET)/mtsystem.$(LIBSUFFIX) $(MTSYSTEMOBJ)

$(MT3SRCDIR)/%.o: $(MT3SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< $(INCLUDES) $(DEFINES)

%.o: %.cpp
	$(CXX) $(CFLAGS) -fPIC -c -o $@ $< $(INCLUDES) $(DEFINES)

clean:
	rm -f $(MT3OBJ) $(MTAIOBJ) $(MTAUDIOOBJ) $(MTDSPOBJ) $(MTDISPLAYOBJ) $(MTGUIOBJ) $(MTOBJECTSOBJ) $(MTSYSTEMOBJ) $(TARGET)/madtracker.$(TARGET) $(TARGET)/mt*.$(LIBSUFFIX)
