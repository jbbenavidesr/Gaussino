from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Beam6500GeV-md100-2016-nu1.6.py")
importOptions("$APPCONFIGOPTS/Gauss/DataType-2016.py")
importOptions("$APPCONFIGOPTS/Gauss/RICHRandomHits.py")
importOptions("$DECFILESROOT/options/59990002.py")
importOptions("$APPCONFIGOPTS/Gauss/NoPacking.py")
importOptions("$LBPGUNSROOT/options/PGuns.py")

# Change PGun output verbosity
from Configurables import ParticleGun, FlatPtRapidity
ParticleGun().OutputLevel = INFO
ParticleGun().FlatPtRapidity.OutputLevel = INFO

#DB tags
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20150724" #current
LHCbApp().CondDBtag = "sim-20161124-2-vc-mu100" #current

#Events to Process
LHCbApp().EvtMax = 3000

#No output file
from Gauss.Configuration import *
Gauss().OutputType = 'NONE' 


from Configurables import MuonMultipleScatteringChecker

MuonMultipleScatteringTest = MuonMultipleScatteringChecker("MuonMultipleScatteringTest")
MuonMultipleScatteringTest.HistoDir = "MuonMultipleScatteringChecker/MuonMultipleScatteringTest"
MuonMultipleScatteringTest.OutputLevel = INFO
#Uncommment for monitor ntuples
#MuonMultipleScatteringTest.fillNtuple = True


# MuonHitChecker is run by default so no need to setup here, however if you wanted to view the monitoring hisoograms
# uncomment the below to create an instance of the MuonHitChecker with these enabled to output

#from Configurables import MuonHitChecker
#MuonHitCheckerTest = MuonHitChecker("MuonHitCheckerTest")
#MuonHitCheckerTest.HistoDir = "MuonHitChecker/MuonHitCheckerTest"
#MuonHitCheckerTest.OutputLevel = INFO
#MuonHitCheckerTest.DetailedMonitor = True

GaudiSequencer("DetectorsMonitor").Members +=  [MuonMultipleScatteringTest] #, MuonHitCheckerTest

HistogramPersistencySvc().OutputFile ='MuonMoniSim_histos.root'

#Uncomment for ntuples to be outputted
#ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
#NTupleSvc().Output = ["FILE1 DATAFILE='MuonTestResults/GaussMonitor.root' TYP='ROOT' OPT='NEW'"]


# Options specific for a given job
# ie. setting of random number seed and name of output files

#from Gaudi.Configurables import GenInit
#from Configurables import CondDB, LHCbApp, DDDBConf, CondDBAccessSvc, Gauss

#--Generator phase, set random numbers
#GaussGen = GenInit("GaussGen")
#GaussGen.FirstEventNumber = 1
#GaussGen.RunNumber = 4585


