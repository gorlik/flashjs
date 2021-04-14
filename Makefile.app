VPATH=..

CXXFLAGS=-Wall -DUSE_WX $(INCLUDE)
CFLAGS=$(INCLUDE)

OBJECTS_CLI = flashjs_cli.o

#OBJECTS_GUI = flashjs_gui.o

LIB_FJS = flashjs_lib.o

OBJECTS=$(LIB_FJS) $(OBJECTS_CLI)
#$(OBJECTS_DUMP) $(OBJECTS_FAN) $(OBJECTS_GUI)

LIBS+=$(LIB_FJS)
LIBS+=-lusb-1.0

ifeq ($(BUILD),WIN32)
AR=i586-mingw32msvc-ar
CC=i586-mingw32msvc-gcc
CXX=i586-mingw32msvc-g++

WXCONFIG=i586-msvc-wx-config
#--unicode=no
BF=-win32

CFLAGS+=-I/usr/local/i586-mingw32msvc/include
CXXFLAGS+=-I/usr/local/i586-mingw32msvc/include -fno-rtti -fno-exceptions
LIBS+=-L/usr/local/i586-mingw32msvc/lib -lsetupapi -lole32 -ladvapi32 

#-lhid

else
WXCONFIG=wx-config
endif

#VPATH=..  ../../ggtest/utils/

ifeq ($(DEBUG),YES)
        CXXFLAGS+= `$(WXCONFIG) --cxxflags --debug=yes` -ggdb
	WXLIBS= `$(WXCONFIG) --libs --debug=yes`
else
        CXXFLAGS+= `$(WXCONFIG) --cxxflags`
	WXLIBS= `$(WXCONFIG) --libs`
endif

#CXXFLAGS+= -I../../common

$(APP): $(OBJECTS)
	$(CC) -o $(APP)_cli $(OBJECTS_CLI) $(LIBS)
#	$(CXX) -o $(APP)_gui $(OBJECTS_GUI) $(LIBS) $(WXLIBS)

clean:
	rm -f *.o $(NAME) core.*
