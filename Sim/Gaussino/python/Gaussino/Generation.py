"""
High level and utility functions to set up the Generation step in Gaussino
"""

from Gaudi.Configuration import ConfigurableUser, Configurable, ApplicationMgr
from GaudiKernel import SystemOfUnits
from Gaudi.Configuration import log
from Gaussino.GenUtils import configure_rnd_init, configure_gen_monitor
from Gaussino.GenUtils import configure_hepmc_writer


class GenPhase(ConfigurableUser):

    """Configurable for the Generation phase in Gaussino. Does not implement
    a self.__apply_configuration__ itself. Instead, all member functions are
    explicitly called during the configuration of Gaussino()"""

    __slots__ = {
        "BeamMomentum"        : 3.5 * SystemOfUnits.TeV,  # NOQA
        "BeamHCrossingAngle"  : -0.520 * SystemOfUnits.mrad,  # NOQA
        "BeamVCrossingAngle"  : 0.0,  # NOQA
        "BeamEmittance"       : 0.0037 * SystemOfUnits.mm,  # NOQA
        "BeamBetaStar"        : 3.1 * SystemOfUnits.m,  # NOQA
        "BeamLineAngles"      : [-0.075 * SystemOfUnits.mrad, 0.035 * SystemOfUnits.mrad],  # NOQA
        "InteractionPosition" : [0.459 * SystemOfUnits.mm, -0.015 * SystemOfUnits.mm, 0.5 * SystemOfUnits.mm],  # NOQA
        "BunchRMS"            : 82.03 * SystemOfUnits.mm,  # NOQA
        "Luminosity"          : 0.247 * (10 ** 30) / (SystemOfUnits.cm2 * SystemOfUnits.s),  # NOQA
        "TotalCrossSection"   : 91.1 * SystemOfUnits.millibarn,  # NOQA
        "B2Momentum"          : 3.5 * SystemOfUnits.TeV,  # NOQA
        "B1Particle"          : 'p',  # NOQA
        "B2Particle"          : 'p',  # NOQA
        "EvtMax"              : -1,  # NOQA
        "WriteHepMC"          : False,  # NOQA
        "GenMonitor"          : False,  # NOQA
        "ParticleGun"         : False,  # NOQA
        "ParticleGunUseDefault": True,  # NOQA
        "Production_kwargs"   : {},  # NOQA
        "ConvertEDM"          : False,  # NOQA
        "SampleGenerationTool": 'SignalPlain',   # NOQA
        "SampleGenerationToolOpts": {},   # NOQA
        "PileUpTool": 'FixedLuminosityWithSvc',   # NOQA
        "ProductionTool"      : 'Pythia8Production',   # NOQA
        "DecayTool"           : '',   # NOQA
        "CutTool"    : '', # NOQA
        "CutToolOpts": {},  # NOQA
        "FullGenEventCutTool"    : '', # NOQA
        "FullGenEventCutToolOpts": {}  # NOQA
    }

    def __init__(self, name=Configurable.DefaultName, **kwargs):
        kwargs["name"] = name
        super(GenPhase, self).__init__(*(), **kwargs)

    # @brief Set the given property in another configurable object
    #  @param other The other configurable to set the property for
    #  @param name  The property name
    def setOtherProp(self, other, name):
        self.propagateProperty(name, other)

    # @brief Set the given properties in another configurable object
    #  @param other The other configurable to set the property for
    #  @param names The property names
    def setOtherProps(self, other, names):
        self.propagateProperties(names, other)

    def configure_generation(self, seq):
        # Algorithm that produces the actual HepMC by talking to stuff
        SampleGenerationTool = self.getProp('SampleGenerationTool')
        ProductionTool = self.getProp('ProductionTool')
        DecayTool = self.getProp('DecayTool')
        CutTool = self.getProp('CutTool')
        FullGenEventCutTool = self.getProp('FullGenEventCutTool')
        PileUpTool = self.getProp('PileUpTool')

        from Gaussino.Utilities import beaminfoService
        from Gaussino.Utilities import get_set_configurable
        beaminfoService()
        from Configurables import Gaussino
        if Gaussino().getProp("ReDecay"):
            from Configurables import ReDecayGeneration
            gen_alg = ReDecayGeneration()
        else:
            from Configurables import Generation
            gen_alg = Generation()
        sgt = get_set_configurable(gen_alg, 'SampleGenerationTool',
                                   SampleGenerationTool)
        sgt_opts = self.getProp('SampleGenerationToolOpts')
        for n, v in sgt_opts.items():
            sgt.setProp(n, v)
        try:
            sgt.DecayTool = DecayTool
        except:
            pass
        try:
            if CutTool != '':
                ct = get_set_configurable(sgt, 'CutTool', CutTool)
                ct_opts = self.getProp('CutToolOpts')
                for n, v in ct_opts.items():
                    ct.setProp(n, v)
            else:
                sgt.CutTool = ''
        except Exception as e:
            log.error('Could not configure CutTool', e)
        if FullGenEventCutTool != '':
            ct = get_set_configurable(gen_alg, 'FullGenEventCutTool',
                                      FullGenEventCutTool)
            ct_opts = self.getProp('FullGenEventCutToolOpts')
            for n, v in ct_opts.items():
                ct.setProp(n, v)
        else:
            gen_alg.FullGenEventCutTool = ''
        prod = get_set_configurable(sgt, 'ProductionTool',
                                    ProductionTool)
        if ProductionTool in ["Pythia8Production", "Pythia8ProductionMT"]:
            prod.BeamToolName = 'CollidingBeamsWithSvc'

        if ProductionTool == "Pythia8ProductionMT":
            from Configurables import Gaussino
            prod.NThreads = Gaussino().ThreadPoolSize

        gen_alg.PileUpTool = PileUpTool
        gen_alg.VertexSmearingTool = 'BeamSpotSmearVertexWithSvc'
        gen_alg.DecayTool = DecayTool

        seq += [gen_alg]

        # Now do it all again for the signal part
        if Gaussino().getProp("ReDecay"):
            from Configurables import ReDecaySignalGeneration
            siggen_alg = ReDecaySignalGeneration()

            siggen_alg.HepMCEventLocation = 'Gen/SignalDecayTree'
            siggen_alg.GenCollisionLocation = 'Gen/SignalCollisions'
            siggen_alg.GenHeaderOutputLocation = 'Gen/SignalGenHeader'

            seq += [siggen_alg]
            sgt = get_set_configurable(siggen_alg, 'SampleGenerationTool',
                                       'SignalPlain')
            sgt.RevertWhenBackward = False  # Don't invert in the redecay part
            siggen_alg.GenFSRLocation = ""
            sgt.GenFSRLocation = ""
            sgt_opts = self.getProp('SampleGenerationToolOpts')
            if 'SignalPIDList' in sgt_opts:
                sgt.setProp('SignalPIDList', sgt_opts['SignalPIDList'])
            else:
                # FIXME: First only support signal like org tool
                log.error("Original sample generation tool not of signal type")
            try:
                sgt.DecayTool = DecayTool
            except:
                pass
            try:
                if CutTool != '':
                    ct = get_set_configurable(sgt, 'CutTool', CutTool)
                    ct_opts = self.getProp('CutToolOpts')
                    for n, v in ct_opts.items():
                        ct.setProp(n, v)
                else:
                    sgt.CutTool = ''
            except Exception as e:
                log.error('Could not configure CutTool', e)
            if FullGenEventCutTool != '':
                ct = get_set_configurable(siggen_alg, 'FullGenEventCutTool',
                                          FullGenEventCutTool)
                ct_opts = self.getProp('FullGenEventCutToolOpts')
                for n, v in ct_opts.items():
                    ct.setProp(n, v)
            else:
                siggen_alg.FullGenEventCutTool = ''
            prod = get_set_configurable(sgt, 'ProductionTool',
                                        'ReDecayProduction')

            siggen_alg.PileUpTool = 'ReDecayPileUp'
            siggen_alg.VertexSmearingTool = ''
            siggen_alg.DecayTool = DecayTool

    def configure_pgun(self, seq):
        """Simple utility function to create and configure an instance of particle
        gun

        :**kwargs: Optional keyword arguments (not curently used)
        :returns: ParticleGun instance

        """

        from GaudiKernel.SystemOfUnits import GeV, rad
        from Configurables import ParticleGun
        pgun = ParticleGun("ParticleGun")

        if self.getProp('ParticleGunUseDefault'):
            pgun.EventType = 53210205

            from Configurables import MomentumRange
            pgun.addTool(MomentumRange, name="MomentumRange")
            pgun.ParticleGunTool = "MomentumRange"

            from Configurables import FlatNParticles
            pgun.addTool(FlatNParticles, name="FlatNParticles")
            pgun.NumberOfParticlesTool = "FlatNParticles"
            pgun.FlatNParticles.MinNParticles = 1
            pgun.FlatNParticles.MaxNParticles = 1
            pgun.MomentumRange.PdgCodes = [-2112]

            pgun.MomentumRange.MomentumMin = 2.0*GeV
            pgun.MomentumRange.MomentumMax = 100.0*GeV
            pgun.MomentumRange.ThetaMin = 0.015*rad
            pgun.MomentumRange.ThetaMax = 0.300*rad
        seq += [pgun]

    def configure_phase(self):  # NOQA
        EvtMax = self.getProp('EvtMax')
        if EvtMax <= 0:
            raise RuntimeError("Generating events but selected '%s' events." % EvtMax)  # NOQA

        seq = []
        if self.getProp('ParticleGun'):
            self.configure_pgun(seq)
        else:
            self.configure_generation(seq)

        # Algorithm to initialise the random seeds and make a GenHeader
        rnd_init = configure_rnd_init()

        seq += [rnd_init]
        if self.getProp('GenMonitor'):
            gen_moni = configure_gen_monitor()
            seq += [gen_moni]
        if self.getProp('WriteHepMC'):
            seq += [configure_hepmc_writer()]
        # seq.Members += [GenerationToSimulation(), CheckMCStructure()]
        ApplicationMgr().TopAlg += seq

    def configure_genonly(self):
        seq = []
        from Configurables import Gaussino
        if Gaussino().getProp('ReDecay'):
            from Configurables import ReDecaySkipSimAlg
            alg = ReDecaySkipSimAlg()
        else:
            from Configurables import SkipSimAlg
            alg = SkipSimAlg()
        from Gaussino.Utilities import get_set_configurable
        tool = get_set_configurable(alg, 'HepMCConverter')
        try:
            tool.CheckParticle = False
        except:
            pass
        seq += [alg]

        ApplicationMgr().TopAlg += seq

    @staticmethod
    def eventType():
        from Configurables import Generation
        evtType = ''
        if Generation("Generation").isPropertySet("EventType"):
            evtType = str(Generation("Generation").EventType)
        return evtType
