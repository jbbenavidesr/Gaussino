#!/usr/bin/env python
#################################################################################
## This option file helps you save time by running in sequence                 ##
## RanLengthAna.py and RadLengthAna_VELO.py and merging the outputs.           ##
## You can run this simply by "python rad_length_scan.py"                      ##
## Twiki at: https://twiki.cern.ch/twiki/bin/view/LHCb/RadLengthStudies        ##
##                                                                             ##
##  @author : L.Pescatore (Modified by K. Zarebski)                            ##
##  @date   : last modified on 2016-12-07                                      ##
#################################################################################

import sys
import os

pwd = os.getcwd()
pwd_str = pwd
home = os.environ['HOME']

#######NEEDS TO BE CHANGED TO LOCATION OF SIMCHECKS IN MAIN GAUSS####################
#simchecks_local = home + '/private/Gauss/Sim/SimChecks'
simchecks_local = os.environ["SIMCHECKSROOT"]

base = simchecks_local + "/options/RadLength/"
from RadLengthMakePlots import makePlots


outputpath = pwd_str + '/Rad_length/root_files/'
outputpathpdf = pwd_str + '/Rad_length/pdf_files/'

out = 'Rad_merged.root'
if(len(sys.argv) == 2):
    out = sys.argv[1]

os.system("mkdir -p %s/Rad_length/" % pwd)
os.system("mkdir -p %s/Rad_length/root_files/" % pwd)
os.system("mkdir -p %s/Rad_length/data_tables/" % pwd)
cmd = "gaudirun.py {base}/MaterialEvalGun.py {base}/Gauss-Job.py {base}".format(base=base)
os.system(cmd+"RadLengthAna.py")
os.system(cmd+"RadLengthAna_VELO.py")
    
output=outputpath+out
merge_command = 'hadd -f {output} {pwd}/Rad.root {pwd}/Rad_VELO.root'.format(output=output, pwd='%s/Rad_length/root_files' % pwd)
os.system(merge_command)

makePlots(outputpath + out,outputpathpdf,"rad")
makePlots(outputpath + out,outputpathpdf,"inter")

