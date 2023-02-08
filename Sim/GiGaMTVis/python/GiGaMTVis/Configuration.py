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

import copy

__author__ = "Filip Bilandžija, Michał Mazurek"
__email__ = "michal.mazurek@cern.ch"


class Geant4Visualization(ConfigurableUser):
    """This class sets up and provides all the necessary tools that are needed
    for visualization and filtering of geometry, tracks and hits in Geant4. It
    also provides presets for some trajectory models, while retaining the option
    for users to build their own custom trajectory model.

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
            from Configurables import SimPhase
            SimPhase().Visualization = True
            from Configurables import Geant4Visualization
            g4vis = Geant4Visualization()
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
        "Driver": "",
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
        "DrawG4Hits": True,
        # view
        "CameraPhi": 0,  # deg,
        "CameraTheta": 0,  # deg,
        "Zoom": 1,
        # OpenGLSettings
        "OGLExportFileName": "",
        # other
        "CombineEvents": True,
        "Debug": False,
        # fields
        "ElectricField": {},
        "MagneticField": {}
    }

    _available_drivers = [
        'ASCIITree',
        'DAWNFILE',
        'HepRep',
        'OpenGLImmediateX',
        'OpenGLStoredX',
    ]

    _parsed_drivers = {
        'ASCIITree': 'ATree',
        'DAWN': 'DAWNFILE',
        'HepRep': 'HepRepFile',
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

    # Available trajectory models:
    _available_trajectory_models = [
        'customTrajectoryModel', 'drawByCharge', 'drawByEncounteredVolume',
        'drawByOriginVolume', 'drawByParticleID', 'generic', 'drawByMomentum',
        'drawByKineticEnergy'
    ]

    _default_trajectory_models = [
        'drawByCharge', 'drawByEncounteredVolume', 'drawByOriginVolume',
        'drawByParticleID', 'generic'
    ]

    # Available trajectory filters
    _trajectory_filters = [
        'chargeFilter', 'customTrajectoryFilter', 'encounteredVolumeFilter',
        'originVolumeFilter', 'particleFilter', 'momentumMagnitudeFilter',
        'kineticEnergyFilter', 'transverseMomentumFilter'
    ]

    # Default trajectory filters
    _default_trajectory_filters = [
        'chargeFilter', 'encounteredVolumeFilter', 'originVolumeFilter',
        'particleFilter', 'attributeFilter'
    ]

    _storing_trajectories = [
        "All",  # store all G4 trajectories
        "Truth",  # store G4 trajectories that correspond to MCTruth
        "Marked",  # store only G4 trajectories that create hits
    ]

    _filter_keyname = {
        'chargeFilter': 'gaussinoCF',
        'particleFilter': 'gaussinoPF',
        'originVolumeFilter': 'gaussinoOVF',
        'attributeFilter': 'gaussinoAF',
        'generic': 'gaussinoG',
        'momentumMagnitudeFilter': 'gaussinoIMagF',
        'kineticEnergyFilter': 'gaussinoIKEF',
        'transverseMomentumFilter': 'gaussinoPTF'
    }

    chargeFilterPresets = {
        'addPositiveChargeParticles', 'addNegativeChargeParticles',
        'addNeutralParticles'
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
        "StepPtsSize": 2
    }

    _default_trajectory_filter_options = {
        "FilterName": "",
        "FilterType": "",
        "Verbose": False,
        "Values": [],
        "Attribute": "",
        "Intervals": [],
        "Invert": False
    }

    _default_field_visualization = {
        "IsVisible": False,
        "N": 0,
        "Representation": "fullArrow"
    }

    _visualization_model = {}

    _visualization_filters = []

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

        vismgr.RequiredDriver = driver
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

        self._activate_fields(cmds)

        self._set_view(cmds)

        self._set_verbosity(cmds)

        visualize_data = self.getProp("DrawTrajectories") or self.getProp(
            "DrawG4Hits")
        if visualize_data and driver in self._only_geometry_drivers:
            if self.isPropertySet("DrawTrajectories") or self.isPropertySet(
                    "DrawG4Hits"):
                raise ValueError(
                    driver +
                    " does not support visualizing of the event data.")
            return

        if self.getProp("DrawGeometry"):
            self._draw_geometry(cmds)

        if self.getProp("DrawTrajectories"):
            self._draw_trajectories(cmds, actioninit)
            if self.isPropertySet("TrajectoryModel"):
                self._create_trajectory_model()
                self._set_trajectory_model(cmds)
            if self.isPropertySet("TrajectoryFilters"):
                self._create_trajectory_filters()
                self._set_trajectory_filters(cmds, vismgr)

        if self.getProp("DrawG4Hits"):
            self._draw_g4hits(cmds)

        self._set_other(cmds, actioninit)

    def _set_ui_command(self,
                        cmds,
                        ui_command,
                        command_argument='',
                        phase='init'):
        cmds[phase].append("{} {}".format(ui_command, command_argument))

    def _activate_g4_driver(self, cmds):
        """ Method to activate on of the available Geant4 drivers

        :param cmds: List of visualization commands
        """
        driver = self.getProp("Driver")
        if driver not in self._available_drivers:
            raise NotImplementedError(driver +
                                      " is not an available G4 driver.")
        parsed_driver = self._parsed_drivers.get(driver, driver)
        self._set_ui_command(cmds, '/vis/open', parsed_driver)
        self._set_ui_command(cmds, '/vis/scene/create')
        # '/vis/sceneHandler/create {}'.format(parsed_driver)

    def _set_view(self, cmds):
        """ Method to set viewpoint coordinates (theta and phi) and zoom

        :param cmds: List of visualization commands
        """
        theta = self.getProp("CameraTheta")
        phi = self.getProp("CameraPhi")
        if theta or phi:
            self._set_ui_command(cmds, '/vis/viewer/set/viewpointThetaPhi',
                                 "{} {}".format(theta, phi))

        zoom = self.getProp("Zoom")
        if zoom != 1:
            self._set_ui_command(cmds, '/vis/viewer/zoom', zoom)

    def _draw_geometry(self, cmds):
        """ Method to add the commands for visualization of geometry and attach it to the scene

        :param cmds: List of visualization commands
        """
        cmd = "/vis/scene/add/volume"
        vols = self.getProp("Volumes")
        for vol in vols:
            copy_no = self.getProp("VolumesCopyNumber").get(vol, -1)
            depth = self.getProp("VolumesDepthOfDescent").get(vol, -1)
            self._set_ui_command(cmds, cmd, "{} {} {}".format(
                vol, copy_no, depth))
        if not vols:
            self._set_ui_command(cmds, cmd)
        self._set_ui_command(cmds, "/vis/sceneHandler/attach")

    def _set_verbosity(self, cmds):
        """ Method to set the verbosity of tracking, event and run

        :param cmds: List of visualization commands
        """
        if self.isPropertySet("TrackingVerbosity"):
            tracking_verbosity = self.getProp("TrackingVerbosity")
            self._set_ui_command(cmds, "/tracking/verbose", tracking_verbosity,
                                 'begin_run')

        if self.isPropertySet("EventVerbosity"):
            event_verbosity = self.getProp("EventVerbosity")
            self._set_ui_command(cmds, "/event/verbose", event_verbosity,
                                 'begin_run')

        if self.isPropertySet("RunVerbosity"):
            run_verbosity = self.getProp("RunVerbosity")
            self._set_ui_command(cmds, "/run/verbose", run_verbosity,
                                 'begin_run')

    def _draw_trajectories(self, cmds, actioninit):
        """ Method to configure trajectory visualization and it's properties:
            trajectory type, storing of trajectories and geometry style

        :param cmds: List of visualization commands
        :param actioninit: Action parameter
        """
        self._set_ui_command(cmds, "/vis/scene/add/trajectories",
                             self.getProp("TrajectoryType"))
        # warning: adding trajectories will set storing of the trajectories
        #          by default, so additional checks have to be made if
        #          storing is to be done internally in Gaussino
        store_type = self.getProp("StoreTrajectories")
        if store_type not in self._storing_trajectories:
            raise ValueError(
                "Only the following types of trajectory storing are available: [{}]"
                .format((", ").join(self._storing_trajectories)))
        if store_type == "Marked":
            self._set_ui_command(cmds, "/tracking/storeTrajectory", 0)
            actioninit.TruthFlaggingTrackAction.StoreMarkedTrajectories = True
        elif store_type == "Truth":
            self._set_ui_command(cmds, "/tracking/storeTrajectory", 0)
            actioninit.TruthFlaggingTrackAction.StoreTrajectories = True

        style = self.getProp("GeometryStyle")
        styles = ['wireframe', 'surface', 'cloud']
        if style not in styles:
            raise ValueError(
                "Only the following styles of geometry are available: [{}]".
                format((", ").join(styles)))
        if style == 'surface':
            self._set_ui_command(cmds, "/vis/viewer/colourByDensity")
        self._set_ui_command(cmds, "/vis/viewer/set/style", style)

    def _create_trajectory_model(self):
        """ Method for creating the trajectory model to be used in ``_set_trajectory_model``

        """
        trajectory_model_name = self.getProp("TrajectoryModel")
        trajectory_model_options = self.getProp("TrajectoryModelOptions")
        trajectory_model = self._default_trajectory_model_options | trajectory_model_options

        if trajectory_model_name not in self._available_trajectory_models:
            raise NotImplementedError(
                trajectory_model_name +
                " is not an available G4 Trajectory Model.")

        if trajectory_model_name in self._default_trajectory_models:
            trajectory_model['ModelName'] = trajectory_model_name

        elif trajectory_model_name == 'drawByMomentum' or trajectory_model_name == 'drawByKineticEnergy':
            trajectory_model['ModelName'] = 'drawByAttribute'
            if trajectory_model_name == 'drawByMomentum':
                trajectory_model['Attribute'] = 'IMag'
            else:
                trajectory_model['Attribute'] = 'IKE'
            if not trajectory_model_options or not trajectory_model_options[
                    'Intervals'] or not len(
                        trajectory_model_options['Intervals']):
                trajectory_model['Intervals'] = [
                    'interval1 0.0 keV 10 MeV', 'interval2 10 MeV 250 MeV',
                    'interval3 250 MeV 1 GeV', 'interval4 1 GeV 10 GeV',
                    'interval5 10 GeV 1 TeV'
                ]
            if not trajectory_model_options or not trajectory_model_options[
                    'LineColors'] or not len(
                        trajectory_model_options['LineColors']):
                trajectory_model['LineColors'] = [
                    'interval1 cyan', 'interval2 orange', 'interval3 green',
                    'interval4 blue', 'interval5 green'
                ]

        elif trajectory_model_name == 'customTrajectoryModel':
            trajectory_model["ModelName"] = 'drawByAttribute'

        for key in trajectory_model.keys():
            self._visualization_model[key] = trajectory_model[key]

    def _set_trajectory_model(self, cmds):
        """ Method to set the trajectory visualization model

        :param cmds: List of visualization commands
        """
        trajectory_model = self._visualization_model
        cmd_preset = "/vis/modeling/trajectories"
        traj_model_key = "trajModel"
        cmd_with_traj_key = "{}/{}".format(cmd_preset, traj_model_key)

        self._set_ui_command(
            cmds, cmd_preset + "/create/" + trajectory_model['ModelName'],
            traj_model_key)
        self._set_ui_command(cmds,
                             cmd_with_traj_key + "/default/setDrawStepPts",
                             trajectory_model['DrawStepPts'])
        self._set_ui_command(cmds,
                             cmd_with_traj_key + "/default/setStepPtsSize",
                             trajectory_model['StepPtsSize'])

        if trajectory_model['ModelName'] == 'drawByAttribute':
            if trajectory_model['Attribute']:
                self._set_ui_command(cmds, cmd_with_traj_key + "/setAttribute",
                                     trajectory_model['Attribute'])

            if trajectory_model['Values']:
                for value in trajectory_model['Values']:
                    self._set_ui_command(cmds, cmd_with_traj_key + "/addValue",
                                         value)

            if trajectory_model['Intervals']:
                for interval in trajectory_model['Intervals']:
                    self._set_ui_command(
                        cmds, cmd_with_traj_key + "/addInterval", interval)

            if trajectory_model['LineColors']:
                for color in trajectory_model['LineColors']:
                    [key, color] = color.split(" ", 1)
                    self._set_ui_command(
                        cmds,
                        cmd_with_traj_key + "/{}/setLineColour".format(key),
                        color)

            if trajectory_model['RGBAColors']:
                for color in trajectory_model['RGBAColors']:
                    [key, color] = color.split(" ", 1)
                    self._set_ui_command(
                        cmds, cmd_with_traj_key +
                        "/{}/setLineColourRGBA".format(key), color)

    def _create_trajectory_filters(self):
        """ Method for creating trajectory filters to be used in ``_set_trajectory_filters``

        """
        trajectory_filters = self.getProp("TrajectoryFilters")

        for filter_options in trajectory_filters:
            if "FilterType" not in filter_options.keys(
            ) or not filter_options["FilterType"]:
                raise ValueError("Filter Type is not provided.")

            if filter_options["FilterType"] not in self._trajectory_filters:
                raise NotImplementedError(
                    filter_options["FilterType"] +
                    " is not an available G4 Trajectory Filter.")
            g4filter = copy.deepcopy(self._default_trajectory_filter_options)
            g4filter["FilterType"] = filter_options["FilterType"]

            if "FilterName" in filter_options.keys(
            ) and filter_options["FilterName"]:
                g4filter["FilterName"] = filter_options["FilterName"]
            else:
                g4filter["FilterName"] = self._filter_keyname[
                    filter_options["FilterType"]]

            if "IsInclusive" in filter_options.keys():
                g4filter["Invert"] = not filter_options["IsInclusive"]

            if filter_options["FilterType"] == 'chargeFilter':
                if "Values" in filter_options.keys():
                    g4filter['Values'] = filter_options["Values"]

                if "Options" in filter_options.keys():
                    if 'addPositiveChargeParticles' in filter_options[
                            "Options"]:
                        g4filter['Values'].append(1)
                    if 'addNegativeChargeParticles' in filter_options[
                            "Options"]:
                        g4filter['Values'].append(-1)
                    if 'addNeutralParticles' in filter_options["Options"]:
                        g4filter['Values'].append(0)

            if filter_options["FilterType"] == 'particleFilter':
                g4filter["Values"] = filter_options["Particles"]

            if filter_options[
                    "FilterType"] == 'momentumMagnitudeFilter' or filter_options[
                        "FilterType"] == 'kineticEnergyFilter':
                g4filter["FilterType"] = 'attributeFilter'

                if filter_options["FilterType"] == 'momentumMagnitudeFilter':
                    g4filter["Attribute"] = "IMag"
                else:
                    g4filter["Attribute"] = "IKE"

                if "MinValue" in filter_options.keys() and filter_options[
                        "MinValue"] and "MaxValue" in filter_options.keys(
                        ) and filter_options["MaxValue"]:
                    g4filter["Intervals"] = [
                        "{} {}".format(filter_options["MinValue"],
                                       filter_options["MaxValue"])
                    ]
                elif "MinValue" in filter_options.keys(
                ) and filter_options["MinValue"]:
                    g4filter["Intervals"] = [
                        "0.0 keV {} MeV".format(
                            filter_options["MinValue"] / MeV)
                    ]
                    g4filter["Invert"] = True
                elif "MaxValue" in filter_options.keys(
                ) and filter_options["MaxValue"]:
                    g4filter["Intervals"] = [
                        "0.0 keV {} MeV".format(
                            filter_options["MaxValue"] / MeV)
                    ]
                else:
                    raise ValueError(
                        "Maximum or minimum value should be set when using Momentum Magnitude or Kinetic Energy Filter."
                    )

            if filter_options["FilterType"] == 'transverseMomentumFilter':
                if "MinValue" in filter_options.keys(
                ) and filter_options["MinValue"]:
                    g4filter["MinValue"] = filter_options["MinValue"]
                if "MaxValue" in filter_options.keys(
                ) and filter_options["MaxValue"]:
                    g4filter["MaxValue"] = filter_options["MaxValue"]

                if "MinValue" not in g4filter.keys(
                ) and "MaxValue" not in g4filter.keys():
                    raise ValueError(
                        "Maximum or minimum value should be set when using Transverse Momentum Filter."
                    )

            self._visualization_filters.append(g4filter)

    def _set_trajectory_filters(self, cmds, vismgr):
        """ Method to set the trajectory filters used in the visualization

        :param cmds: List of visualization commands
        :param vismgr: Visualization manager
        """
        trajectory_filters = self._visualization_filters
        cmd_preset = "/vis/filtering/trajectories"

        for g4filter in trajectory_filters:
            if g4filter["FilterType"] in self._default_trajectory_filters:
                filter_name = g4filter["FilterName"]
                cmd_preset_with_filter_name = "{}/{}".format(
                    cmd_preset, filter_name)

                self._set_ui_command(
                    cmds, cmd_preset + "/create/" + g4filter["FilterType"],
                    filter_name)

                if g4filter["FilterType"] == 'attributeFilter':
                    if g4filter["Attribute"]:
                        self._set_ui_command(
                            cmds,
                            cmd_preset_with_filter_name + "/setAttribute",
                            g4filter["Attribute"])

                    if g4filter["Intervals"]:
                        for condition in g4filter["Intervals"]:
                            self._set_ui_command(
                                cmds,
                                cmd_preset_with_filter_name + "/addInterval",
                                condition)

                    if g4filter["Values"]:
                        for condition in g4filter["Values"]:
                            self._set_ui_command(
                                cmds,
                                cmd_preset_with_filter_name + "/addValue",
                                condition)

                if g4filter["Values"] and g4filter[
                        "FilterType"] != 'attributeFilter':
                    for condition in g4filter["Values"]:
                        self._set_ui_command(
                            cmds, cmd_preset_with_filter_name + "/add",
                            condition)

                if g4filter["Invert"]:
                    self._set_ui_command(
                        cmds, cmd_preset_with_filter_name + "/invert",
                        g4filter["Invert"])

                if g4filter["Verbose"]:
                    self._set_ui_command(
                        cmds, cmd_preset_with_filter_name + "/verbose",
                        g4filter["Verbose"])

            elif g4filter["FilterType"] == 'transverseMomentumFilter':
                if "MinValue" in g4filter.keys(
                ) or "MaxValue" in g4filter.keys():
                    from Configurables import GiGaTrajectoryInitialPTFilter
                    vismgr.TrajectoryFactories.append(
                        "GiGaTrajectoryInitialPTFilter")
                    factory = GiGaTrajectoryInitialPTFilter(
                        "GiGaMT.GiGaVisManager.GiGaTrajectoryInitialPTFilter")
                    if "MinValue" in g4filter.keys():
                        factory.MinPT = g4filter["MinValue"]
                    if "MaxValue" in g4filter.keys():
                        factory.MaxPT = g4filter["MaxValue"]
                    self._set_ui_command(
                        cmds, cmd_preset + "/create/initialPTFilter")

    def _draw_g4hits(self, cmds):
        """ Method for adding visualization of hits

        :param cmds: List of visualization commands
        """
        self._set_ui_command(cmds, "/vis/scene/add/hits")

    def _set_other(self, cmds, actioninit):
        """ Method to set various different options, such as driver or combining events.

        :param cmds: List of visualization commands
        """
        combine = self.getProp("CombineEvents")
        driver = self.getProp("Driver")
        if combine:
            if driver == "OpenGLImmediateX":
                raise NotImplementedError(
                    "OpenGLImmediateX does not work correctly with event accumulation."
                )
            self._set_ui_command(cmds, "/vis/scene/endOfEventAction",
                                 "accumulate")

        # for interactive drivers enable UI sessions
        if driver in self._interactive_drivers:
            if combine:
                self._set_ui_command(
                    cmds, "/vis/viewer/refresh", phase="end_run")
                actioninit.GiGaRunActionCommand.EndOfRunUISession = True
            else:
                self._set_ui_command(
                    cmds, "/vis/viewer/refresh", phase="end_event")
                actioninit.GiGaEventActionCommand.EndOfEventUISession = True

        if driver == "OpenGLImmediateX" or driver == "OpenGLStoredX":
            exportFileName = self.getProp("OGLExportFileName")
            if isinstance(exportFileName, str) and exportFileName != "":
                self._set_ui_command(cmds, "/vis/ogl/export", exportFileName,
                                     "end_run")

    def _activate_fields(self, cmds):
        """ Method for adding the visualization of the electic and the magnetic field

        :param cmds: List of visualization commands
        """
        if self.isPropertySet("ElectricField") or self.isPropertySet(
                "MagneticField"):
            electricField = self._default_field_visualization | self.getProp(
                "ElectricField")
            if electricField["IsVisible"]:
                self._set_ui_command(
                    cmds, "/vis/scene/add/electricField", "{} {}".format(
                        electricField["N"], electricField["Representation"]))

            magneticField = self._default_field_visualization | self.getProp(
                "MagneticField")
            if magneticField["IsVisible"]:
                self._set_ui_command(
                    cmds, "/vis/scene/add/magneticField", "{} {}".format(
                        magneticField["N"], magneticField["Representation"]))
