"""
High level and utility functions to set up the Generation step in Gaussino
"""

from Gaudi.Configuration import ConfigurableUser, Configurable, ApplicationMgr
from Gaudi.Configuration import GaudiSequencer
from GaudiKernel import SystemOfUnits
from Gaussino.GenUtils import configure_pgun, configure_generation
from Gaussino.GenUtils import configure_rnd_init, configure_gen_monitor
from Gaussino.GenUtils import configure_hepmc_writer

# from Configurables import GenerationToSimulation
# from Configurables import CheckMCStructure


class GenPhase(ConfigurableUser):

    """Configurable for the Generation phase in Gaussino. Does not implement
    a self.__apply_configuration__ itself. Instead, all member functions are
    explicitly called during the configuration of Gaussino()"""

    _production_type_map = {
        'PGUN': configure_pgun,
        'PHYS': configure_generation,
    }

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
        "evtMax"              : -1,  # NOQA
        "Production"          : 'PHYS',  # NOQA
        "WriteHepMC"          : False,  # NOQA
        "Production_kwargs"   : {}  # NOQA
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

    def configure_phase(self):
        evtMax = self.getProp('evtMax')
        if evtMax <= 0:
            raise RuntimeError("Generating events but selected '%s' events." % evtMax)  # NOQA

        # Algorithm that produces the actual HepMC by talking to stuff
        prod_name = self.getProp('Production')
        prod_kwargs = self.getProp('Production_kwargs')
        prod_alg = self._production_type_map[prod_name](**prod_kwargs)

        # Algorithm to initialise the random seeds and make a GenHeader
        rnd_init = configure_rnd_init()

        # Algorithm to make some monitoring histograms
        gen_moni = configure_gen_monitor()

        seq = GaudiSequencer('GenerationPhase')
        seq.Members = [rnd_init, prod_alg, gen_moni]
        if self.getProp('WriteHepMC'):
            seq.Members += [configure_hepmc_writer()]
        # seq.Members += [GenerationToSimulation(), CheckMCStructure()]
        ApplicationMgr().TopAlg += [seq]

    @staticmethod
    def eventType():
        from Configurables import Generation
        evtType = ''
        if Generation("Generation").isPropertySet("EventType"):
            evtType = str(Generation("Generation").EventType)
        return evtType
