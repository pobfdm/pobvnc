#!/bin/bash

#First execute "rpmdev-setuptree" and then put the script inside rpmbuild

wget https://github.com/pobfdm/pobvnc/archive/master.zip -O ./SOURCES/pobvnc-master.zip && cd SPECS/ && rpmbuild -ba pobvnc.spec
 
