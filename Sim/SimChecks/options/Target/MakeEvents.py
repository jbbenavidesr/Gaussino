### This option generates all pguns but only in a block of 1mm Al
### Takes approximately 1.3h 

import sys
import os
import re

from Target.TargetCreateEvents import RunTargetJobs
from optparse import OptionParser

def getArgsNum(line):
   x = re.findall(r"\d+",line)
   y = []
   for i in x:
	y.append(int(i))
   return y
def getArgsChar(line):
   x = re.findall(r"\w+",line)
   return x

pwd = os.getcwd()

parser = OptionParser()
parser.add_option("--physList", default="['FTFP_BERT','QGSP_BERT']", dest="physList", help="Specify a single Physics List to be used, default set to both FTFP_BERT and QGSP_BERT" )
parser.add_option("--energyList", default="[1]", dest="energies", help="Specify which energies (in GeV) to use for particle guns" )
parser.add_option("--materialList", default="['Al']", dest="materials", help="Specify the Target materials" )
parser.add_option("--thicknessList", default="[1]", dest="thickness", help="Specify the Target thickness" )
parser.add_option("--pgunList", default="['p']", dest="pguns", help="Specify the particle gun" )
(opts, args) = parser.parse_args()

#version = re.search("GAUSS_v(.*?)/",os.environ["GAUSSROOT"]).groups(0)[0]
path='%s/TargetOutput' % pwd # where you want your output (absolute or relative path)

energies=getArgsNum(opts.energies)
models = getArgsChar(opts.physList)
materials=getArgsChar(opts.materials) # 'Al' 'Be' 'Si'
thicks=getArgsNum(opts.thickness)  #in mm 1, 5, 10 (only)
pguns=getArgsChar(opts.pguns) # Available: 'Piminus' 'Piplus' 'Kminus' 'Kplus' 'p' 'pbar'

RunTargetJobs(path, models, pguns, energies, materials, thicks)

from ROOT import *
from Target.TargetPlots import Plot

plots = [ "RATIO_TOTAL", "RATIO_INEL", "TOTAL", "INEL", "EL", "MULTI", "MULTI_NCH", "MULTI_GAMMA", "ASYM_INEL" ]

file = TFile(path+"/TargetsPlots.root")
dataTree = file.Get("summaryTree")
	
os.system("mkdir -p "+path+"/Kaons")
os.system("mkdir -p "+path+"/Protons")
os.system("mkdir -p "+path+"/Pions")

for p in plots :
	Plot( dataTree, "energy", p, path, models , pguns , materials , 2 , thicks[0], True )
	if "p" in pguns and "pbar" in pguns :
		Plot( dataTree, "energy", p, path+"/Protons", models , ["p","pbar"] , materials , 2 , thicks[0], True )
	if "Kplus" in pguns and "Kminus" in pguns :
		Plot( dataTree, "energy", p, path+"/Kaons", models , ["Kplus","Kminus"] , materials , 2 , thicks[0], True )
	if "Piplus" in pguns and "Piminus" in pguns :
		Plot( dataTree, "energy", p, path+"/Pions", models , ["Piplus","Piminus"] , materials , 2 , thicks[0], True )
