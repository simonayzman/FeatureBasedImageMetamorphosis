# the name of the compiled binary

T1 := featureAdder
T2 := imageMorph
T3 := frameExtractor
T4 := autoFeatures
T5 := combiner

T1MAKENAME := T1
T2MAKENAME := T2
T3MAKENAME := T3
T4MAKENAME := T4
T5MAKENAME := T5

# list files to compile and link together
FILES1   := featureAdder SimpleImage util
FILES2   := imageMorph SimpleImage SimpleVideo util
FILES3   := frameExtractor SimpleVideo SimpleImage
FILES4   := autoFeatures util
FILES5   := combiner SimpleVideo SimpleImage

#################################################################


# The following Makefile rules should work for Linux or Cygwin
CC				 := g++
LD		 		 := g++
OBJSUFFIX	   	 := .o
LIBPREFIX	 	 := lib
STATIC_LIBSUFFIX := .a
CFLAGS 		 	 := -g -std=c++11 -stdlib=libc++
CFLAGS_PLATFORM  := 
LDFLAGS		 	 :=
WFLAGS			 := -Wno-deprecated
FRAMEWORKS	 	 := 
LIBS		 	 := png

ARCH=$(shell uname | sed -e 's/-.*//g')

ifeq ($(ARCH), CYGWIN_NT)

# if we're building in cygwin, we'll need to use the
# win32 versions of gl and glut
EXESUFFIX  := .exe
LIBS  	   += glut32 opengl32
CFLAGS_PLATFORM += 

else

ifeq ($(ARCH),Darwin)

# we're building on the mac

EXESUFFIX  :=
FRAMEWORKS += OpenGL GLUT 
CFLAGS_PLATFORM += 

LIBS_FFMPEG := m z pthread avformat avcodec avutil
LIBS_OPENCV := opencv_core opencv_highgui opencv_imgproc opencv_video opencv_objdetect
LIBS_OTHER  := pthread dl m

LIBS += $(LIBS_OTHER) $(LIBS_FFMPEG) $(LIBS_OPENCV)

#
# Mac users need to point to these directories
EXTRA_LIB_DIRS := /usr/local/lib /opt/local/lib
EXTRA_INC_DIRS := /usr/local/include /opt/local/include

# 
# The default location of a fink install is given below.
# 
#EXTRA_LIB_DIRS := /sw/lib /opt/local/lib
#EXTRA_INC_DIRS  := /sw/include /opt/local/include

else

# building on Linux

EXESUFFIX  :=
LIBS	   += glut GL GLU
CFLAGS_PLATFORM += 

#
# hack for myth machines.  Add /usr/lib as an explicit lib dir so
# it gets picked up instead of /usr/pubsw/lib.
# 

EXTRA_LIB_DIRS := /usr/lib
EXTRA_INC_DIRS :=

endif
endif


TARGET     := $(addsuffix $(EXESUFFIX), $(TARGET))

INCDIRS    := .  $(EXTRA_INC_DIRS)
LIBDIRS    := $(EXTRA_LIB_DIRS)

CFLAGS     += $(addprefix -I, $(INCDIRS))
CFLAGS	   += $(CFLAGS_PLATFORM)
LDFLAGS    += $(addprefix -L, $(LIBDIRS))

LDLIBS	   := $(addprefix -l, $(LIBS))
LDFRAMEWORKS := $(addprefix -framework , $(FRAMEWORKS))

OBJS1       :=  $(addsuffix $(OBJSUFFIX), $(FILES1))
OBJS2       :=  $(addsuffix $(OBJSUFFIX), $(FILES2))
OBJS3       :=  $(addsuffix $(OBJSUFFIX), $(FILES3))
OBJS4       :=  $(addsuffix $(OBJSUFFIX), $(FILES4))
OBJS5       :=  $(addsuffix $(OBJSUFFIX), $(FILES5))

.SUFFIXES : .cpp $(OBJSUFFIX)

.PHONY : clean release all

all: $(T1MAKENAME) $(T2MAKENAME) $(T3MAKENAME) $(T4MAKENAME) $(T5MAKENAME)

$(T1MAKENAME): $(T1)

$(T1) : $(OBJS1)
	$(LD) -o $(T1) $(OBJS1) $(LDFLAGS) $(LDLIBS) $(LDFRAMEWORKS)

%.o : %.cpp
	$(CC) $(WFLAGS) $(CFLAGS) -o $@ -c $<

$(T2MAKENAME): $(T2)

$(T2) : $(OBJS2)
	$(LD) -o $(T2) $(OBJS2) $(LDFLAGS) $(LDLIBS) $(LDFRAMEWORKS)
	
%.o : %.cpp
	$(CC) $(WFLAGS) $(CFLAGS) -o $@ -c $<

$(T3MAKENAME): $(T3)

$(T3) : $(OBJS3)
	$(LD) -o $(T3) $(OBJS3) $(LDFLAGS) $(LDLIBS) $(LDFRAMEWORKS)

%.o : %.cpp
	$(CC) $(WFLAGS) $(CFLAGS) -o $@ -c $<

$(T4MAKENAME): $(T4)

$(T4) : $(OBJS4)
	$(LD) -o $(T4) $(OBJS4) $(LDFLAGS) $(LDLIBS) $(LDFRAMEWORKS)

%.o : %.cpp
	$(CC) $(WFLAGS) $(CFLAGS) -o $@ -c $<

$(T5MAKENAME): $(T5)

$(T5) : $(OBJS5)
	$(LD) -o $(T5) $(OBJS5) $(LDFLAGS) $(LDLIBS) $(LDFRAMEWORKS)

%.o : %.cpp
	$(CC) $(WFLAGS) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf *$(OBJSUFFIX) $(TARGET) *~ .#* #*
	rm -rf *$(OBJSUFFIX) $(T1) *~ .#* #*
	rm -rf *$(OBJSUFFIX) $(T2) *~ .#* #*
	rm -rf *$(OBJSUFFIX) $(T3) *~ .#* #*
	rm -rf *$(OBJSUFFIX) $(T4) *~ .#* #*
	rm -rf *$(OBJSUFFIX) $(T5) *~ .#* #*

release:
	@make --no-print-directory RELEASE=1


