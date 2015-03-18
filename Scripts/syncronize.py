#!/usr/bin/python                                                                                                                        
from ConfigParser import ConfigParser
import pexpect

cfg_file = 'ssh.cfg'
cfg = ConfigParser()
cfg.read(cfg_file)
passwd = cfg.get('zangjj', 'beamtest')
		 
cmd= 'rsync -rtuv --delete zangjj@beamtest:/data/15testdata/BT/RAWData_Bin/A2Data00_20150317-224625.frd /home/ufan/workspace/beam_test/DAMPE_HEX/'
child = pexpect.spawn(cmd)
child.expect('password:')
child.sendline(passwd)
child.interact()

