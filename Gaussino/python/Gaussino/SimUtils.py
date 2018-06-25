from __future__ import print_function


def configure_giga_alg(**kwargs):
    """Simple utility function to create and configure a GiGaAlg instance

    :**kwargs: Optional keyword arguments (not curently used)
    :returns: GenMonitorAlg instance

    """
    from Configurables import GiGaAlg

    TESLocation = "/Event/Gen/HepMCEvents"
    return GiGaAlg(
        "GiGaAlg",
        Input=TESLocation
        )
