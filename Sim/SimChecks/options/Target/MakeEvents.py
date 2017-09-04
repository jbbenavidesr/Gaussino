#################################################################################
##  General Test Script for Hadronic Cross Section and Multiplicity test       ##
##  which can be passed options for target materials, particle guns,           ##
##  thickness of the target and energy.                                        ##
##                                                                             ##
##  @author   :  Kristian Zarebski                                             ##
##  @date     :  last modified 2017-02-03                                      ##
#################################################################################

import sys
import os
import re
import subprocess

from Target.TargetCreateEvents import RunTargetJobs
from argparse import ArgumentParser


def getArgsNum(line):
    x = re.findall(r"\d+", line)
    y = []
    for i in x:
        y.append(int(i))
    return y


def getArgsChar(line):
    x = re.findall(r"\w+", line)
    return x

pwd = os.getcwd()
subprocess.check_call("mkdir -p {}/TargetOutput".format(pwd), shell=True)
output_directory = os.path.join(pwd, "TargetOutput")

parser = ArgumentParser('MakeTargetTestEvents')
parser.add_argument("--physList", default="['FTFP_BERT','QGSP_BERT']", dest="physList", help="Specify a single Physics List to be used, default set to both FTFP_BERT and QGSP_BERT")
parser.add_argument("--energyList", default="[1,2,5,10,100]", dest="energies", help="Specify which energies (in GeV) to use for particle guns")
parser.add_argument("--materialList", default="['Al']", dest="materials", help="Specify the Target materials")
parser.add_argument("--thicknessList", default="[1]", dest="thickness", help="Specify the Target thickness")
parser.add_argument("--pgunList", default="['p', 'pbar', 'Kplus', 'Kminus','Piplus, 'Piminus']", dest="pguns", help="Specify the particle gun")
parser.add_argument("--use-gauss-geo", dest="gauss_geo", default=False, action='store_true', help='Use GaussGeo instead of GigaGeo for geometry initialisation if option available in Gauss version')
parser.add_argument("--use-giga-geo", dest="giga_geo", default=False, action='store_true', help='Use GiGaGeo instead of GaussGeo for geometry initialisation if option available in Gauss version')
opts = parser.parse_args()
energies = getArgsNum(opts.energies)
models = getArgsChar(opts.physList)
materials = getArgsChar(opts.materials)   # 'Al' 'Be' 'Si'
thicks = getArgsNum(opts.thickness)    # in mm 1, 5, 10 (only)
pguns = getArgsChar(opts.pguns)       # Available: 'Piminus' 'Piplus' 'Kminus' 'Kplus' 'p' 'pbar'

gauss_geo_opts = ''
if opts.gauss_geo:
  gauss_geo_opts = 'from Configurables import Gauss; Gauss().UseGaussGeo = True'
elif opts.giga_geo:
  gauss_geo_opts = 'from Configurables import Gauss; Gauss().UseGaussGeo = False'

RunTargetJobs(output_directory, models, pguns, energies, materials, thicks, use_gauss_geo=gauss_geo_opts)

from ROOT import *
from Target.TargetPlots import Plot

plots = ["RATIO_TOTAL", "RATIO_INEL", "TOTAL", "INEL", "EL", "MULTI", "MULTI_NCH", "MULTI_GAMMA", "ASYM_INEL"]

file_ = TFile(os.path.join(output_directory, "ROOTFiles/TargetsPlots.root"))
dataTree = file_.Get("summaryTree")

subprocess.check_call("mkdir -p {}/Kaons".format(output_directory), shell=True)
subprocess.check_call("mkdir -p {}/Protons".format(output_directory), shell=True)
subprocess.check_call("mkdir -p {}/Pions".format(output_directory), shell=True)

for t in thicks:
    for p in plots:
        Plot(dataTree, "energy", p, output_directory, models, pguns, materials, 2, t, True)
        if "p" in pguns and "pbar" in pguns:
            Plot(dataTree, "energy", p, os.path.join(output_directory, "Protons"), models, ["p", "pbar"], materials, 2, t, True)
        if "Kplus" in pguns and "Kminus" in pguns:
            Plot(dataTree, "energy", p, os.path.join(output_directory, "Kaons"), models, ["Kplus", "Kminus"], materials, 2, t, True)
        if "Piplus" in pguns and "Piminus" in pguns:
            Plot(dataTree, "energy", p, os.path.join(output_directory, "Pions"), models, ["Piplus", "Piminus"], materials, 2, t, True)

subprocess.check_call( 'hadd -f {} {}'.format( os.path.join(output_directory, 'ROOTGraphs/TargetTestAllPlots.root'), os.path.join(output_directory, 'ROOTGraphs/*.root') ), shell=True )
