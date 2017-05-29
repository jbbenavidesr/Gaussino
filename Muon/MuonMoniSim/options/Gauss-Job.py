# Options specific for a given job
# ie. setting of random number seed and name of output files

from Gaudi.Configurables import GenInit
from Configurables import CondDB, LHCbApp, DDDBConf, CondDBAccessSvc, Gauss

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber = 4585
Gauss().OutputType = 'NONE'

#--Number of events
nEvts = 50000
LHCbApp().EvtMax = nEvts
LHCbApp().DDDBtag   = "dddb-20150724" #current
LHCbApp().CondDBtag = "sim-20161124-2-vc-mu100" #current


