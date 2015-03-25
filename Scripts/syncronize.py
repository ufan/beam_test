#!/usr/bin/python                                                                                                                        
from ConfigParser import ConfigParser
import pexpect

cfg_file = '/home/ufan/src/git/beam_test/Scripts/ssh.cfg'
cfg = ConfigParser()
cfg.read(cfg_file)
passwd = cfg.get('zangjj', 'beamtest')
		 
#cmd= 'rsync -rtuv --delete zangjj@beamtest:/data/15testdata/BT/RAWData_Bin/ /home/ufan/workspace/beam_test/DAMPE_HEX/'
cmd_hex= 'rsync -rtuv --delete zangjj@beamtest:/data/15testdata/BT/RAWData_Bin/ /run/media/ufan/15beamtest/DAMPE_HEX/'
child_hex=pexpect.spawn(cmd_hex)
child_hex.expect('password:')
child_hex.sendline(passwd+'\n')
child_hex.interact()
#log=pexpect.run(cmd_hex,events={'password:':(passwd+'\n')})
#print log


#cmd_anc= 'rsync -rtuv --delete zangjj@beamtest:/mnt/DAMPEDATA/MARCH_2015_TB/ANCILLARY/ /run/media/ufan/15beamtest/ANC/'
#child_anc=pexpect.spawn(cmd_anc)
#child_anc.expect('password:')
#child_anc.sendline(passwd+'\n')
#child_anc.interact()
#log=pexpect.run(cmd_anc,events={'password:':(passwd+'\n')})
#print log


#cmd_ams= 'rsync -rtuv --delete zangjj@beamtest:/home/zangjj/bt2015/run/AMS/ /run/media/ufan/15beamtest/AMS/'
#child_ams=pexpect.spawn(cmd_ams)
#child_ams.expect('password:')
#child_ams.sendline(passwd+'\n')
#child_ams.interact()
#log=pexpect.run(cmd_ams,events={'password:':(passwd+'\n')})
#print log