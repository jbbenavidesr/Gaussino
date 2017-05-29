from Gaudi.Configuration import *

from Configurables import Generation
Generation().CommonVertex = True
Generation().SampleGenerationTool = "Special"
from Configurables import Special, DaughtersInLHCbKeepOnlySignal, FixedNInteractions
Generation().addTool( Special )
Generation().PileUpTool = "FixedNInteractions"
Generation().addTool( FixedNInteractions )
Generation().FixedNInteractions.NInteractions = 2
Generation().Special.CutTool = "DaughtersInLHCbKeepOnlySignal"
Generation().Special.PileUpProductionTool = "Pythia8Production"
Generation().Special.ReinitializePileUpGenerator = False
Generation().Special.addTool( DaughtersInLHCbKeepOnlySignal )
Generation().Special.DaughtersInLHCbKeepOnlySignal.SignalPID = 443
