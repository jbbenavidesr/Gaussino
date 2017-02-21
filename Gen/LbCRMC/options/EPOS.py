from Configurables import MinimumBias, Generation, CRMCProduction,Pythia8Production
from Configurables import Special, Inclusive, SignalPlain, FixedNInteractions
from Configurables import DaughtersInLHCbKeepOnlySignal, Gauss, BoostForEpos, GaudiSequencer
from Configurables import AsymmetricCollidingBeams
from GaudiKernel import SystemOfUnits

import math

__ion_pdg_id__ = { 'Pb': 1000822080 , 'Ar': 1000180400 ,
                   'p': 2212 , 'Ne': 1000100200 , 'He': 1000020040 ,
                   'Kr': 1000360840 ,
                   'Xe': 1000541320 }

gen = Generation()

def finalConfiguration():
    event_type = gen.getProp('EventType')
    gauss = Gauss()

    gen.MinimumBias.ProductionTool = "CRMCProduction"

    horizontalCrossingAngle = gauss.getProp('BeamHCrossingAngle')
    horizontalBeamlineAngle = gauss.getProp('BeamLineAngles')[ 0 ]
    verticalCrossingAngle = gauss.getProp('BeamVCrossingAngle')
    verticalBeamlineAngle =  gauss.getProp('BeamLineAngles')[ 1 ]
    pzB = -math.fabs(gauss.getProp('B2Momentum')) / SystemOfUnits.GeV
    pxB = -pzB * math.sin( horizontalCrossingAngle - horizontalBeamlineAngle )
    pyB = -pzB * math.sin( verticalCrossingAngle - verticalBeamlineAngle )
    pzA = gauss.getProp('BeamMomentum') / SystemOfUnits.GeV
    pxA = pzA * math.sin( horizontalCrossingAngle + horizontalBeamlineAngle )
    pyA = pzA * math.sin( verticalCrossingAngle + verticalBeamlineAngle )

    gen.MinimumBias.CRMCProduction.ProjectileMomentum = pzA
    gen.MinimumBias.CRMCProduction.TargetMomentum = pzB
    gen.Special.CRMCProduction.ProjectileMomentum = pzA
    gen.Special.CRMCProduction.TargetMomentum = pzB
    gen.Inclusive.CRMCProduction.ProjectileMomentum = pzA
    gen.Inclusive.CRMCProduction.TargetMomentum = pzB
    gen.SignalPlain.CRMCProduction.ProjectileMomentum = pzA
    gen.SignalPlain.CRMCProduction.TargetMomentum = pzB


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

        signal_pid = gen.SignalPlain.getProp( 'SignalPIDList' )
        if len(signal_pid) > 0:
            gen.Special.DaughtersInLHCbKeepOnlySignal.SignalPID = math.fabs( signal_pid[ 0 ] )
        #
        gen.Special.ProductionTool = "Pythia8Production/SignalPythia8"
        gen.Special.PileUpProductionTool = "CRMCProduction"
        gen.Special.ReinitializePileUpGenerator = False

        gen.Special.addTool( Pythia8Production , name = 'SignalPythia8' )
        gen.Special.SignalPythia8.Tuning = "LHCbDefault.cmd"
        # give correct beam parameters to Pythia8
        gen.Special.SignalPythia8.Commands += [ 'Beams:pxB = %.2f' % pxB ,
                                                'Beams:pyB = %.2f' % pyB ,
                                                'Beams:pzB = %.2f' % pzB ,
                                                'Init:showProcesses = on' ]

        ## specific parameters for the generation of quarkonia or W/Z
        ## inclusive J/psi
        if event_type/1000000 == 24:
            gen.Special.SignalPythia8.Commands += [ 'SoftQCD:all=off' , 'Onia:all=on' ]
        ## other inclusive charmonium
        elif event_type/1000000 == 28:
            gen.Special.SignalPythia8.Commands += [ 'SoftQCD:all=off' , 'Onia:all=on' ]
        ## inclusive bottomonium
        elif event_type/1000000 == 18:
            signal_pid = gen.Special.UpsilonDaughtersInLHCb.getProp( 'SignalPID' )
            if signal_pid:
                gen.Special.DaughtersInLHCbKeepOnlySignal.SignalPID = math.fabs( signal_pid )
            gen.Special.SignalPythia8.Commands += [ 'SoftQCD:all=off' , 'Bottomonium:all=on' ]
        ## Z -> mu mu
        elif event_type == 42112000:
            gen.Special.DaughtersInLHCbKeepOnlySignal.SignalPID = 23
            gen.Special.SignalPythia8.Commands += [ 'SoftQCD:all=off' ,
                                                    "WeakSingleBoson:ffbar2gmZ = on", #Z0/gamma* production
                                                    "WeakZ0:gmZmode = 2", #Z0 only
                                                    "23:onMode = off", #turn it off
                                                    "23:onIfMatch = 13 -13" ] # only mu mu
        ## W -> mu nu_mu
        elif event_type == 42311000:
            gen.Special.DaughtersInLHCbKeepOnlySignal.SignalPID = 24
            gen.Special.SignalPythia8.Commands += [ 'SoftQCD:all=off' ,
                                                    "WeakSingleBoson:ffbar2W = on",
                                                    "24:onMode = off",
                                                    "24:onIfMatch = 13 -14",
                                                    "24:onIfMatch = -13 14" ]
        ## DY -> mu mu, mass > 2 GeV
        elif event_type == 42112010:
            gen.Special.DaughtersInLHCbKeepOnlySignal.SignalPID = 23
            gen.Special.SignalPythia8.Commands += [ 'SoftQCD:all=off' ,
                                                    "WeakSingleBoson:ffbar2gmZ = on"
                                                    "23:mMin = 2.",
                                                    "TimeShower:mMaxGamma = 2.",
                                                    "PhaseSpace:mHatMin = 2.",
                                                    "23:onMode = off",                # turn it off
                                                    "23:onIfMatch = 13 -13",          # decay to muon only
                                                    ]            
            
        gen.Special.CRMCProduction.ProjectileID = __ion_pdg_id__[  gauss.getProp('B1Particle') ]
        gen.Special.CRMCProduction.TargetID = __ion_pdg_id__[ gauss.getProp('B2Particle') ]
        gen.Special.CRMCProduction.ProjectileMomentum = pzA
        gen.Special.CRMCProduction.TargetMomentum =  pzB
        #
        ## set the correct beam 2 momentum in case of asymmetric beam
        if gauss.getProp('BeamMomentum') != gauss.getProp('B2Momentum'):
            gen.Special.SignalPythia8.BeamToolName = "AsymmetricCollidingBeams"
            gen.Special.SignalPythia8.addTool( AsymmetricCollidingBeams )
            gen.Special.SignalPythia8.AsymmetricCollidingBeams.Beam2Momentum = math.fabs( pzB ) * SystemOfUnits.GeV

    gen.MinimumBias.CRMCProduction.Frame = "nucleon-nucleon"
    gen.Special.CRMCProduction.Frame = "nucleon-nucleon"
    gen.Inclusive.CRMCProduction.Frame = "nucleon-nucleon"
    gen.SignalPlain.CRMCProduction.Frame = "nucleon-nucleon"

    ## then boost the EPOS interactions in the correct frame
    genSequence = GaudiSequencer( "GeneratorSlotMainSeq" )
    boost_px = (pxA + pxB) * SystemOfUnits.GeV
    boost_py = (pyA + pyB) * SystemOfUnits.GeV
    m_p =  0.938272
    boost_pz = (pzA + pzB) * SystemOfUnits.GeV
    boost_e = ( math.sqrt( m_p * m_p + pxA*pxA + pyA*pyA + pzA*pzA ) +  math.sqrt( m_p * m_p + pxB*pxB + pyB*pyB + pzB*pzB ) ) * SystemOfUnits.GeV

    boostAlg = BoostForEpos( p_x = boost_px , p_y= boost_py , p_z = boost_pz ,
                             e = boost_e )
    genSequence.Members += [ boostAlg ]

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
  "fname check none",
  "fname initl $CRMC_TABS/epos.initl",
  "fname iniev $CRMC_TABS/epos.iniev",
  "fname inirj $CRMC_TABS/epos.inirj",
  "fname inics $CRMC_TABS/epos.inics",
  "fname inihy $CRMC_TABS/epos.inihy"
]

gen.MinimumBias.CRMCProduction.SwitchOffEventTruncation = True 
gen.MinimumBias.CRMCProduction.ProduceTables = False
gen.MinimumBias.CRMCProduction.AddUserSettingsToDefault = True
gen.MinimumBias.CRMCProduction.Commands += configuration_commands

gen.Special.CRMCProduction.SwitchOffEventTruncation = True 
gen.Special.CRMCProduction.ProduceTables = False
gen.Special.CRMCProduction.AddUserSettingsToDefault = True
gen.Special.CRMCProduction.Commands += configuration_commands

gen.Inclusive.CRMCProduction.SwitchOffEventTruncation = True 
gen.Inclusive.CRMCProduction.ProduceTables = False
gen.Inclusive.CRMCProduction.AddUserSettingsToDefault = True
gen.Inclusive.CRMCProduction.Commands += configuration_commands

gen.SignalPlain.CRMCProduction.SwitchOffEventTruncation = True 
gen.SignalPlain.CRMCProduction.ProduceTables = False
gen.SignalPlain.CRMCProduction.AddUserSettingsToDefault = True
gen.SignalPlain.CRMCProduction.Commands += configuration_commands
