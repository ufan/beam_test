#!/usr/bin/python

import os
import sys
import glob

#---- science data dirs-----------------------
HEXDIR='/run/media/ufan/15beamtest/DAMPE_HEX/'
ADCDIR='/run/media/ufan/15beamtest/DAMPE_ADC/'
ANCDIR='/run/media/ufan/15beamtest/ANC/'
PSDSIMPLEDIR='/run/media/ufan/15beamtest/psd_simple/'

DMP_RDC_OUT_FILE_NAME_IN_JOBOPTION='DmpAdc.root'
#---- analysis algorithm jobs-----------------
JOB_HEX2ROOT=os.environ['DMPSWSYS']+"/share/TestRelease/JobOpt_Hex2Root"
JOB_GENERALCHECK='/home/ufan/src/git/beam_test/AlgPsdGeneralCheck/share/Job_PsdGeneralCheck'
JOB_PSDTOSIMPLE='/home/ufan/src/git/beam_test/AlgPsdToSimple/share/Job_PsdToSimple'


def run_dampe_rdc(inputfilename, outputfilename):
        if os.path.isfile(DMP_RDC_OUT_FILE_NAME_IN_JOBOPTION):
            os.system("rm %s "%(DMP_RDC_OUT_FILE_NAME_IN_JOBOPTION))
            
        command = "python %s %s >> hex2root.log"%(JOB_HEX2ROOT,
                                           HEXDIR+inputfilename
                                           ) 
        os.system(command)
        if os.path.isfile(DMP_RDC_OUT_FILE_NAME_IN_JOBOPTION):
            os.system("mv %s %s"%(DMP_RDC_OUT_FILE_NAME_IN_JOBOPTION,ADCDIR+outputfilename))
        return
            


#---------------------------------------------
Dampefile=sys.argv[1]
Adcfile=Dampefile.split('.frd')[0]+'_ADC.root'
Ancfile=''
useAnc='false'
offsetDampe='0'
offsetAnc='0'

argc=len(sys.argv)

run_dampe_rdc(Dampefile,Adcfile)
#----------------------------------------------
cmd_GeneralCheck='python '+JOB_GENERALCHECK+' '+Adcfile
os.system(cmd_GeneralCheck)

if argc>2:
  useAnc=sys.argv[2]
  Ancfile=ANCDIR + sys.argv[3]
  if argc>4:
    offsetDampe=sys.argv[4]
  if argc>5:
    offsetAnc=sys.argv[5]
  cmd_ToSimple='python '+JOB_PSDTOSIMPLE+' '+Adcfile+' '+useAnc+' '+Ancfile+' '+offsetDampe+' '+offsetAnc
  os.system(cmd_ToSimple)
elif argc==2:
  cmd_ToSimple='python '+JOB_PSDTOSIMPLE+' '+Adcfile
  os.system(cmd_ToSimple)
    

#for file in glob.glob(ANCDIR+'*.root'):
#for file in os.listdir(ANCDIR):
#  print file
