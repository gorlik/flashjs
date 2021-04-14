#js16 flasher host app makefile

SOURCES=*.c 
#*.cpp
NAME=flashjs


linux: $(SOURCES)
	mkdir -p linux
	( cd linux; $(MAKE) -f../Makefile.app APP=$(NAME))
	mv linux/$(NAME)_cli $(NAME)
#	mv linux/$(NAME)_gui $(NAME)_gui

linux_debug:
	mkdir -p linuxd
	( cd linuxd; $(MAKE) -f../Makefile.app APP=$(NAME) DEBUG=YES)
	mv linuxd/$(NAME)_cli $(NAME)_clid
#	mv linux/$(NAME)_gui $(NAME)_guid

win:
	mkdir -p win32
	( cd win32; $(MAKE) -f../Makefile.app APP=$(NAME) BUILD=WIN32)
	mv win32/$(NAME)_cli $(NAME).exe
#	mv win32/$(NAME)_gui $(NAME)_gui.exe
	strip *.exe

dbg: linux_debug

all: linux win

clean:
	rm -rf *.a core.* \
	*.exe flashjs_gui flashjsd_gui flashjs_cli flashjs \
	linux linuxd win32 \
	*~
