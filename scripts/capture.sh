#!/bin/bash
cd /home/odroid/images/new
sh ~/develop/chdkptp/chdkptp.sh -c -e"rec" #Place the camera into record mode
sh ~/develop/chdkptp/chdkptp.sh -c -e"rs -shots=8000 -quick -sv=80 -av=2.8 -tv=1/2000" #take a bunch of shots and download them as we go
