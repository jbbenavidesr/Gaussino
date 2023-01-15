###############################################################################
# (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

__author__ = "Dominik Muller, Michal Mazurek, and Gloria Corti"
__email__ = "lhcb-simulation@cern.ch"

from Gaudi.Configuration import log
from GaudiKernel import SystemOfUnits

from Gaussino.Utilities import (
    GaussinoConfigurable,
    get_set_configurable,
    add_constructors_with_names,
)

# Configurables
from ParallelGeometry.Configuration import ParallelGeometry
from Gaussino.Geometry import GaussinoGeometry


class GaussinoSimulation(GaussinoConfigurable):
    """Configurable for the simulation phase.

    :var PhysicsConstructors: default: ``[]``, list of the factories used
        to attach physics to the main modular list
    :vartype PhysicsConstructors: list, required

    :var TrackTruth: default: ``True``
    :vartype TrackTruth: bool, optional

    G4 commands

    :var G4BeginRunCommand: default:
        ``["/tracking/verbose 0", "/process/eLoss/verbose 0"]``
    :vartype G4BeginRunCommand: bool, optional

    :var G4EndRunCommand: default: ``[]``
    :vartype G4EndRunCommand: bool, optional

    Cuts

    :var CutForElectron: default: ``-1. * km``
    :vartype CutForElectron: float, optional

    :var CutForGamma: default: ``-1. * km``
    :vartype CutForGamma: float, optional

    :var CutForPositron: default: ``-1. * km``
    :vartype CutForPositron: float, optional

    :var DumpCutsTable: default: ``False``
    :vartype DumpCutsTable: bool, optional
    """


    __required_configurables__ = [
        "Gaussino",
    ]

    __slots__ = {
        "TrackTruth": True,
        "G4BeginRunCommand": ["/tracking/verbose 0", "/process/eLoss/verbose 0"],
        "G4EndRunCommand": [],
        "PhysicsConstructors": [],
        "CutForElectron": -1.0 * SystemOfUnits.km,
        "CutForPositron": -1 * SystemOfUnits.km,
        "CutForGamma": -1 * SystemOfUnits.km,
        "DumpCutsTable": False,
    }

    # internal options to be set by Gaussino
    only_generation_phase = False
    redecay = False

    def __apply_configuration__(self):
        log.debug("Configuring GaussinoSimulation")
        if GaussinoSimulation.only_generation_phase:
            log.debug("-> Only the generation phase, skipping.")
            return
        self._set_giga_service()
        self._set_giga_alg()
        self._set_physics()
        self._set_truth_actions()

        # ensure GaussinoGeometry is enabled
        GaussinoGeometry()

    def _check_options_compatibility(self):
        if not self.getProp("PhysicsConstructors"):
            msg = "No physics constructors specified!"
            log.error(msg)
            raise ValueError(msg)

    def _set_giga_service(self):
        from Configurables import (
            ApplicationMgr,
            GiGaMT,
            GiGaRunActionCommand,
        )

        log.debug("-> Configuring GiGa service: GiGaMT")
        giga = GiGaMT()
        actioninit = get_set_configurable(giga, "ActionInitializer")
        actioninit.RunActions += ["GiGaRunActionCommand"]
        commands = actioninit.addTool(GiGaRunActionCommand, "GiGaRunActionCommand")
        commands.BeginOfRunCommands = self.getProp("G4BeginRunCommand")
        commands.EndOfRunCommands = self.getProp("G4EndRunCommand")
        ApplicationMgr().ExtSvc += [giga]

    def _set_giga_alg(self):
        from Configurables import ApplicationMgr

        log.debug("-> Configuring GiGa algorithm")
        if GaussinoSimulation.redecay:
            log.debug("--> Using ReDecaySimAlg")
            from Configurables import ReDecaySimAlg

            ApplicationMgr().TopAlg += [ReDecaySimAlg()]
        else:
            log.debug("--> Using GiGaAlg")
            from Configurables import GiGaAlg

            alg = GiGaAlg("GiGaAlg", Input="/Event/Gen/HepMCEvents")
            ApplicationMgr().TopAlg += [alg]

    def _set_physics(self):
        from Configurables import (
            GiGaMTModularPhysListFAC,
            GiGaMT,
        )

        log.debug("-> Configuring physics constructors")
        giga = GiGaMT()
        gmpl = giga.addTool(GiGaMTModularPhysListFAC(), name="ModularPL")
        giga.PhysicsListFactory = getattr(giga, "ModularPL")

        phys_list = self.getProp("PhysicsConstructors")
        gmpl.PhysicsConstructors = phys_list
        add_constructors_with_names(gmpl, phys_list)

        gmpl.CutForElectron = self.getProp("CutForElectron")
        gmpl.CutForGamma = self.getProp("CutForGamma")
        gmpl.CutForPositron = self.getProp("CutForPositron")
        gmpl.DumpCutsTable = self.getProp("DumpCutsTable")

        # Add parallel physics
        if ParallelGeometry().getProp("ParallelPhysics"):
            log.debug("-> Configuring physics in parallel worlds")
            ParallelGeometry().attach_physics(gmpl)

    def _set_truth_actions(self):
        if not self.getProp("TrackTruth"):
            return
        log.debug("-> Configuring truth actions")
        from Configurables import GiGaMT

        giga = GiGaMT()
        actioninit = get_set_configurable(giga, "ActionInitializer")
        actioninit.TrackingActions += [
            "TruthFlaggingTrackAction",
            "TruthStoringTrackAction",
        ]

        from Configurables import (
            TruthFlaggingTrackAction,
            TruthStoringTrackAction,
        )

        flagging = actioninit.addTool(
            TruthFlaggingTrackAction,
            "TruthFlaggingTrackAction",
        )
        storing = actioninit.addTool(
            TruthStoringTrackAction,
            "TruthStoringTrackAction",
        )
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
