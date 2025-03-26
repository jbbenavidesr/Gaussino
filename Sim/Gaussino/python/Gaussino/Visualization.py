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
import copy

from Gaudi.Configuration import DEBUG, log
from GaudiKernel import SystemOfUnits as units
from Gaussino.Utilities import GaussinoConfigurable

__author__ = "Filip Bilandžija, Michał Mazurek"
__email__ = "michal.mazurek@cern.ch"


class GaussinoVisualization(GaussinoConfigurable):
    """This class sets up and provides all the necessary tools that are needed
    for visualization and filtering of geometry, tracks and hits in Geant4. It
    also provides presets for some trajectory models, while retaining the option
    for users to build their own custom trajectory model.

    :var Framework: default: ``[]``
        Framework for the visualization. Options are ``"Geant4"`` and ``"Phoenix"`.
    :vartype Framework: list, required

    :var Driver: default: ``ASCIITree``,
        Geant4 visualization drivers. The list of available drivers is:
            - ``'ASCIITree'``,
            - ``'DAWNFILE'``,
            - ``'HepRep'``,
            - ``'OpenGLStoredX'``,
            - ``'OpenGLImmediateX'``
    :vartype Driver: str, required

    :var DrawGeometry: default: ``True``,
        Boolean flag for drawing geometry in visualization.
    :vartype DrawGeometry: bool, optional

    :var DrawTrajectories: default: ``True``,
        Boolean flag for drawing trajectories in visualization.
    :vartype DrawTrajectories: bool, optional

    :var DrawG4Hits: default: ``False``,
        Boolean flag for drawing hits in visualization.
    :vartype DrawG4Hits: bool, optional

    :var TrajectoryType: default: ``""``,
        Defines trajectory type in visualization.
        The list of available trajectory types is:
            - ``"smooth"``,
            - ``"rich"``,
            - ``""``
    :vartype TrajectoryType: string, optional

    :var TrajectoryModel: default: ``"drawByParticleID"``,
        Defines trajectory model to draw by in visualization.
        It takes effect only if ``DrawTrajectories`` is set to ``True``.
        The list of available trajectory models is:
            - ``'drawByCharge'``,
            - ``'drawByEncounteredVolume'``,
            - ``'drawByOriginVolume'``,
            - ``'drawByParticleID'``,
            - ``'generic'``,
            - ``'drawByMomentum'``,
            - ``'drawByKineticEnergy'``,
            - ``'customTrajectoryModel'``
    :vartype TrajectoryModel: string, optional

    :var TrajectoryModelOptions: default ``{}``,
        Defines trajectory model options to provide -
        both for modification of default trajectory models
        or making your custom trajectory model.
    :vartype TrajectoryModelOptions: dict, optional

    :var TrajectoryFilters: default: ``[]``
        Defines trajectory filter models to filter
        by in visualization. It takes effect only if
        ``DrawTrajectories`` is set to to ``True``.
    :vartype TrajectoryFilters: list, optional

    :var StoreTrajectories: default: ``"Marked"``
        Defines which trajectories to store.
        The list of available trajectory store options is:
        ``["All", "Marked", "Truth"]``. The option ``"Marked"``
        will store the trajectories which produce hits in
        simulations, option ``"Truth"`` will store trajectories
        that correspond to MCTruth, while option ``"All"`` will
        store all the trajectories.
    :vartype StoreTrajectories: string, optional

    :var GeometryStyle: default: ``"wireframe"``
        Defines geometry style.
        List of available geometry styles is:
            - ``"wireframe"``,
            - ``"surface"``,
            - ``"cloud"``
    :vartype GeometryStyle: string, optional

    :var ElectricField: default: ``{}``
        Enables visualization of the electric field
        and defines it's properties.
    :vartype ElectricField: dict, optional

    :var MagneticField: default: ``{}``
        Enables visualization of the magnetic field
        and defines it's properties.
    :vartype MagneticField: dict, optional

    :var TrackingVerbosity: default: ``0``
    :vartype TrackingVerbosity: int, optional

    :var EventVerbosity: default: ``0``
    :vartype EventVerbosity: int, optional

    :var RunVerbosity: default: ``0``
    :vartype RunVerbosity: int, optional

    :var OGLExportFileName: default: ``""``
    :vartype OGLExportFileName: string, optional

    :var Debug: default: ``False``
    :vartype Debug: bool, optional

    :Example:

        .. highlight:: python
        .. code-block:: python

            # Initialize visualization and choose
            # the Geant4 driver and storage options
            from Configurables import GaussinoVisualization
            g4vis = GaussinoVisualization()
            g4vis.Driver = "OpenGLStoredX"
            g4vis.StoreTrajectories = "All"

            # Enable drawing of geometry and trajectories
            # and choose trajectory type
            g4vis.DrawGeometry = True
            g4vis.DrawTrajectories = True
            g4vis.TrajectoryType = "smooth"

            # Choose trajectory model and provide options to the model
            # (if you're not satisfied with default options)
            g4vis.TrajectoryModel = "drawByMomentum"
            g4vis.TrajectoryModelOptions = {
                "Intervals": [
                    'interval1 0.0 keV 20 MeV',
                    'interval2 20 MeV 500 MeV',
                    'interval3 500 MeV 2.5 GeV',
                    'interval4 2.5 GeV 25 GeV',
                    'interval5 25 GeV 1 TeV'
                ],
                "LineColors": [
                    'interval1 green',
                    'interval2 cyan',
                    'interval3 red',
                    'interval4 magenta'
                ]
            }

            # Choose trajectory filters and provide options for them
            g4vis.TrajectoryFilters = [{
                "FilterType": "transverseMomentumFilter",
                "MinValue": 3 * MeV
            }, {
                "FilterType": "chargeFilter",
                "Options": "addPositiveChargeParticles",
                "IsInclusive": False
            }]
    """

    __slots__ = {
        "Framework": [],
        "Driver": "ASCIITree",
        # geometry
        "DrawGeometry": False,
        # verbosity
        "TrackingVerbosity": 0,
        "EventVerbosity": 0,
        "RunVerbosity": 0,
        # Volumes
        "GeometryStyle": "wireframe",
        "Volumes": [],
        "VolumesCopyNumber": {},
        "VolumesDepthOfDescent": {},
        # event data
        "DrawTrajectories": False,
        "TrajectoryType": "",
        "TrajectoryModel": "",
        # custom trajectory model options
        "TrajectoryModelOptions": {},
        # trajectory filters
        "TrajectoryFilters": [],
        # store trajectories
        "StoreTrajectories": "Marked",
        "DrawG4Hits": False,
        # view
        "CameraPhi": 0,  # deg,
        "CameraTheta": 0,  # deg,
        "Zoom": 1,
        # OpenGLSettings
        "OGLExportFileName": "",
        # Phoenix settings:
        "PhoenixOutputFile": "gaussino_phoenix.json",
        # other
        "CombineEvents": True,
        "Debug": False,
        # fields
        "ElectricField": {},
        "MagneticField": {},
    }

    _available_drivers = [
        "ASCIITree",
        "DAWNFILE",
        "HepRep",
        "OpenGLImmediateX",
        "OpenGLStoredX",
    ]

    _parsed_drivers = {
        "ASCIITree": "ATree",
        "DAWN": "DAWNFILE",
        "HepRep": "HepRepFile",
        "OpenGLImmediateX": "OGLIX",
        "OpenGLStoredX": "OGLSX",
    }

    _only_geometry_drivers = [
        "ASCIITree",
    ]

    # some drivers open a window
    # it might be therefore useful to open
    # an interactive UI session
    _interactive_drivers = [
        "OpenGLImmediateX",
        "OpenGLStoredX",
    ]

    # Available trajectory models:
    _available_g4_trajectory_models = [
        "customTrajectoryModel",
        "drawByCharge",
        "drawByEncounteredVolume",
        "drawByOriginVolume",
        "drawByParticleID",
        "generic",
        "drawByMomentum",
        "drawByKineticEnergy",
    ]

    _default_trajectory_models = [
        "drawByCharge",
        "drawByEncounteredVolume",
        "drawByOriginVolume",
        "drawByParticleID",
        "generic",
    ]

    _phoenix_trajectory_models = [
        "drawByParticleID",
        "drawByMomentum",
        "drawByKineticEnergy",
        "drawByCharge",
    ]

    # Available trajectory filters
    _available_g4_trajectory_filters = [
        "chargeFilter",
        "customTrajectoryFilter",
        "encounteredVolumeFilter",
        "originVolumeFilter",
        "particleFilter",
        "momentumMagnitudeFilter",
        "kineticEnergyFilter",
        "transverseMomentumFilter",
    ]

    # Default trajectory filters
    _default_trajectory_filters = [
        "chargeFilter",
        "encounteredVolumeFilter",
        "originVolumeFilter",
        "particleFilter",
        "attributeFilter",
    ]

    _phoenix_trajectory_filters = [
        "chargeFilter",
        "momentumMagnitudeFilter",
        "kineticEnergyFilter",
        "transverseMomentumFilter",
        "pseudorapidityFilter",
    ]

    _storing_trajectories = [
        "All",  # store all G4 trajectories
        "Truth",  # store G4 trajectories that correspond to MCTruth
        "Marked",  # store only G4 trajectories that create hits
    ]

    _filter_keyname = {
        "chargeFilter": "gaussinoCF",
        "particleFilter": "gaussinoPF",
        "originVolumeFilter": "gaussinoOVF",
        "attributeFilter": "gaussinoAF",
        "generic": "gaussinoGF",
        "momentumMagnitudeFilter": "gaussinoIMagF",
        "kineticEnergyFilter": "gaussinoIKEF",
        "transverseMomentumFilter": "gaussinoPTF",
        "pseudorapidityFilter": "gaussinoPF",
        "customTrajectoryFilter": "gaussinoCTF",
    }

    chargeFilterPresets = {
        "addPositiveChargeParticles",
        "addNegativeChargeParticles",
        "addNeutralParticles",
    }

    _default_trajectory_model_options = {
        "ModelName": "drawByParticleID",
        "Verbose": False,
        "Values": [],
        "Attribute": "",
        "Intervals": [],
        "LineColors": [],
        "RGBAColors": [],
        "DrawStepPts": True,
        "StepPtsSize": 2,
    }

    _default_trajectory_filter_options = {
        "FilterName": "",
        "FilterType": "",
        "Verbose": False,
        "Values": [],
        "Attribute": "",
        "Intervals": [],
        "Invert": False,
    }

    _default_field_visualization = {
        "IsVisible": False,
        "N": 0,
        "Representation": "fullArrow",
    }

    _gaussino_colors_hex = {
        "red": "FF0000",
        "green": "00FF00",
        "blue": "0000FF",
        "cyan": "00FFFF",
        "magenta": "FF00FF",
        "orange": "FFAA00",
        "yellow": "FFFF00",
        "purple": "AA22FF",
    }

    _visualization_model = {}

    _visualization_filters = []

    def __apply_configuration__(self):
        frameworks = self.getProp("Framework")
        if not frameworks:
            return

        from Configurables import GiGaMT, GiGaMTRunManagerFAC

        giga = GiGaMT()
        actioninit = giga.ActionInitializer
        run_action = actioninit.GiGaRunActionCommand
        event_action = actioninit.GiGaEventActionCommand
        # this has to be accessed this way...
        run_mgr = GiGaMTRunManagerFAC("GiGaMT.GiGaMTRunManagerFAC")

        cmds = {
            # -> init commands:
            "init": run_mgr.InitCommands,
            # -> run commands:
            "begin_run": run_action.BeginOfRunCommands,
            "end_run": run_action.EndOfRunCommands,
            # -> event commands:
            "begin_event": event_action.BeginOfEventCommands,
            "end_event": event_action.EndOfEventCommands,
        }

        if "Geant4" in frameworks:
            self._apply_g4(giga, cmds, actioninit)
        if "Phoenix" in frameworks:
            self._apply_phoenix(cmds, actioninit)

    def _apply_phoenix(self, cmds, actioninit):
        from Configurables import LHCb__Phoenix__Sink

        sink = LHCb__Phoenix__Sink()
        sink.FileName = self.getProp("PhoenixOutputFile")

        from Configurables import ApplicationMgr

        appMgr = ApplicationMgr()
        appMgr.ExtSvc.append(sink)

        from Configurables import GiGaPhoenixDumpG4Trajectories

        dumpg4traj = GiGaPhoenixDumpG4Trajectories()

        if self.getProp("DrawTrajectories"):
            self._set_trajectory_storage(cmds, actioninit)
            # phoenix vis without Geant4 vis in gaussino doesn't support smooth or rich traj yet
            dumpg4traj.TrajectoryType = ""
            if "Geant4" in self.getProp("Framework"):
                dumpg4traj.TrajectoryType = self.getProp("TrajectoryType")
            self._set_phoenix_trajectory_model(dumpg4traj)
            self._set_phoenix_trajectory_filters(dumpg4traj)

        appMgr.TopAlg.append(dumpg4traj)

    def _apply_g4(self, giga, cmds, actioninit):
        driver = self.getProp("Driver")
        if self.getProp("Framework") == "Phoenix":
            driver = "OpenGLStoredX"
        if not driver:
            log.info("Geant4 visualization is turned off.")
            return

        # activate GiGaVisManager
        from Configurables import GiGaVisManager

        giga.VisManager = "GiGaVisManager"
        vismgr = GiGaVisManager("GiGaMT.GiGaVisManager")
        if self.getProp("Debug"):
            vismgr.OutputLevel = DEBUG

        vismgr.RequiredDriver = driver
        # access the UI interface

        # and now append vis UI commands
        # note: order is important!

        self._activate_g4_driver(cmds)

        self._activate_g4_fields(cmds)

        self._set_view(cmds)

        self._set_verbosity(cmds)

        visualize_data = self.getProp("DrawTrajectories") or self.getProp("DrawG4Hits")
        if visualize_data and driver in self._only_geometry_drivers:
            if self.isPropertySet("DrawTrajectories") or self.isPropertySet(
                "DrawG4Hits"
            ):
                raise ValueError(
                    driver + " does not support visualizing of the event data."
                )
            return

        if self.getProp("DrawGeometry"):
            self._draw_geometry(cmds)

        if self.getProp("DrawTrajectories"):
            self._set_trajectory_storage(cmds, actioninit)
            self._draw_g4_trajectories(cmds, actioninit)
            if self.isPropertySet("TrajectoryModel"):
                self._create_g4_trajectory_model()
                self._set_g4_trajectory_model(cmds)
            if self.isPropertySet("TrajectoryFilters"):
                self._create_g4_trajectory_filters()
                self._set_g4_trajectory_filters(cmds, vismgr)

        if self.getProp("DrawG4Hits"):
            self._draw_g4hits(cmds)

        self._set_other(cmds, actioninit)

    def _set_ui_command(self, cmds, ui_command, command_argument="", phase="init"):
        cmds[phase].append("{} {}".format(ui_command, command_argument))

    def _find_color_hex(self, color_key):
        if color_key in self._gaussino_colors_hex.keys():
            return self._gaussino_colors_hex[color_key]
        return color_key

    def _unit_str_conversion(self, unit):
        if unit == "eV":
            return 1e-6 * units.MeV
        if unit == "keV":
            return 1e-3 * units.MeV
        if unit == "MeV":
            return units.MeV
        if unit == "GeV":
            return 1e3 * units.MeV
        if unit == "TeV":
            return 1e6 * units.MeV
        if unit == "PeV":
            return 1e9 * units.MeV
        raise NotImplementedError("Unit " + unit + " is not supported.")

    def _set_trajectory_storage(self, cmds, actioninit):
        store_type = self.getProp("StoreTrajectories")
        if store_type not in self._storing_trajectories:
            raise ValueError(
                "Only the following types of trajectory storing are available: [{}]".format(
                    (", ").join(self._storing_trajectories)
                )
            )
        if store_type == "Marked":
            self._set_ui_command(cmds, "/tracking/storeTrajectory", 0)
            actioninit.TruthFlaggingTrackAction.StoreMarkedTrajectories = True
        elif store_type == "Truth":
            self._set_ui_command(cmds, "/tracking/storeTrajectory", 0)
            actioninit.TruthFlaggingTrackAction.StoreTrajectories = True
        else:
            self._set_ui_command(cmds, "/tracking/storeTrajectory", 1)

    def _activate_g4_driver(self, cmds):
        """Method to activate on of the available Geant4 drivers

        :param cmds: List of visualization commands
        """
        driver = self.getProp("Driver")
        if driver not in self._available_drivers:
            raise NotImplementedError(driver + " is not an available G4 driver.")
        parsed_driver = self._parsed_drivers.get(driver, driver)
        self._set_ui_command(cmds, "/vis/open", f"{parsed_driver} 1500x1500+600+400")
        self._set_ui_command(cmds, "/vis/scene/create")
        # '/vis/sceneHandler/create {}'.format(parsed_driver)

    def _set_view(self, cmds):
        """Method to set viewpoint coordinates (theta and phi) and zoom

        :param cmds: List of visualization commands
        """
        theta = self.getProp("CameraTheta")
        phi = self.getProp("CameraPhi")
        if theta or phi:
            self._set_ui_command(
                cmds, "/vis/viewer/set/viewpointThetaPhi", "{} {}".format(theta, phi)
            )

        zoom = self.getProp("Zoom")
        if zoom != 1:
            self._set_ui_command(cmds, "/vis/viewer/zoom", zoom)

    def _draw_geometry(self, cmds):
        """Method to add the commands for visualization of geometry and attach it to the scene

        :param cmds: List of visualization commands
        """
        cmd = "/vis/scene/add/volume"
        vols = self.getProp("Volumes")
        for vol in vols:
            copy_no = self.getProp("VolumesCopyNumber").get(vol, -1)
            depth = self.getProp("VolumesDepthOfDescent").get(vol, -1)
            self._set_ui_command(cmds, cmd, "{} {} {}".format(vol, copy_no, depth))
        if not vols:
            self._set_ui_command(cmds, cmd)
        self._set_ui_command(cmds, "/vis/sceneHandler/attach")

        style = self.getProp("GeometryStyle")
        styles = ["wireframe", "surface", "cloud"]
        if style not in styles:
            raise ValueError(
                "Only the following styles of geometry are available: [{}]".format(
                    (", ").join(styles)
                )
            )
        if style == "surface":
            self._set_ui_command(cmds, "/vis/viewer/colourByDensity")
        self._set_ui_command(cmds, "/vis/viewer/set/style", style)

    def _set_verbosity(self, cmds):
        """Method to set the verbosity of tracking, event and run

        :param cmds: List of visualization commands
        """
        if self.isPropertySet("TrackingVerbosity"):
            tracking_verbosity = self.getProp("TrackingVerbosity")
            self._set_ui_command(
                cmds, "/tracking/verbose", tracking_verbosity, "begin_run"
            )

        if self.isPropertySet("EventVerbosity"):
            event_verbosity = self.getProp("EventVerbosity")
            self._set_ui_command(cmds, "/event/verbose", event_verbosity, "begin_run")

        if self.isPropertySet("RunVerbosity"):
            run_verbosity = self.getProp("RunVerbosity")
            self._set_ui_command(cmds, "/run/verbose", run_verbosity, "begin_run")

    def _draw_g4_trajectories(self, cmds, actioninit):
        """Method to configure trajectory visualization and it's properties:
            trajectory type, storing of trajectories and geometry style

        :param cmds: List of visualization commands
        :param actioninit: Action parameter
        """
        self._set_ui_command(
            cmds, "/vis/scene/add/trajectories", self.getProp("TrajectoryType")
        )
        # warning: adding trajectories will set storing of the trajectories
        #          by default, so additional checks have to be made if
        #          storing is to be done internally in Gaussino
        self._set_trajectory_storage(cmds, actioninit)

    def _create_g4_trajectory_model(self):
        """Method for creating the trajectory model to be used in ``_set_trajectory_model``"""
        trajectory_model_name = self.getProp("TrajectoryModel")
        trajectory_model_options = self.getProp("TrajectoryModelOptions")
        trajectory_model = (
            self._default_trajectory_model_options | trajectory_model_options
        )

        if trajectory_model_name not in self._available_g4_trajectory_models:
            raise NotImplementedError(
                trajectory_model_name + " is not an available G4 Trajectory Model."
            )

        if trajectory_model_name in self._default_trajectory_models:
            trajectory_model["ModelName"] = trajectory_model_name

        elif (
            trajectory_model_name == "drawByMomentum"
            or trajectory_model_name == "drawByKineticEnergy"
        ):
            trajectory_model["ModelName"] = "drawByAttribute"
            if trajectory_model_name == "drawByMomentum":
                trajectory_model["Attribute"] = "IMag"
            else:
                trajectory_model["Attribute"] = "IKE"
            if (
                not trajectory_model_options
                or not trajectory_model_options["Intervals"]
                or not len(trajectory_model_options["Intervals"])
            ):
                trajectory_model["Intervals"] = [
                    "interval1 0.0 keV 10 MeV",
                    "interval2 10 MeV 250 MeV",
                    "interval3 250 MeV 1 GeV",
                    "interval4 1 GeV 10 GeV",
                    "interval5 10 GeV 1 TeV",
                ]
            if (
                not trajectory_model_options
                or not trajectory_model_options["LineColors"]
                or not len(trajectory_model_options["LineColors"])
            ):
                trajectory_model["LineColors"] = [
                    "interval1 cyan",
                    "interval2 orange",
                    "interval3 green",
                    "interval4 blue",
                    "interval5 red",
                ]

        elif trajectory_model_name == "customTrajectoryModel":
            trajectory_model["ModelName"] = "drawByAttribute"

        for key in trajectory_model.keys():
            self._visualization_model[key] = trajectory_model[key]

    def _set_g4_trajectory_model(self, cmds):
        """Method to set the trajectory visualization model

        :param cmds: List of visualization commands
        """
        trajectory_model = self._visualization_model
        cmd_preset = "/vis/modeling/trajectories"
        traj_model_key = "trajModel"
        cmd_with_traj_key = "{}/{}".format(cmd_preset, traj_model_key)

        self._set_ui_command(
            cmds,
            cmd_preset + "/create/" + trajectory_model["ModelName"],
            traj_model_key,
        )
        self._set_ui_command(
            cmds,
            cmd_with_traj_key + "/default/setDrawStepPts",
            trajectory_model["DrawStepPts"],
        )
        self._set_ui_command(
            cmds,
            cmd_with_traj_key + "/default/setStepPtsSize",
            trajectory_model["StepPtsSize"],
        )

        if trajectory_model["ModelName"] == "drawByAttribute":
            if trajectory_model["Attribute"]:
                self._set_ui_command(
                    cmds,
                    cmd_with_traj_key + "/setAttribute",
                    trajectory_model["Attribute"],
                )

            if trajectory_model["Values"]:
                for value in trajectory_model["Values"]:
                    self._set_ui_command(cmds, cmd_with_traj_key + "/addValue", value)

            if trajectory_model["Intervals"]:
                for interval in trajectory_model["Intervals"]:
                    self._set_ui_command(
                        cmds, cmd_with_traj_key + "/addInterval", interval
                    )

            if trajectory_model["LineColors"]:
                for color in trajectory_model["LineColors"]:
                    [key, color] = color.split(" ", 1)
                    self._set_ui_command(
                        cmds, cmd_with_traj_key + "/{}/setLineColour".format(key), color
                    )

            if trajectory_model["RGBAColors"]:
                for color in trajectory_model["RGBAColors"]:
                    [key, color] = color.split(" ", 1)
                    self._set_ui_command(
                        cmds,
                        cmd_with_traj_key + "/{}/setLineColourRGBA".format(key),
                        color,
                    )

    def _set_phoenix_trajectory_model(self, dumpg4traj):
        if self.getProp("TrajectoryModel") in self._phoenix_trajectory_models:
            dumpg4traj.TrajectoryModel = self.getProp("TrajectoryModel")

            if self.isPropertySet("TrajectoryModelOptions"):
                model_options = self.getProp("TrajectoryModelOptions")

                if "LineColors" in model_options.keys():
                    if self.getProp("TrajectoryModel") == "drawByCharge":
                        charge_colors = {
                            "Positive": "0000FF",
                            "Negative": "FF0000",
                            "Neutral": "00FF00",
                        }
                        for color_str in model_options["LineColors"]:
                            charge, color_val = color_str.split(" ", 1)
                            charge_colors[charge] = self._find_color_hex(color_val)
                        dumpg4traj.ChargeColors = charge_colors

                    elif self.getProp("TrajectoryModel") == "drawByParticleID":
                        particle_colors = {
                            "gamma": "00FF00",
                            "e-": "FF0000",
                            "e+": "0000FF",
                            "pi-": "FF00FF",
                            "pi+": "FF00FF",
                            "proton": "00FFFF",
                        }
                        for color_str in model_options["LineColors"]:
                            particle, color_val = color_str.split(" ", 1)
                            particle_colors[particle] = self._find_color_hex(color_val)
                        dumpg4traj.ParticleIDColors = particle_colors

                    else:
                        intervals = {}
                        interval_colors = {}
                        for interval_str in model_options["Intervals"]:
                            (
                                interval_key,
                                lower_bound,
                                lower_bound_unit,
                                upper_bound,
                                upper_bound_unit,
                            ) = interval_str.split(" ", 4)
                            intervals[interval_key] = (
                                float(lower_bound)
                                * self._unit_str_conversion(lower_bound_unit),
                                float(upper_bound)
                                * self._unit_str_conversion(upper_bound_unit),
                            )
                        for color_str in model_options["LineColors"]:
                            interval_key, color = color_str.split(" ", 2)
                            interval_colors[self._find_color_hex(color)] = intervals[
                                interval_key
                            ]
                        dumpg4traj.IntervalColors = interval_colors

        else:
            raise NotImplementedError(
                "Trajectory model '{}' is not implemented in Phoenix.".format(
                    self.getProp("TrajectoryModel")
                )
            )

    def _create_g4_trajectory_filters(self):
        """Method for creating trajectory filters to be used in ``_set_trajectory_filters``"""
        trajectory_filters = self.getProp("TrajectoryFilters")

        for filter_options in trajectory_filters:
            if (
                "FilterType" not in filter_options.keys()
                or not filter_options["FilterType"]
            ):
                raise ValueError("Filter Type is not provided.")

            if (
                filter_options["FilterType"]
                not in self._available_g4_trajectory_filters
            ):
                raise NotImplementedError(
                    filter_options["FilterType"]
                    + " is not an available G4 Trajectory Filter."
                )
            g4filter = copy.deepcopy(self._default_trajectory_filter_options)
            g4filter["FilterType"] = filter_options["FilterType"]

            if "FilterName" in filter_options.keys() and filter_options["FilterName"]:
                g4filter["FilterName"] = filter_options["FilterName"]
            else:
                g4filter["FilterName"] = self._filter_keyname[
                    filter_options["FilterType"]
                ]

            if "IsInclusive" in filter_options.keys():
                g4filter["Invert"] = not filter_options["IsInclusive"]

            if filter_options["FilterType"] == "chargeFilter":
                if "Values" in filter_options.keys():
                    g4filter["Values"] = filter_options["Values"]

                if "Options" in filter_options.keys():
                    if "addPositiveChargeParticles" in filter_options["Options"]:
                        g4filter["Values"].append(1)
                    if "addNegativeChargeParticles" in filter_options["Options"]:
                        g4filter["Values"].append(-1)
                    if "addNeutralParticles" in filter_options["Options"]:
                        g4filter["Values"].append(0)

            if filter_options["FilterType"] == "particleFilter":
                g4filter["Values"] = filter_options["Particles"]

            if (
                filter_options["FilterType"] == "momentumMagnitudeFilter"
                or filter_options["FilterType"] == "kineticEnergyFilter"
            ):
                g4filter["FilterType"] = "attributeFilter"

                if filter_options["FilterType"] == "momentumMagnitudeFilter":
                    g4filter["Attribute"] = "IMag"
                else:
                    g4filter["Attribute"] = "IKE"

                if (
                    "MinValue" in filter_options.keys()
                    and filter_options["MinValue"]
                    and "MaxValue" in filter_options.keys()
                    and filter_options["MaxValue"]
                ):
                    g4filter["Intervals"] = [
                        "{} MeV {} MeV".format(
                            filter_options["MinValue"] / units.MeV,
                            filter_options["MaxValue"] / units.MeV,
                        )
                    ]
                elif "MinValue" in filter_options.keys() and filter_options["MinValue"]:
                    g4filter["Intervals"] = [
                        "0.0 keV {} MeV".format(filter_options["MinValue"] / units.MeV)
                    ]
                    g4filter["Invert"] = True
                elif "MaxValue" in filter_options.keys() and filter_options["MaxValue"]:
                    g4filter["Intervals"] = [
                        "0.0 keV {} MeV".format(filter_options["MaxValue"] / units.MeV)
                    ]
                else:
                    raise ValueError(
                        "Maximum or minimum value should be set when using Momentum Magnitude or Kinetic Energy Filter."
                    )

            if (
                filter_options["FilterType"] == "transverseMomentumFilter"
                or filter_options["FilterType"] == "pseudorapidityFilter"
            ):
                if "MinValue" in filter_options.keys() and filter_options["MinValue"]:
                    g4filter["MinValue"] = filter_options["MinValue"]
                if "MaxValue" in filter_options.keys() and filter_options["MaxValue"]:
                    g4filter["MaxValue"] = filter_options["MaxValue"]

                if (
                    "MinValue" not in g4filter.keys()
                    and "MaxValue" not in g4filter.keys()
                ):
                    raise ValueError(
                        "Maximum or minimum value should be set when using Transverse Momentum Filter."
                    )

            self._visualization_filters.append(g4filter)

    def _set_g4_trajectory_filters(self, cmds, vismgr):
        """Method to set the trajectory filters used in the visualization

        :param cmds: List of visualization commands
        :param vismgr: Visualization manager
        """
        trajectory_filters = self._visualization_filters
        cmd_preset = "/vis/filtering/trajectories"

        for g4filter in trajectory_filters:
            if g4filter["FilterType"] in self._default_trajectory_filters:
                filter_name = g4filter["FilterName"]
                cmd_preset_with_filter_name = "{}/{}".format(cmd_preset, filter_name)

                self._set_ui_command(
                    cmds, cmd_preset + "/create/" + g4filter["FilterType"], filter_name
                )

                if g4filter["FilterType"] == "attributeFilter":
                    if g4filter["Attribute"]:
                        self._set_ui_command(
                            cmds,
                            cmd_preset_with_filter_name + "/setAttribute",
                            g4filter["Attribute"],
                        )

                    if g4filter["Intervals"]:
                        for condition in g4filter["Intervals"]:
                            self._set_ui_command(
                                cmds,
                                cmd_preset_with_filter_name + "/addInterval",
                                condition,
                            )

                    if g4filter["Values"]:
                        for condition in g4filter["Values"]:
                            self._set_ui_command(
                                cmds,
                                cmd_preset_with_filter_name + "/addValue",
                                condition,
                            )

                if g4filter["Values"] and g4filter["FilterType"] != "attributeFilter":
                    for condition in g4filter["Values"]:
                        self._set_ui_command(
                            cmds, cmd_preset_with_filter_name + "/add", condition
                        )

                if g4filter["Invert"]:
                    self._set_ui_command(
                        cmds,
                        cmd_preset_with_filter_name + "/invert",
                        g4filter["Invert"],
                    )

                if g4filter["Verbose"]:
                    self._set_ui_command(
                        cmds,
                        cmd_preset_with_filter_name + "/verbose",
                        g4filter["Verbose"],
                    )

            elif g4filter["FilterType"] == "transverseMomentumFilter":
                if "MinValue" in g4filter.keys() or "MaxValue" in g4filter.keys():
                    from Configurables import GiGaTrajectoryInitialPTFilter

                    vismgr.TrajectoryFactories.append("GiGaTrajectoryInitialPTFilter")
                    factory = GiGaTrajectoryInitialPTFilter(
                        "GiGaMT.GiGaVisManager.GiGaTrajectoryInitialPTFilter"
                    )
                    if "MinValue" in g4filter.keys():
                        factory.MinPT = g4filter["MinValue"]
                    if "MaxValue" in g4filter.keys():
                        factory.MaxPT = g4filter["MaxValue"]
                    self._set_ui_command(cmds, cmd_preset + "/create/initialPTFilter")

            elif g4filter["FilterType"] == "pseudorapidityFilter":
                if "MinValue" in g4filter.keys() or "MaxValue" in g4filter.keys():
                    from Configurables import GiGaTrajectoryInitialEtaFilter

                    vismgr.TrajectoryFactories.append("GiGaTrajectoryInitialEtaFilter")
                    factory = GiGaTrajectoryInitialEtaFilter(
                        "GiGaMT.GiGaVisManager.GiGaTrajectoryInitialEtaFilter"
                    )
                    if "MinValue" in g4filter.keys():
                        factory.MinEta = g4filter["MinValue"]
                    if "MaxValue" in g4filter.keys():
                        factory.MaxEta = g4filter["MaxValue"]
                    self._set_ui_command(cmds, cmd_preset + "/create/initialEtaFilter")

    def _set_phoenix_trajectory_filters(self, dumpg4traj):
        trajectory_filters = self.getProp("TrajectoryFilters")

        for filter in trajectory_filters:
            if "FilterType" not in filter.keys() or not filter["FilterType"]:
                raise ValueError("Filter Type is not provided.")

            if filter["FilterType"] not in self._phoenix_trajectory_filters:
                raise NotImplementedError(
                    filter["FilterType"] + " is not an available G4 Trajectory Filter."
                )

            if filter["FilterType"] == "chargeFilter":
                chargeValues = []
                if "addPositiveChargeParticles" in filter["Options"]:
                    chargeValues.append(1.0)
                if "addNegativeChargeParticles" in filter["Options"]:
                    chargeValues.append(-1.0)
                if "addNeutralParticles" in filter["Options"]:
                    chargeValues.append(0.0)

                if "IsInclusive" in filter.keys() and not filter["IsInclusive"]:
                    chargeValues = list(set([-1.0, 0.0, 1.0]) - set(chargeValues))

                dumpg4traj.AcceptedCharges = chargeValues

            else:
                min_value, max_value = 0, 0

                if "MinValue" in filter.keys() and filter["MinValue"]:
                    min_value = filter["MinValue"]
                if "MaxValue" in filter.keys() and filter["MaxValue"]:
                    max_value = filter["MaxValue"]

                if "IsInclusive" in filter.keys() and not filter["IsInclusive"]:
                    min_value, max_value = max_value, min_value

                if filter["FilterType"] == "momentumMagnitudeFilter":
                    if min_value:
                        dumpg4traj.MinP = min_value
                    if max_value:
                        dumpg4traj.MaxP = max_value

                if filter["FilterType"] == "transverseMomentumFilter":
                    if min_value:
                        dumpg4traj.MinPt = min_value
                    if max_value:
                        dumpg4traj.MaxPt = max_value

                if filter["FilterType"] == "kineticEnergyFilter":
                    if min_value:
                        dumpg4traj.MinKE = min_value
                    if max_value:
                        dumpg4traj.MaxKE = max_value

                if filter["FilterType"] == "pseudorapidityFilter":
                    if min_value:
                        dumpg4traj.MinEta = min_value
                    if max_value:
                        dumpg4traj.MaxEta = max_value

    def _draw_g4hits(self, cmds):
        """Method for adding visualization of hits

        :param cmds: List of visualization commands
        """
        self._set_ui_command(cmds, "/vis/scene/add/hits")

    def _set_other(self, cmds, actioninit):
        """Method to set various different options, such as driver or combining events.

        :param cmds: List of visualization commands
        """
        combine = self.getProp("CombineEvents")
        driver = self.getProp("Driver")
        if combine:
            if driver == "OpenGLImmediateX":
                raise NotImplementedError(
                    "OpenGLImmediateX does not work correctly with event accumulation."
                )
            self._set_ui_command(cmds, "/vis/scene/endOfEventAction", "accumulate")

        # for interactive drivers enable UI sessions
        end_ui_session = True
        if self.getProp("Framework") == "Phoenix":
            end_ui_session = False

        if driver in self._interactive_drivers:
            if combine:
                self._set_ui_command(cmds, "/vis/viewer/refresh", phase="end_run")
                actioninit.GiGaRunActionCommand.EndOfRunUISession = end_ui_session
            else:
                self._set_ui_command(cmds, "/vis/viewer/refresh", phase="end_event")
                actioninit.GiGaEventActionCommand.EndOfEventUISession = end_ui_session

        if driver == "OpenGLImmediateX" or driver == "OpenGLStoredX":
            exportFileName = self.getProp("OGLExportFileName")
            if isinstance(exportFileName, str) and exportFileName != "":
                self._set_ui_command(cmds, "/vis/ogl/export", exportFileName, "end_run")

    def _activate_g4_fields(self, cmds):
        """Method for adding the visualization of the electic and the magnetic field

        :param cmds: List of visualization commands
        """
        if self.isPropertySet("ElectricField") or self.isPropertySet("MagneticField"):
            electricField = self._default_field_visualization | self.getProp(
                "ElectricField"
            )
            if electricField["IsVisible"]:
                self._set_ui_command(
                    cmds,
                    "/vis/scene/add/electricField",
                    "{} {}".format(electricField["N"], electricField["Representation"]),
                )

            magneticField = self._default_field_visualization | self.getProp(
                "MagneticField"
            )
            if magneticField["IsVisible"]:
                self._set_ui_command(
                    cmds,
                    "/vis/scene/add/magneticField",
                    "{} {}".format(magneticField["N"], magneticField["Representation"]),
                )
