#######################################################################
## This script runs the Muon tests and stores the output as a        ##
## ROOT file in the MuonTestResults folder in the same               ##
## directory the scirpt is run in for use of a handler to move data  ##
## into LHCbPR.                                                      ##
## The name of the input histogram can be can be changed in          ## 
## the MuonMoniSim.py options file. Number of events can be          ## 
## changed in the Gauss-Job.py options file, default is 50000.       ##
## To run the script do:                                             ##
## lb-run Gauss [Gauss version]  python runmuontest.py               ##
## or if doing devleopment replace lb-run Gauss with path/to/run     ##
## @author : R.Calladine                                             ## 
## @date   : last modified 2016-12-12                                ##
#######################################################################

import sys, os, json

#Environment variable created when Gauss is built
mu_path = os.environ['MUONMONISIMROOT']
pwd = os.getcwd()

os.system("mkdir -p {pwd}/MuonTestResults".format(pwd=pwd))

cmd = "gaudirun.py {mu_path}/options/Gauss-Job.py {mu_path}/options/MuonMoniSim.py".format(mu_path=mu_path)
os.system(cmd)

os.system("mv ./MuonMoniSim_histos.root {pwd}/MuonTestResults".format(pwd=pwd))



