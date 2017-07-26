#################################################################################################
#
# Set options for gauss job. Options are set from config function.
#
#  Peter Griffith 21.11.14
#  email:peter.griffith@cern.ch
#
# Georgios Chatzikonstantinidis
# email.georgios.chatzikonstantinidis@cern.ch
#
#################################################################################################

import sys
sys.path.append('./')
from Gauss.Configuration import *
from Configurables import CondDB, LHCbApp, DDDBConf, CondDBAccessSvc
from Configurables import Gauss
from configurations import config


opts     = config()
saveSim  = opts['saveSim' ]
nEvts    = opts['nEvts'   ]
Gauss().Production = 'PGUN'


GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber = 4585


LHCbApp().EvtMax = nEvts
Gauss().Production = 'PGUN'
Gauss().OutputType = 'NONE'


if (saveSim):
	tape = OutputStream("GaussTape")
	tape.Output  = "DATAFILE='PFN:myOutputFile.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"
	ApplicationMgr(OutStream = [tape])
histosName = 'RootFileSimMonitor_{}_{}_{}_{}-histos.root'.format(str(      opts['emPL'    ]) ,
								 str(float(opts['pgunID'  ])),
								 str(      opts['pgunE'   ]) ,
								 str(      opts['veloType']))
HistogramPersistencySvc().OutputFile = histosName
ApplicationMgr().ExtSvc += ["NTupleSvc"]
NTupleSvc().Output = ["FILE1 DATAFILE='testout.root' TYP='ROOT' OPT='NEW'"]
importOptions ("./Pgun.py")

from Configurables import EMGaussMoni
SimMonitor = GaudiSequencer("SimMonitor")
SimMonitor.Members += [EMGaussMoni("VeloGaussMoni")]

