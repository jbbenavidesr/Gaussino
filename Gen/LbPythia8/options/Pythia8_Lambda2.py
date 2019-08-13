from Configurables import Generation
from Configurables import MinimumBias
from Configurables import Inclusive
from Configurables import ( SignalPlain, SignalRepeatedHadronization, Special )
from Configurables import Pythia8Production

commandsTuning_Lambda2 = [
    'MultipartonInteractions:pT0Ref = 2.3003',
    'ColourReconnection:lambdaForm = 2',
    'ColourReconnection:timeDilationPar = 2.3123',
    'ColourReconnection:m0          = 1.4563',
    'ColourReconnection:junctionCorrection = 1.0763'
    'StringFlav:probSQtoQQ          = 0.5117',
    'StringFlav:mesonSvector        = 0.3860',
    'StringFlav:probStoUD           = 0.3162',
    'StringFlav:probQQtoQ           = 0.0380',
    ]

Pythia8TurnOffFragmentation = [ "HadronLevel:all = off" ]

gen = Generation("Generation")

gen.addTool( MinimumBias , name = "MinimumBias" )
gen.MinimumBias.ProductionTool = "Pythia8Production"
gen.MinimumBias.addTool( Pythia8Production , name = "Pythia8Production" )
gen.MinimumBias.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.MinimumBias.Pythia8Production.Commands += commandsTuning_Lambda2

gen.addTool( Inclusive , name = "Inclusive" )
gen.Inclusive.ProductionTool = "Pythia8Production"
gen.Inclusive.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Inclusive.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.MinimumBias.Pythia8Production.Commands += commandsTuning_Lambda2

gen.addTool( SignalPlain , name = "SignalPlain" )
gen.SignalPlain.ProductionTool = "Pythia8Production"
gen.SignalPlain.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalPlain.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.SignalPlain.Pythia8Production.Commands += commandsTuning_Lambda2

gen.addTool( SignalRepeatedHadronization , name = "SignalRepeatedHadronization" )
gen.SignalRepeatedHadronization.ProductionTool = "Pythia8Production"
gen.SignalRepeatedHadronization.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalRepeatedHadronization.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.SignalRepeatedHadronization.Pythia8Production.Commands += Pythia8TurnOffFragmentation
gen.SignalRepeatedHadronization.Pythia8Production.Commands += commandsTuning_Lambda2

gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "Pythia8Production"
gen.Special.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Special.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.Special.Pythia8Production.Commands += commandsTuning_Lambda2
gen.Special.PileUpProductionTool = "Pythia8Production/Pythia8PileUp"
gen.Special.addTool(Pythia8Production, name = "Pythia8PileUp")
gen.Special.Pythia8PileUp.Tuning = "LHCbDefault.cmd"
gen.Special.Pythia8PileUp.Commands += commandsTuning_Lambda2
gen.Special.ReinitializePileUpGenerator  = False

# Use same generator and configuration for spillover
from Configurables import Gauss
spillOverList = Gauss().getProp("SpilloverPaths")
for slot in spillOverList:
    genSlot = Generation("Generation"+slot)
    genSlot.addTool(MinimumBias, name = "MinimumBias")
    genSlot.MinimumBias.ProductionTool = "Pythia8Production"
    genSlot.MinimumBias.addTool(Pythia8Production, name = "Pythia8Production")
    genSlot.MinimumBias.Pythia8Production.Commands += commandsTuning_Lambda2
    genSlot.MinimumBias.Pythia8Production.Tuning = "LHCbDefault.cmd"




