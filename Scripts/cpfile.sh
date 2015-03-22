#!/bin/bash

#------------------------------------------------
SOURCEDIR=/data/15testdata/BT/RAWData_Bin/
TARGETDIR=$1/DAMPE_HEX/

printf "copy hex data from:\n"
printf "%s\n" ${SOURCEDIR}
printf "to:\n"
printf "%s\n" ${TARGETDIR}

rsync -rtuv --delete ${SOURCEDIR} ${TARGETDIR}

#------------------------------------------------
SOURCEDIR=/home/zangjj/bt2015/run/ANC/
TARGETDIR=$1/ANC/

printf "copy ancillary data from:\n"
printf "%s\n" ${SOURCEDIR}
printf "to:\n"
printf "%s\n" ${TARGETDIR}

rsync -rtuv --delete ${SOURCEDIR} ${TARGETDIR}
#------------------------------------------------
SOURCEDIR=/home/zangjj/bt2015/run/AMS/
TARGETDIR=$1/AMS/

printf "copy ams data from:\n"
printf "%s\n" ${SOURCEDIR}
printf "to:\n"
printf "%s\n" ${TARGETDIR}

rsync -rtuv --delete ${SOURCEDIR} ${TARGETDIR}