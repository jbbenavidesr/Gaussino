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
    """Configurable for the Simulation phase in Gaussino.

    **Main**

    :var PhysicsConstructors: default: ``[]``, list of the factories used
        to attach physics to the main modular list
    :vartype PhysicsConstructors: list, required

    :var TrackTruth: default: ``True``
    :vartype TrackTruth: bool, optional

    :var Visualization: default: ``False``, activate various visualization
        configurables
    :vartype Visualization: bool, optional

    **G4 commands**

    :var G4BeginRunCommand: default:
        ``["/tracking/verbose 0", "/process/eLoss/verbose 0"]``
    :vartype G4BeginRunCommand: bool, optional

    :var G4EndRunCommand: default: ``[]``
    :vartype G4EndRunCommand: bool, optional

    :var G4BeginEventCommand: default: ``[]``
    :vartype G4BeginEventCommand: list, optional

    :var G4EndEventCommand: default: ``[]``
    :vartype G4EndEventCommand: list, optional

    **Cuts**

    :var CutForElectron: default: ``-1. * km``
    :vartype CutForElectron: float, optional

    :var CutForGamma: default: ``-1. * km``
    :vartype CutForGamma: float, optional

    :var CutForPositron: default: ``-1. * km``
    :vartype CutForPositron: float, optional

    :var DumpCutsTable: default: ``False``
    :vartype DumpCutsTable: bool, optional
    """

    __slots__ = {
        # MAIN
        "TrackTruth": True,
        "PhysicsConstructors": [],
        "Visualization": False,
        # G4 commands
        "G4BeginRunCommand":
        ["/tracking/verbose 0", "/process/eLoss/verbose 0"],
        "G4EndRunCommand": [],
        "G4BeginEventCommand": [],
        "G4EndEventCommand": [],
        # Cuts
        "CutForElectron": -1.0 * SystemOfUnits.km,
        "CutForPositron": -1 * SystemOfUnits.km,
        "CutForGamma": -1 * SystemOfUnits.km,
        "DumpCutsTable": False,
    }

    # internal options to be set by Gaussino
    only_generation_phase = False
    """ options set internally by Gaussino() """
    redecay = False
    """ options set internally by Gaussino() """

    def __apply_configuration__(self):
        """Main configuration method for the simulation phase.
        It applies the properties of the simulation phase right after the main
        :class:`Gaussino <Gaussino.Configuration.Gaussino>` and generation
        :class:`GaussinoGeneration <Gaussino.Generation.GaussinoGeneration>` configurable, but before
        the geometry configurable :class:`GaussinoGeometry <Gaussino.Geometry.GaussinoGeometry>`.
        """
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
        """Checks the general compatibility of the provided properties.

        Raises:
            ValueError: if no physics contructors were provided
        """
        if not self.getProp("PhysicsConstructors"):
            msg = "No physics constructors specified!"
            log.error(msg)
            raise ValueError(msg)

    def _set_giga_service(self):
        """Sets up the main simulation service ``GiGaMT``. It will be
        available throughout the whole execution time of the simulation.
        """
        from Configurables import (
            ApplicationMgr,
            GiGaMT,
            GiGaRunActionCommand,
            GiGaEventActionCommand,
        )

        log.debug("-> Configuring GiGa service: GiGaMT")
        giga = GiGaMT()
        actioninit = get_set_configurable(giga, "ActionInitializer")

        actioninit.RunActions += ["GiGaRunActionCommand"]
        run_commands = actioninit.addTool(GiGaRunActionCommand,
                                          "GiGaRunActionCommand")
        run_commands.BeginOfRunCommands = self.getProp("G4BeginRunCommand")
        run_commands.EndOfRunCommands = self.getProp("G4EndRunCommand")

        actioninit.EventActions += ['GiGaEventActionCommand']
        event_commands = actioninit.addTool(GiGaEventActionCommand)
        event_commands.BeginOfEventCommands = self.getProp(
            'G4BeginEventCommand')
        event_commands.EndOfEventCommands = self.getProp('G4EndEventCommand')

        self._activate_visualizations(giga)

        ApplicationMgr().ExtSvc += [giga]

    def _set_giga_alg(self):
        """Sets up the main simulation algorithm:

        - ``ReDecaySimAlg`` when using ReDecay,
        - ``GiGaAlg`` otherwise.

         The algorithm is executed right after the generation algorithms, but
         before hit extraction and monitoring algorithms.
        """
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
        """Sets up the physics constructors and applies the cuts.
        """
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
        """Sets up the custom optimization features that decide
        which tracks/particles should be stored and which not.
        """
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
        actioninit.addTool(
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

    def _activate_visualizations(self, giga):
        if self.getProp("Visualization"):
            from Configurables import GaussinoVisualization
            GaussinoVisualization().apply(giga)
