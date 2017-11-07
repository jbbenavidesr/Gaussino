#!/usr/bin/env python
#################################################################################
## This option file helps you save time by running in sequence                 ##
## RanLengthAna.py and RadLengthAna_VELO.py and merging the outputs.           ##
## You can run this simply by "python rad_length_scan.py"                      ##
## Twiki at: https://twiki.cern.ch/twiki/bin/view/LHCb/RadLengthStudies        ##
##                                                                             ##
##  @author : K. Zarebski                                                      ##
##  @date   : last modified on 2017-06-09                                      ##
#################################################################################

import sys
import os

pwd = os.getcwd()
pwd_str = pwd
home = os.environ['HOME']

simchecks_local = os.environ["SIMCHECKSROOT"]

base = simchecks_local + "/options/RadLength/"
sys.path.append(os.path.join(simchecks_local, 'python'))

from RadLengthMakePlots import makePlots

outputpath = os.path.join(pwd_str, 'Rad_length/root_files')
outputpathpdf = os.path.join(pwd_str, 'Rad_length/pdf_files')

out = 'Rad_merged.root'
if(len(sys.argv) == 2):
    out = sys.argv[1]

os.system("mkdir -p %s/Rad_length/" % pwd)
os.system("mkdir -p %s/Rad_length/root_files/" % pwd)
os.system("mkdir -p %s/Rad_length/data_tables/" % pwd)
os.system("mkdir -p %s/Rad_length/pdf_files/" % pwd)
cmd = "gaudirun.py {base}/MaterialEvalGun.py {base}/Gauss-Job.py {base}".format(base=base)
os.system(cmd + "RadLengthAna.py")
os.system(cmd + "RadLengthAna_VELO.py")

output = os.path.join(outputpath, out)
merge_command = 'hadd -f {output} {pwd}/Rad.root {pwd}/Rad_VELO.root'.format(output=output, pwd='%s/Rad_length/root_files' % pwd)
os.system(merge_command)

makePlots(os.path.join(outputpath, out), outputpathpdf, "rad")
makePlots(os.path.join(outputpath, out), outputpathpdf, "inter")
