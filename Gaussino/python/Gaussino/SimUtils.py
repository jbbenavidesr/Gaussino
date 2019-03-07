from __future__ import print_function
from Utilities import get_set_configurable


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

def append_truth_actions(**kwargs):
    """Simple utility function to create and configure a GiGaAlg instance

    :**kwargs: Optional keyword arguments (not curently used)
    :returns: GenMonitorAlg instance

    """
    from Configurables import GiGaMT
    giga = GiGaMT()
    actioninit = get_set_configurable(giga, 'ActionInitializer')
    if not hasattr(actioninit, 'TrackingActions'):
        actioninit.TrackingActions = []
    actioninit.TrackingActions += ["TruthFlaggingTrackAction",
                                   "TruthStoringTrackAction"]
    if 'OutputLevel' in kwargs:
        from Configurables import TruthFlaggingTrackAction
        from Configurables import TruthStoringTrackAction
        actioninit.addTool(TruthFlaggingTrackAction, "TruthFlaggingTrackAction").OutputLevel = kwargs['OutputLevel']
        actioninit.addTool(TruthStoringTrackAction, "TruthStoringTrackAction").OutputLevel = kwargs['OutputLevel']

