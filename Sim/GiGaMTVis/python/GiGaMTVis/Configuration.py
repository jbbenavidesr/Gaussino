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
from Gaudi.Configuration import (
    ConfigurableUser,
    log,
    DEBUG,
)

from GaudiKernel.SystemOfUnits import (
    MeV, )

__author__ = "Filip Bilandžija, Michał Mazurek"
__email__ = "michal.mazurek@cern.ch"


class Geant4Visualization(ConfigurableUser):
    """Documentation to be filled in later...

    :var Driver: default: ``ASCIITree`` Geant4 visualization drivers.
        The list of available drivers is : ``['ASCIITree']``
    :vartype Driver: str, required

    :var DrawGeometry: default: ``True``
    :vartype DrawGeometry: bool, optional

    :var Debug: default: ``False``
    :vartype Debug: bool, optional

    """

    __slots__ = {
        "Driver": "",
        # geometry
        "DrawGeometry": True,
        "GeometryStyle": "wireframe",
        "Volumes": [],
        "VolumesCopyNumber": {},
        "VolumesDepthOfDescent": {},
        # event data
        "DrawTrajectories": True,
        "StoreTrajectories": "Marked",
        "DrawG4Hits": True,
        "TrajectoryMinPT": 0 * MeV,
        "TrajectoryMaxPT": 0 * MeV,
        # view
        "CameraPhi": 0,  # deg,
        "CameraTheta": 0,  # deg,
        "Zoom": 1,
        # other
        "CombineEvents": True,
        "Debug": False,
    }

    _available_drivers = [
        'ASCIITree',
        'DAWNFILE',
        'OpenGLImmediateX',
        'OpenGLStoredX',
    ]

    _parsed_drivers = {
        'ASCIITree': 'ATree',
        'OpenGLImmediateX': 'OGLIX',
        'OpenGLStoredX': 'OGLSX',
    }

    _only_geometry_drivers = [
        'ASCIITree',
    ]

    # some drivers open a window
    # it might be therefore useful to open
    # an interactive UI session
    _interactive_drivers = [
        'OpenGLImmediateX',
        'OpenGLStoredX',
    ]

    _storing_trajectories = [
        "All",  # store all G4 trajectories
        "Truth",  # store G4 trajectories that correspond to MCTruth
        "Marked",  # store only G4 trajectories that create hits
    ]

    def apply(self, giga):
        """ Main method to be called from SimPhase()

        :param giga: GiGaMT configurable
        """
        driver = self.getProp("Driver")
        if not driver:
            log.info("Geant4 visualization is turned off.")
            return

        # activate GiGaVisManager
        from Configurables import GiGaVisManager
        giga.VisManager = "GiGaVisManager"
        vismgr = GiGaVisManager("GiGaMT.GiGaVisManager")
        if self.getProp("Debug"):
            vismgr.OutputLevel = DEBUG

        # access the UI interface
        actioninit = giga.ActionInitializer
        run_action = actioninit.GiGaRunActionCommand
        event_action = actioninit.GiGaEventActionCommand

        from Configurables import GiGaMTRunManagerFAC
        # this has to be accessed this way...
        run_mgr = GiGaMTRunManagerFAC("GiGaMT.GiGaMTRunManagerFAC")

        cmds = {
            # -> init commands:
            'init': run_mgr.InitCommands,
            # -> run commands:
            'begin_run': run_action.BeginOfRunCommands,
            'end_run': run_action.EndOfRunCommands,
            # -> event commands:
            'begin_event': event_action.BeginOfEventCommands,
            'end_event': event_action.EndOfEventCommands,
        }

        # and now append vis UI commands
        # note: order is important!
        self._activate_g4_driver(cmds)

        self._set_view(cmds)

        if self.getProp("DrawGeometry"):
            self._draw_geometry(cmds)

        visualize_data = self.getProp("DrawTrajectories") or self.getProp(
            "DrawG4Hits")
        if visualize_data and driver in self._only_geometry_drivers:
            if self.isPropertySet("DrawTrajectories") or self.isPropertySet(
                    "DrawG4Hits"):
                raise ValueError(
                    driver +
                    " does not support visualizing of the event data.")
            return

        if self.getProp("DrawTrajectories"):
            self._draw_trajectories(cmds, actioninit, vismgr)

        if self.getProp("DrawG4Hits"):
            self._draw_g4hits(cmds)

        self._set_other(cmds, actioninit)

    def _activate_g4_driver(self, cmds):
        driver = self.getProp("Driver")
        if driver not in self._available_drivers:
            raise NotImplementedError(driver +
                                      " is not an available G4 driver.")
        main_cmd = '/vis/open '
        parsed_driver = self._parsed_drivers.get(driver, driver)
        main_cmd += parsed_driver
        cmds['init'] += [
            main_cmd,
            '/vis/scene/create',
        ]

    def _set_view(self, cmds):
        theta = self.getProp("CameraTheta")
        phi = self.getProp("CameraPhi")
        if theta or phi:
            cmd = "/vis/viewer/set/viewpointThetaPhi {} {}".format(theta, phi)
            cmds['init'].append(cmd)
        zoom = self.getProp("Zoom")
        if zoom != 1:
            cmds['init'].append("/vis/viewer/zoom {}".format(zoom))

    def _draw_geometry(self, cmds):
        cmd = "/vis/scene/add/volume"
        vols = self.getProp("Volumes")
        for vol in vols:
            copy_no = self.getProp("VolumesCopyNumber").get(vol, -1)
            depth = self.getProp("VolumesDepthOfDescent").get(vol, -1)
            cmds['init'].append("{} {} {} {}".format(cmd, vol, copy_no, depth))
        if not vols:
            cmds['init'].append(cmd)
        cmds['init'].append("/vis/sceneHandler/attach")
        style = self.getProp("GeometryStyle")
        styles = ['wireframe', 'surface', 'cloud']
        if style not in styles:
            raise ValueError(
                "Only the following styles of geometry are available: [{}]".
                format((", ").join(styles)))
        if style == 'surface':
            cmds['init'].append("/vis/viewer/colourByDensity")
        cmds['init'].append("/vis/viewer/set/style " + style)

    def _draw_trajectories(self, cmds, actioninit, vismgr):
        cmds['init'].append("/vis/scene/add/trajectories smooth")
        # warning: adding trajectories will set storing of the trajectories
        #          by default, so additional checks have to be made if
        #          storing is to be done internally in Gaussino
        store_type = self.getProp("StoreTrajectories")
        if store_type not in self._storing_trajectories:
            raise ValueError(
                "Only the following types of trajectory storing are available: [{}]"
                .format((", ").join(self._storing_trajectories)))
        if store_type == "Marked":
            cmds['init'].append("/tracking/storeTrajectory 0")
            actioninit.TruthFlaggingTrackAction.StoreMarkedTrajectories = True
        elif store_type == "Truth":
            cmds['init'].append("/tracking/storeTrajectory 0")
            actioninit.TruthFlaggingTrackAction.StoreTrajectories = True

        # TODO: factory should be a property
        cmds['init'].append("/vis/modeling/trajectories/create/drawByCharge")

        min_init_p = self.getProp("TrajectoryMinPT")
        max_init_p = self.getProp("TrajectoryMaxPT")
        if min_init_p or max_init_p:
            from Configurables import GiGaTrajectoryInitialTransverseMomentumFilter
            vismgr.TrajectoryFactories.append(
                "GiGaTrajectoryInitialTransverseMomentumFilter")
            factory = GiGaTrajectoryInitialMomentumFilter(
                "GiGaMT.GiGaVisManager.GiGaTrajectoryInitialTransverseMomentumFilter")
            if min_init_p:
                factory.MinPT = min_init_p
            if max_init_p:
                factory.MaxPT = max_init_p
            cmds['init'].append(
                "/vis/filtering/trajectories/create/initialPTFilter")

    def _draw_g4hits(self, cmds):
        cmds['init'] += [
            "/vis/scene/add/hits",
        ]

    def _set_other(self, cmds, actioninit):
        combine = self.getProp("CombineEvents")
        driver = self.getProp("Driver")
        if combine:
            if driver == "OpenGLImmediateX":
                raise NotImplementedError(
                    "OpenGLImmediateX does not work correctly with event accumulation."
                )
            cmds['init'].append("/vis/scene/endOfEventAction accumulate")

        # for interactive drivers enable UI sessions
        if driver in self._interactive_drivers:
            if combine:
                cmds['end_run'].append("/vis/viewer/refresh")
                actioninit.GiGaRunActionCommand.EndOfRunUISession = True
            else:
                cmds['end_event'].append("/vis/viewer/refresh")
                actioninit.GiGaEventActionCommand.EndOfEventUISession = True
