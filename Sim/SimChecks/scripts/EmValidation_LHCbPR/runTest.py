###########################################################################################################
#
# Generate root files with all information that we need for the LHCbPR test.
#
# Georgios Chatzikonstantinidis
# email: georgios.chatzikonstantinidis@cern.ch
#
###########################################################################################################
import sys,os


sys.path.append(os.environ["SIMCHECKSROOT"]+"/scripts/EmValidation_LHCbPR/")
from Configurables import Gauss
from Gauss.Configuration import *

from Gaudi.Configuration import *
from Configurables import LHCbApp
from Configurables import CondDB
from Configurables import CondDBAccessSvc
from configurations import config
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *


###########################################################################################################
# Set options for Gauss.
def switchOffRICHCuts():
    from Configurables import SimulationSvc
    SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHesOff.xml"

def addTool():
    from Configurables import GiGa, BremVeloCheck
    giga = GiGa()
    giga.TrackSeq.Members.append( "BremVeloCheck" )


if (config()['veloType'] == 'velo'):
    LHCbApp().DDDBtag   = "dddb-20130312-1"
    LHCbApp().CondDBtag = "sim-20130222-1-vc-md100"
    Gauss.DetectorGeo  = { "Detectors": ['PuVeto', 'Velo'] }
    Gauss.DetectorSim  = { "Detectors": ['PuVeto', 'Velo'] }
    Gauss.DetectorMoni = { "Detectors": ['PuVeto', 'Velo'] }
    #importOptions ("$GAUSSOPTS/RICHesOff.py"  )
    #appendPostConfigAction( switchOffRICHCuts )

    
Gauss.PhysicsList = {"Em":config()['emPL'], "Hadron":'FTFP_BERT', "GeneralPhys":True, "LHCbPhys":False}
from Configurables import SimulationSvc
Gauss.DeltaRays = config()['dRays']
if(config()['testType'] == 'bream' or 'both'):appendPostConfigAction(addTool)


Gauss().Production = 'PGUN'
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber = 4585


LHCbApp().EvtMax   = config()['nEvts']
Gauss().Production = 'PGUN'
Gauss().OutputType = 'NONE'


if (config()['saveSim']):
    tape = OutputStream("GaussTape")
    tape.Output  = "DATAFILE='PFN:myOutputFile.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"
    ApplicationMgr(OutStream = [tape])
    
histosName = 'RootFileSimMonitor_{}_{}_{}_{}-histos.root'.format(str(      config()['emPL'    ]) ,
                                                                 str(float(config()['pgunID'  ])),
                                                                 str(      config()['pgunE'   ]) ,
                                                                 str(      config()['veloType']))
HistogramPersistencySvc().OutputFile = histosName
ApplicationMgr().ExtSvc += ["NTupleSvc"]
NTupleSvc().Output = ["FILE1 DATAFILE='testout.root' TYP='ROOT' OPT='NEW'"]


###############################################################################################################
# Set options and run particle gun
ParticleGun = ParticleGun("ParticleGun")
ParticleGun.EventType = 53210205;


from Configurables import MomentumRange
ParticleGun.addTool(MomentumRange, name="MomentumRange")
ParticleGun.ParticleGunTool = "MomentumRange"


from Configurables import FlatNParticles
ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
ParticleGun.NumberOfParticlesTool = "FlatNParticles"
ParticleGun.MomentumRange.PdgCodes = [-1*config()['pgunID'], config()['pgunID']]


ParticleGun.MomentumRange.MomentumMin = config()['pgunE']*GeV
ParticleGun.MomentumRange.MomentumMax = config()['pgunE']*GeV
ParticleGun.MomentumRange.ThetaMin    = 0.0*rad
ParticleGun.MomentumRange.ThetaMax    = 0.39*rad


if(config()['testType']=='dedx' or config()['testType']=='both'):
    from Configurables import EMGaussMoni
    SimMonitor = GaudiSequencer("SimMonitor")
    SimMonitor.Members += [EMGaussMoni("VeloGaussMoni")]






