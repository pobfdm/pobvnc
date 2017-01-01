#!/bin/bash
#Run this script from another directory (ex: Desktop/ )

VER=0.2
URL=https://github.com/pobfdm/pobvnc/archive/master.zip

if [ ! -f $(basename $URL) ]
then
	wget $URL
	unzip $(basename $URL)
	mv pobvnc-master pobvnc-$VER  
fi

cd pobvnc-$VER
cmake .
make
dh_make -n
cp pkgs/Debian/control  debian/
dpkg-buildpackage





