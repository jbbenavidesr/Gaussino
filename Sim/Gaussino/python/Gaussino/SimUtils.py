###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from __future__ import print_function
from Gaussino.Utilities import get_set_configurable
from GaudiKernel import SystemOfUnits


def configure_giga_alg(**kwargs):
    """Simple utility function to create and configure a GiGaAlg instance

    :**kwargs: Optional keyword arguments (not curently used)
    :returns: GenMonitorAlg instance

    """
    from Configurables import GiGaAlg
    from Configurables import Gaussino
    if Gaussino().getProp('ReDecay'):
        from Configurables import ReDecaySimAlg
        return ReDecaySimAlg()

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
    from Configurables import TruthFlaggingTrackAction
    from Configurables import TruthStoringTrackAction
    flagging = actioninit.addTool(
        TruthFlaggingTrackAction,
        "TruthFlaggingTrackAction")
    storing = actioninit.addTool(
        TruthStoringTrackAction,
        "TruthStoringTrackAction")
    flagging.StoreAll = False
    flagging.StorePrimaries = True
    # FIXME: This option does not exist anymore, sensdet supposed to set the
    # flag for storing directly
    # flagging.StoreMarkedTracks = True
    flagging.StoreForcedDecays = True
    flagging.StoreByOwnEnergy = True
    flagging.OwnEnergyThreshold = 100.0 * SystemOfUnits.MeV

    flagging.StoreByChildProcess = True
    flagging.StoredChildProcesses = ["RichG4Cerenkov", "Decay"]
    flagging.StoreByOwnProcess = True
    flagging.StoredOwnProcesses = ["Decay"]

    if 'OutputLevel' in kwargs:
        flagging.OutputLevel = kwargs['OutputLevel']
        storing.OutputLevel = kwargs['OutputLevel']
