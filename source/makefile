CPP = gcc
OPTS = `pkg-config --cflags --libs gtk+-2.0` 

CPPWIN32= i686-w64-mingw32.static-gcc  
OPTSWIN32 = -mwindows -mms-bitfields `i686-w64-mingw32.static-pkg-config --cflags --libs glib-2.0 gtk+-3.0`

resource:
	glib-compile-resources --generate-source linux64-resource.xml

msys2:
	glib-compile-resources --generate-source win32-resource.xml
	$(CPP) exeicon.o win32-resource.c callbacks.c utils.c main.c -o ../win32/pobvnc-i686-msys2.exe $(OPTS) -mwindows -mms-bitfields



linux32:
	glib-compile-resources --generate-source linux32-resource.xml
	$(CPP) linux32-resource.c callbacks.c utils.c main.c -o ../linux32/pobvnc-x86 $(OPTS)


linux64:
	glib-compile-resources --generate-source linux64-resource.xml
	$(CPP) linux64-resource.c callbacks.c utils.c main.c -o ../linux64/pobvnc-x86_64 $(OPTS)


win32:
	glib-compile-resources --generate-source win32-resource.xml
	$(CPPWIN32) exeicon.o win32-resource.c callbacks.c utils.c main.c -o ../win32/pobvnc-i686.exe $(OPTSWIN32)
	strip ../win32/pobvnc-i686.exe 

translation:
	msgfmt mo/it/pobvnc.po -o mo/it/pobvnc.mo


all:
	make linux64


clean:
	
