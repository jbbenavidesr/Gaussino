from Configurables import Gaussino
from Gaussino.Generation import GenPhase
from Configurables import HepMCWriter
Gaussino().Phases = ['Generator']
GenPhase().ParticleGun = True
Gaussino().EvtMax=1000
Gaussino().EnableHive = True

Gaussino().ThreadPoolSize = 20
Gaussino().EventSlots = 20

HepMCWriter().Writer = 'WriterAscii'
GenPhase().WriteHepMC = True
Gaussino().DatasetName =  'FirstRun'
Gaussino().DatasetNameForced = True
