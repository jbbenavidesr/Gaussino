#
# Options specific for a given job
# ie. setting of random number seed and name of output files
#

from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts=50000
LHCbApp().EvtMax = nEvts

Gauss().OutputType = 'NONE'
Gauss().Histograms = 'NONE'
