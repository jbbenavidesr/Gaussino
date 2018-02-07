def configure_pgun(**kwargs):
    """Simple utility function to create and configure an instance of particle
    gun

    :**kwargs: Optional keyword arguments (not curently used)
    :returns: ParticleGun instance

    """

    from GaudiKernel.SystemOfUnits import GeV, rad
    from Configurables import ParticleGun
    pgun = ParticleGun("ParticleGun")
    pgun.EventType = 53210205

    from Configurables import MomentumRange
    pgun.addTool(MomentumRange, name="MomentumRange")
    pgun.ParticleGunTool = "MomentumRange"

    from Configurables import FlatNParticles
    pgun.addTool(FlatNParticles, name="FlatNParticles")
    pgun.NumberOfParticlesTool = "FlatNParticles"
    pgun.MomentumRange.PdgCodes = [-13, 13]

    pgun.MomentumRange.MomentumMin = 200.0*GeV
    pgun.MomentumRange.MomentumMax = 300.0*GeV
    pgun.MomentumRange.ThetaMin = 0.015*rad
    pgun.MomentumRange.ThetaMax = 0.300*rad
    return pgun


def configure_generation(**kwargs):
    """Simple utility function to create and configure a Generation instance

    :**kwargs: Optional keyword arguments (not curently used)
    :returns: Generation instance

    """

    from Configurables import Generation, MinimumBias, Pythia8Production
    from Configurables import CollidingBeamsWithSvc
    from .Utilities import beaminfoService
    gen = Generation()
    # Only configure BeamInfoSvc here as pgun won't need it
    beaminfoService()

    mbias = gen.addTool(MinimumBias, name="MinimumBias")
    mbias.CutTool = ""
    pprod = gen.MinimumBias.addTool(Pythia8Production, name="Pythia8Production")
    gen.MinimumBias.ProductionTool = "Pythia8Production"
    pprod.addTool(CollidingBeamsWithSvc, name="CollidingBeamsWithSvc")
    pprod.BeamToolName = 'CollidingBeamsWithSvc'

    from Configurables import PoissonPileUp
    gen.addTool(PoissonPileUp, name='PoissonPileUp')
    gen.PoissonPileUp.PileUpNu = 1
    gen.PileUpTool = 'PoissonPileUp'
    gen.VertexSmearingTool = 'BeamSpotSmearVertexWithSvc'

    gen.DecayTool = ""
    gen.MinimumBias.DecayTool = ""

    return gen


def configure_rnd_init(**kwargs):
    """Simple utility function to create and configure an instance GenRndInit

    :**kwargs: Optional keyword arguments (not curently used)
    :returns: GenRndInit

    """

    from Configurables import GenRndInit
    _name = kwargs['name'] if 'name' in kwargs else 'GenRndInit'

    from Configurables import SeedingTool
    GenRndInit(_name).addTool(SeedingTool, name='SeedingTool')
    GenRndInit(_name).RndInitToolName = 'SeedingTool'
    return GenRndInit(_name)


def configure_gen_monitor(**kwargs):
    """Simple utility function to create and configure a GenMonitorAlg instance

    :**kwargs: Optional keyword arguments (not curently used)
    :returns: GenMonitorAlg instance

    """
    from Configurables import GenMonitorAlg

    TESLocation = "/Event/Gen/HepMCEvents"
    return GenMonitorAlg(
        "GenMonitorAlg",
        HistoProduce=True,
        Input=TESLocation
        )
