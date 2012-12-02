# Makefile for MadTracker 3
# Written by Ethan "flibitijibibo" Lee

# General TODO Stuff:
# - ASM files! Oh noes!
# - Non-cpp/h files! Oh noes?
# - 64-bit support.

# System information
UNAME = $(shell uname)
ARCH = $(shell uname -m)

# Compiler
ifeq ($(UNAME), Darwin)
	CXX = clang++
else
	CXX = g++
endif

# Compiler flags
CFLAGS = -O2 -Wno-write-strings -m32 # Yeah, let's assume 32-bit for now.

# Dynamic Libraries
# TODO
DEPENDENCIES = -lFLAC `pkg-config --libs gtk+-2.0 libxml2`

# Includes
INCLUDES = -I..
INCLUDES += -I../Headers
INCLUDES += -I../in_mt3
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
# TODO
DEFINES = -DFLIBIT_RULES

# Executable name
ifeq ($(UNAME), Darwin)
	EXECUTABLE = osx/madtracker.osx
endif
ifeq ($(UNAME), Linux)
	EXECUTABLE = x86/madtracker.x86
endif
# We are assuming 32-bit until a 64-bit port is made.
#ifeq ($(UNAME), Linux)
#	ifeq ($(ARCH), x86_64)
#		EXECUTABLE = x86_64/madtracker.x86_64
#	else
#		EXECUTABLE = x86/madtracker.x86
#	endif
#endif

# Source directories
HEADERSRCDIR = ../Headers
INMT3SRCDIR = ../in_mt3
MT3SRCDIR = ../MT3
MTAISRCDIR = ../MTAI
MTAUDIOSRCDIR = ../MTAudio
MTDSPSRCDIR = ../MTDSP
MTDISPLAYSRCDIR = ../MTDisplay
MTGUISRCDIR = ../MTGUI
MTOBJECTSRCDIR = ../MTObjects
MTSYSTEMSRCDIR = ../MTSystem

# Source lists
HEADERSRC = \
	$(HEADERSRCDIR)/MTXSystem2.cpp \
	$(HEADERSRCDIR)/MTXEffect.cpp
INMT3SRC = \
	$(INMT3SRCDIR)/MTWinamp1.cpp \
	$(INMT3SRCDIR)/MTPluginInterface.cpp \
	$(INMT3SRCDIR)/in_mt3.cpp
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
MTOBJECTSRC = \
	$(MTOBJECTSRCDIR)/MTTest.cpp \
	$(MTOBJECTSRCDIR)/MTInstrument.cpp \
	$(MTOBJECTSRCDIR)/MTObjectsASM.cpp \
	$(MTOBJECTSRCDIR)/MTIOInstrument.cpp \
	$(MTOBJECTSRCDIR)/MTObjects2.cpp \
	$(MTOBJECTSRCDIR)/MTGenerator.cpp \
	$(MTOBJECTSRCDIR)/MTXMCommands.cpp \
	$(MTOBJECTSRCDIR)/MTFilter.cpp \
	$(MTOBJECTSRCDIR)/MTOscillator.cpp \
	$(MTOBJECTSRCDIR)/MTEffect.cpp \
	$(MTOBJECTSRCDIR)/MTObjects1.cpp \
	$(MTOBJECTSRCDIR)/MTIOModule_MT2.cpp \
	$(MTOBJECTSRCDIR)/MTGraphs.cpp \
	$(MTOBJECTSRCDIR)/MTModule.cpp \
	$(MTOBJECTSRCDIR)/MTSeqManager.cpp \
	$(MTOBJECTSRCDIR)/MTAutomation.cpp \
	$(MTOBJECTSRCDIR)/MTPattern.cpp \
	$(MTOBJECTSRCDIR)/MTDelay.cpp \
	$(MTOBJECTSRCDIR)/MTTrack.cpp \
	$(MTOBJECTSRCDIR)/MTIOOscillator.cpp \
	$(MTOBJECTSRCDIR)/MTColumns.cpp \
	$(MTOBJECTSRCDIR)/MTObject.cpp \
	$(MTOBJECTSRCDIR)/MTInstrEditor.cpp \
	$(MTOBJECTSRCDIR)/MTPattEditor.cpp \
	$(MTOBJECTSRCDIR)/MTObjects.cpp \
	$(MTOBJECTSRCDIR)/MTPattManager.cpp
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
HEADEROBJ = $(HEADERSRC:%.cpp=%.o)
INMT3OBJ = $(INMT3SRC:%.cpp=%.o)
MT3OBJ = $(MT3SRC:%.cpp=%.o)
MTAIOBJ = $(MTAISRC:%.cpp=%.o)
MTAUDIOOBJ = $(MTAUDIOSRC:%.cpp=%.o)
MTDSPOBJ = $(MTDSPSRC:%.cpp=%.o)
MTDISPLAYOBJ = $(MTDISPLAYSRC:%.cpp=%.o)
MTGUIOBJ = $(MTGUISRC:%.cpp=%.o)
MTOBJECTOBJ = $(MTOBJECTSRC:%.cpp=%.o)
MTSYSTEMOBJ = $(MTSYSTEMSRC:%.cpp=%.o)

# Targets

all: $(HEADEROBJ) $(INMT3OBJ) $(MT3OBJ) $(MTAIOBJ) $(MTAUDIOOBJ) $(MTDSPOBJ) $(MTDISPLAYOBJ) $(MTGUIOBJ) $(MTOBJECTOBJ) $(MTSYSTEMOBJ)
	$(CXX) $(CFLAGS) $(HEADEROBJ) $(INMT3OBJ) $(MT3OBJ) $(MTAIOBJ) $(MTOBJECTOBJ) $(MTSYSTEMOBJ) -o $(EXECUTABLE) $(DEPENDENCIES)

headers: $(HEADEROBJ)
in_mt3: $(INMT3OBJ)
mt3: $(MT3OBJ)
ai: $(MTAIOBJ)
audio: $(MTAUDIOOBJ)
dsp: $(MTDSPOBJ)
display: $(MTDISPLAYOBJ)
gui: $(MTGUIOBJ)
object: $(MTOBJECTOBJ)
system: $(MTSYSTEMOBJ)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< $(INCLUDES) $(DEFINES)

clean:
	rm -f $(HEADEROBJ) $(INMT3OBJ) $(MT3OBJ) $(MTAIOBJ) $(MTAUDIOOBJ) $(MTDSPOBJ) $(MTDISPLAYOBJ) $(MTGUIOBJ) $(MTOBJECTOBJ) $(MTSYSTEMOBJ) $(EXECUTABLE)
