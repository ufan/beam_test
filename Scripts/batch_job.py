#!/bin/python

import os
import sys
import re

HEXDIR='/run/media/ufan/15beamtest/DAMPE_HEX/'
ADCDIR='/run/media/ufan/15beamtest/DAMPE_ADC/'
JOB_PSDANALYZE='/home/ufan/src/git/beam_test/Scripts/psd-analyze.py'
JOB_PSDGENERALCHECK='/home/ufan/src/git/beam_test/AlgPsdGeneralCheck/Share/Job_PsdGeneralCheck.py'

for file in os.listdir(HEXDIR):
  if os.path.isdir(file):
    continue
  if re.search('.frd',file):
    if(os.path.isfile(ADCDIR+file.rsplit('.frd',1)[0]+'_ADC.root')):
      continue
    os.system('python '+JOB_PSDANALYZE+' '+file)
    