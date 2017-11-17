# To run the test execute the bash script muonmonisim.sh prefixing with
# ./run or lb-run Gauss <version> where appropriate.
# Output can be found in the MuonTestResults directory which will be
# created in the directory from where the script is run.
# @author : R.Calladine                                             
# @date   : last modified 2017-06-07                                


import sys, os

#Environment variable created when Gauss is built
mu_path = os.environ['MUONMONISIMROOT']

phys_list_dict = { 
                   'EmOpt1' : '$APPCONFIGOPTS/Gauss/G4PL_FTFP_BERT_EmOpt1.py',
                   'EmNoCuts' : '$APPCONFIGOPTS/Gauss/G4PL_FTFP_BERT_EmNoCuts.py',
                   'EmNoCutsNoLHCb' :  '$APPCONFIGOPTS/Gauss/G4PL_FTFP_BERT_EmNoCuts_noLHCbphys.py'
                 }

import argparse

parser = argparse.ArgumentParser( 'MuonMoniSim options to run with different Geant4 EM Physics Lists' )

parser.add_argument( 'physList', help='G4 EM Physics List', choices=[key for key in phys_list_dict] )

args = parser.parse_args()

pwd = os.getcwd()

os.system("mkdir -p {}/MuonTestResults".format(pwd))

cmd = "gaudirun.py $PRCONFIGOPTS/Gauss/PRTEST-2016-SIM-PGun-59990002-5Kevts-MuonMoni.py  {}".format( phys_list_dict[args.physList])
os.system(cmd)

os.system("mv ./MuonMoniSim_histos.root {}/MuonTestResults".format(pwd))
os.system("mv {}/MuonTestResults/MuonMoniSim_histos.root {}/MuonTestResults/MuonMoniSim_{}.root".format(pwd,pwd,args.physList)) 
