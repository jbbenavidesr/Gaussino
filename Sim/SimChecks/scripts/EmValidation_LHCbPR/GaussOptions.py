###################################################################################################
#
# Detector and tools configurations.
# 
# Peter Griffith 21/11/2014
# email:peter.griffith@cern.ch
#
###################################################################################################

import sys
sys.path.append('./')
from Configurables import Gauss
from Gauss.Configuration import *

from Gaudi.Configuration import *
from Configurables import LHCbApp
from Configurables import CondDB
from Configurables import CondDBAccessSvc
from configurations import config

importOptions("./configurations.py")
opts     = config()
testType = opts['testType']
veloType = opts['veloType']
dRays    = opts['dRays']
emPL     = opts['emPL']

print 'EM physics list set in config: '+str(emPL)
def switchOffRICHCuts():
	from Configurables import SimulationSvc
	SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHesOff.xml"

def addTool():
    from Configurables import GiGa, BremVeloCheck
    giga = GiGa()
    giga.TrackSeq.Members.append( "BremVeloCheck" )


if (veloType == 'velo'):
	LHCbApp().DDDBtag   = "dddb-20130312-1"
	LHCbApp().CondDBtag = "sim-20130222-1-vc-md100"
	Gauss.DetectorGeo = {"Detectors": ["PuVeto","Velo","Rich1","Rich2"] }
	Gauss.DetectorSim = {"Detectors": ["PuVeto","Velo","Rich1","Rich2"] }
	Gauss.DetectorMoni ={"Detectors": ["PuVeto","Velo","Rich1","Rich2"] }
	importOptions ("$GAUSSOPTS/RICHesOff.py"  )
	appendPostConfigAction( switchOffRICHCuts )


Gauss.PhysicsList = {"Em":emPL, "Hadron":'QGSP_BERT', "GeneralPhys":True, "LHCbPhys":False}
from Configurables import SimulationSvc
Gauss.DeltaRays = dRays

if(testType == 'bream' or 'both'):
	appendPostConfigAction(addTool)





