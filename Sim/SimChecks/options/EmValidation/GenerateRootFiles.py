###################################################################################################
#
# Generate root files that contain full information of the EMGaussMoni and BremVeloCheck. Energies,
# physics lists and particles with all posible permutations make unique examples. Each of these
# results are stored in rootFile with relevant name.
#
# Peter Griffith 21/11/2014  
# email:peter.griffith@cern.ch
#
# Georgios Chatzikonstantinidis 14/05/2017
# email:georgios.chatzikonstantinidis@cern.ch
#
###################################################################################################

import os
import sys
from tempfile import mkstemp
from shutil import move
from os import remove, close, system

from itertools import *
import fileinput
pathScripts=os.environ["SIMCHECKSROOT"]+'/options/EmValidation'

def replaceFile(path,pattern,sub):
    outpath=path+"tmp"
    with open(path) as infile, open(outpath,"w") as outfile:
        for i, line in enumerate(infile):
            if pattern in line:
                outfile.write("\t"+sub+"\n")
            else:
                outfile.write(line)
        os.remove(path)
        os.rename(outpath,path)
                

def replaceVar(var,val):
    fPath = pathScripts+'/configurations.py'
    pattern = var+" = "
    sub = pattern+str(val)
    replaceFile(fPath,pattern,sub)
    
    
def replaceStr(var,val,ToStr):
    fPath = pathScripts+'/configurations.py'
    pattern = var+" = "
    sub = pattern+"'"+str(val)+"'" if ToStr else pattern+str(val)
    replaceFile(fPath,pattern,sub)
        
    
def runJob(pgunID,emPL,pgunE):
    replaceVar('pgunID',pgunID)
    replaceStr('emPL',emPL,True)
    replaceStr('pgunE',pgunE,False)
    os.system('gaudirun.py %s/runTest.py' % pathScripts)
        

#for id in [11,13,211]:
#    for pl in ['NoCuts','Opt1','Opt2','Opt3','LHCb']:
#        for en in [0.1, 0.2, 0.4, 1., 5., 10., 16.8, 50., 100., 120., 168.]:
#            runJob(id,pl,en)
#            os.remove("testout.root")
for id in [11,13]:
    for pl in ['NoCuts','Opt1','Opt2','Opt3','LHCb']:
        for en in [0.1, 0.2, 0.4, 1., 5., 10., 16.8, 50., 100., 120., 168.]:
            runJob(id,pl,en)
            os.remove("testout.root")
            

