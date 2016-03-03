#!/bin/sh

cd /tmp
ARCH=$(uname -m)


if [ "$ARCH" = "x86_64" ] 
then
	echo "ARCH: 64-bit"
	wget http://www.freemedialab.org/pobvnc/tigervnc/tigervnc-Linux-x86_64-1.6.0.tar.gz
	mkdir /opt/tigervnc
	tar xzf tigervnc-Linux-x86_64-1.6.0.tar.gz -C /opt/tigervnc
fi

if [ "$ARCH" = "x86" ] 
then
	echo "ARCH: 32-bit"
	wget http://www.freemedialab.org/pobvnc/tigervnc/tigervnc-Linux-i686-1.6.0.tar.gz
	mkdir /opt/tigervnc
	tar xzf tigervnc-Linux-x86_64-1.6.0.tar.gz -C /opt/tigervnc
fi

ln -s /opt/tigervnc/usr/bin/vncviewer /usr/local/bin/vncviewer
