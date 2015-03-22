#!/bin/bash

set -e

scriptname=$(basename $0)
lock="$HOME/workspace/beam_test/${scriptname}-lock"

exec 200>$lock
if(! flock -n 200);then
  echo "an other instance is running" 
  exit 1
fi

logfile=$HOME/workspace/beam_test/syncronize.log
echo "### START TIME: " &>>${logfile}
date "+%D %R" &>>${logfile}
python	$HOME/src/git/beam_test/Scripts/syncronize.py &>>${logfile}
echo "### STOP TIME: " &>>${logfile}
date "+%D %R%n" &>>${logfile}