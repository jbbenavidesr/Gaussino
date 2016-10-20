from Configurables import MinimumBias, Generation, CRMCProduction,Pythia8Production, Special, Inclusive, SignalPlain, FixedNInteractions, DaughtersInLHCbKeepOnlySignal, Gauss

import math

gen = Generation()

def finalConfiguration():
    event_type = gen.getProp('EventType')
    if event_type != 30000000: ## embedding
        gen.CommonVertex = True
        gen.SampleGenerationTool = "Special"
        #
        gen.PileUpTool = "FixedNInteractions"
        gen.addTool( FixedNInteractions )
        gen.FixedNInteractions.NInteractions = 2
        #
        gen.Special.CutTool = "DaughtersInLHCbKeepOnlySignal"
        gen.Special.addTool( DaughtersInLHCbKeepOnlySignal )

        signal_pid = []
        if not gen.SignalPlain.getProp( 'SignalPIDList' ):
            signal_pid = gen.SignalPlain.getProp( 'SignalPIDList' )
        else: signal_pid = [ 443 ]
        
        gen.Special.DaughtersInLHCbKeepOnlySignal.SignalPID = math.fabs( signal_pid[ 0 ] )
        #
        gen.Special.ProductionTool = "Pythia8Production/SignalPythia8"
        gen.Special.PileUpProductionTool = "CRMCProduction"
        gen.Special.ReinitializePileUpGenerator = False

        Generation().Special.addTool( Pythia8Production , name = 'SignalPythia8' )
        Generation().Special.SignalPythia8.Tuning = "LHCbDefault.cmd"
    ## decide if fixed target or not
    gauss = Gauss()
    if gauss.getProp('BeamMomentum')==0. or gauss.getProp('B2Momentum')==0.:
        gen.MinimumBias.CRMCProduction.Frame = "target"
        gen.Special.CRMCProduction.Frame = "target"
        gen.Inclusive.CRMCProduction.Frame = "target"
        gen.SignalPlain.CRMCProduction.Frame = "target"
    else:
        gen.MinimumBias.CRMCProduction.Frame = "nucleon-nucleon"
        gen.Special.CRMCProduction.Frame = "nucleon-nucleon"
        gen.Inclusive.CRMCProduction.Frame = "nucleon-nucleon"
        gen.SignalPlain.CRMCProduction.Frame = "nucleon-nucleon"

from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( finalConfiguration )


## Configure the production tools
gen.addTool( MinimumBias )
gen.MinimumBias.ProductionTool = "CRMCProduction"
gen.MinimumBias.addTool( CRMCProduction )
gen.addTool( Special )
gen.Special.addTool( CRMCProduction )
gen.addTool( Inclusive )
gen.Inclusive.addTool( CRMCProduction )
gen.addTool( SignalPlain )
gen.SignalPlain.addTool( CRMCProduction )

configuration_commands = [
  "fdpmjetpho dat $CRMC_TABS/phojet_fitpar.dat",
  "fdpmjet dat $CRMC_TABS/dpmjet.dat",
  "fqgsjet dat $CRMC_TABS/qgsjet.dat",
  "fqgsjet ncs $CRMC_TABS/qgsjet.ncs",
  "fqgsjetII03 dat $CRMC_TABS/qgsdat-II-03.lzma",
  "fqgsjetII03 ncs $CRMC_TABS/sectnu-II-03",
  "fqgsjetII dat $CRMC_TABS/qgsdat-II-04.lzma",
  "fqgsjetII ncs $CRMC_TABS/sectnu-II-04",
  "fname check  none",
  "fname initl  $CRMC_TABS/epos.initl",
  "fname iniev  $CRMC_TABS/epos.iniev",
  "fname inirj  $CRMC_TABS/epos.inirj",
  "fname inics  $CRMC_TABS/epos.inics",
  "fname inihy  $CRMC_TABS/epos.inihy"
]

gen.MinimumBias.CRMCProduction.SwitchOffEventTruncation = True 
gen.MinimumBias.CRMCProduction.ProduceTables = False
gen.MinimumBias.CRMCProduction.BoostAndRotate = False
gen.MinimumBias.CRMCProduction.AddUserSettingsToDefault = True
gen.MinimumBias.CRMCProduction.Commands += configuration_commands

gen.Special.CRMCProduction.SwitchOffEventTruncation = True 
gen.Special.CRMCProduction.ProduceTables = False
gen.Special.CRMCProduction.BoostAndRotate = False
gen.Special.CRMCProduction.AddUserSettingsToDefault = True
gen.Special.CRMCProduction.Commands += configuration_commands

gen.Inclusive.CRMCProduction.SwitchOffEventTruncation = True 
gen.Inclusive.CRMCProduction.ProduceTables = False
gen.Inclusive.CRMCProduction.BoostAndRotate = False
gen.Inclusive.CRMCProduction.AddUserSettingsToDefault = True
gen.Inclusive.CRMCProduction.Commands += configuration_commands

gen.SignalPlain.CRMCProduction.SwitchOffEventTruncation = True 
gen.SignalPlain.CRMCProduction.ProduceTables = False
gen.SignalPlain.CRMCProduction.BoostAndRotate = False
gen.SignalPlain.CRMCProduction.AddUserSettingsToDefault = True
gen.SignalPlain.CRMCProduction.Commands += configuration_commands
