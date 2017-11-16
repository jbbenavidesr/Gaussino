# Runs the Muon Multiple Scattering Test for different EM Physics Lists
# To run the test execute the bash script muonmonisim.sh prefixing with
# ./run or lb-run Gauss <version> where appropriate.
# Output can be found in the MuonTestResults directory which will be
# created in the directory from where the script is run.
# @author : R.Calladine                                             
# @date   : last modified 2017-06-07                                


import sys, os

#Environment variable created when Gauss is built
mu_path = os.environ['MUONMONISIMROOT']

phys_list_dict = { 'EmStd' : '{}/options/G4PL_EmStd.py'.format(mu_path),
                   'EmOpt1' : '{}/options/G4PL_EmOpt1.py'.format(mu_path),
                   'EmOpt2' : '{}/options/G4PL_EmOpt2.py'.format(mu_path),
                   'EmOpt3' : '{}/options/G4PL_EmOpt3.py'.format(mu_path),
                   'EmNoCuts' : '{}/options/G4PL_EmNoCuts.py'.format(mu_path),
                   'EmLHCb' : '{}/options/G4PL_EmLHCb.py'.format(mu_path),
                   'EmLHCbNoCuts' : '{}/options/G4PL_EmLHCbNoCuts.py'.format(mu_path),
                 }

import argparse

parser = argparse.ArgumentParser( 'MuonMoniSim options to run with different Geant4 EM Physics Lists' )

parser.add_argument( 'physList', help='G4 EM Physics List', choices=[key for key in phys_list_dict] )

args = parser.parse_args()

pwd = os.getcwd()

os.system("mkdir -p {}/MuonTestResults".format(pwd))

cmd = "gaudirun.py {}/options/MuonMoniSim.py {} ".format(mu_path, phys_list_dict[args.physList])
os.system(cmd)

os.system("mv ./MuonMoniSim_histos.root {}/MuonTestResults".format(pwd))
os.system("mv {}/MuonTestResults/MuonMoniSim_histos.root {}/MuonTestResults/MuonMoniSim_{}.root".format(pwd,pwd,args.physList)) 
