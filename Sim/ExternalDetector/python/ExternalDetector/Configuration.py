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
from Gaudi.Configuration import (
    ConfigurableUser,
    log,
)
import Configurables

__author__ = "Michal Mazurek"
__email__ = "michal.mazurek@cern.ch"


class ExternalDetectorEmbedder(ConfigurableUser):
    """This class sets up the provides all the necessary tools that are needed
    to embed volumes (``Shapes``) and make them sensitive (``Sensitive``).
    Moreover, it is possible to attach hit extraction (``Hit``) and monitoring
    algorithms (``Moni``). It can also serve as a geometry service that can
    work on its own by providing the ``World`` property. Finally, it gives a
    possibility to create custom materials through ``Materials`` property.

    :var Shapes: Properties of the volumes used.
    :vartype Shapes: dict, optional

    :var Sensitive: Properties of the sensitive detectors associated with
        the previosuly defined volumes.
    :vartype Sensitive: dict, optional

    :var Hit: Properties of the hit exteraction algorithms associated
        with the previosuly defined sensitive detectors.
    :vartype Hit: dict, optional

    :var Moni: Properties of the monitoring algorithms associated
        with the previosuly defined sensitive detectors.
    :vartype Moni: dict, optional

    :var World: Properties of the world volume. Set this on if you
        want ``ExternalDetector`` pacage to work in a standalone mode
    :vartype World: dict, optional

    :var Materials: Properties of the materials used by the world or
        other volumes created by the ``ExternalDetector``.
    :vartype Materials: dict, optional

    :var MagneticField: Properties of the magnetic field factory.
    :vartype MagneticField: dict, optional

    :Example:

        .. highlight:: python
        .. code-block:: python

            from Configurables import (
                GaussinoSimulation,
                ExternalDetectorEmbedder,
            )
            GaussinoSimulation().ExternalDetectorEmbedder = "MyEmbedder"
            external = ExternalDetectorEmbedder("MyEmbedder")

            external.Shapes = {
                "MyPlane": {
                    # -> type of the embedder
                    "Type": "Cuboid",
                    # -> x position of its center
                    "xPos": 0 * m,
                    # -> y position of its center
                    "yPos": 0 * m,
                    # -> z position of its center
                    "zPos": 0 * m,
                    # -> length of the x side
                    "xSize": 1. * m,
                    # -> length of the y side
                    "ySize": 1. * m,
                    # -> length of the z side
                    "zSize": 1. * m,
                    # + other properties used by the CuboidEmbedder
                      e.g. OutputLevel etc.
                },
            }

            external.Sensitive = {
                # -> important! you have to specify what volume
                "MyPlane": {
                    # -> required, type of the sensitive detector factory
                    "Type": "MCCollectorSensDet",
                    # + other properties used by the GiGaMTG4SensDetFactory
                      e.g. OutputLevel etc.
                },
            }

            external.Hit = {
                # -> important! you have to specify what sensitive volume
                "MyPlane": {
                    # -> required, type of the hit extraction algorithm
                    "Type": "GetMCCollectorHitsAlg",
                    # + other properties used by the Gaudi::Functional
                    #   algorithm e.g. OutputLevel etc.
                },
            }

            external.Moni = {
                # -> important! you have to specify what sensitive volume
                "MyPlane": {
                    # -> required, type of the monitoring algorithm
                    "Type": "SomeMonitoringAlgorithm",
                    # + other properties used by the Gaudi::Functional
                    #   algorithm e.g. OutputLevel etc.
                },
            }



    """

    __slots__ = {
        "Shapes": {},
        #
        # ex. Cuboid
        #
        # "MyPlane": {
        #      -> type of the embedder
        #     "Type": "Cuboid",
        #      -> x position of its center
        #     "xPos": 0 * m,
        #      -> y position of its center
        #     "yPos": 0 * m,
        #      -> z position of its center
        #     "zPos": 0 * m,
        #      -> length of the x side
        #     "xSize": 1. * m,
        #      -> length of the y side
        #     "ySize": 1. * m,
        #      -> length of the z side
        #     "zSize": 1. * m,
        #     + other properties used by the CuboidEmbedder
        #       e.g. OutputLevel etc.
        # },
        #
        #
        #
        "Sensitive": {},
        #
        # ex. MCCollectorSensDet
        #
        # -> important! you have to specify what volume
        # "MyPlane": {
        #      -> required, type of the sensitive detector factory
        #     "Type": "MCCollectorSensDet",
        #     + other properties used by the GiGaMTG4SensDetFactory
        #       e.g. OutputLevel etc.
        # },
        #
        #
        #
        "Hit": {},
        #
        # ex. GetMCCollectorHitsAlg
        #
        # -> important! you have to specify what volume
        # "MyPlane": {
        #      -> required, type of the hit extraction algorithm
        #     "Type": "GetMCCollectorHitsAlg",
        #     + other properties used by the Gaudi::Functional
        #       algorithm e.g. OutputLevel etc.
        # },
        #
        #
        #
        "Moni": {},
        #
        # ex. SomeMonitoringAlgorithm
        #
        # -> important! you have to specify what volume
        # "MyPlane": {
        #      -> required, type of the hit extraction algorithm
        #     "Type": "SomeMonitoringAlgorithm",
        #     + other properties used by the Gaudi::Functional
        #       algorithm e.g. OutputLevel etc.
        # },
        #
        #
        #
        "Materials": {},
        #
        # ex. 1
        #
        # 'Pb': {
        #    'Type': 'MaterialFromElements',
        #      -> type of material factory
        #     'Symbols': ['Pb'],
        #      -> required, list of the elements' symbols
        #     'AtomicNumbers': [82.],
        #      -> required, list of atomic numbers per element
        #     'MassNumbers': [207.2 * g / mole],
        #      -> required, list of mass numbers per element
        #     'MassFractions': [1.],
        #      -> required, list of mass fractions in 0. - 1., total <= 1.
        #     'Density': 11.29 * g / cm3,
        #      -> required, density of the material [g / cm3]
        #     'State': 'Solid',
        #      -> optional, state: ['Gas', 'Liquid', 'Solid'],
        #         default: 'Undefined'
        #     + other properties used by the MaterialFactory
        #       e.g. OutputLevel etc.
        # },
        #
        # ex. 2
        #
        # "OuterSpace": {
        #     "Type": "MaterialFromChemicalPropertiesFactory"
        #      -> type of material factory
        #     "AtomicNumber": 1.,
        #      -> required, atomic number
        #     "MassNumber": 1.01 * g / mole,
        #      -> required, mass number [g / mole]
        #     "Density": 1.e-25 * g / cm3,
        #      -> required, density of the material [g / cm3]
        #     "Pressure": 3.e-18 * pascal,
        #      -> optional, pressure [Pa], default: 1 atm
        #     "Temperature": 2.73 * kelvin,
        #      -> optional, state: ['Gas', 'Liquid', 'Solid'],
        #         default: 'Undefined'
        #     + other properties used by the MaterialFactory
        #       e.g. OutputLevel etc.
        # },
        #
        #
        #
        # only if you want to implement a custom, external world
        # this is mostly for testing purposes
        "World": {},
        #
        # ex.
        #
        # 'World': {
        #   "Type": "ExternalWorldCreator",
        #     -> type of the factory used to build the world
        #     -> see: Sim/ExternalDetector/WorldFactory.h
        #        if you need to create a custom one
        #   "WorldMaterial": "SomeG4Material",
        #     -> material used by the world, it is the name of the G4Material
        #     -> that must be defined before
        #     -> see: Sim/ExternalDetector/MaterialFactory.h
        #     + other properties used by the WorldFactory
        #       e.g. OutputLevel etc.
        # }
        # only if you implemented the custom, external world
        'MagneticField': {},
        #
        # ex.
        #
        # 'MagneticField': {
        #   "Type": "UniformMagneticField",
        #     -> type of the factory used to build the world
        #   "Name": "YourOptionalName",
        #     -> optional name for the magnetic field
        #   "B_x": 0. * tesla,
        #   "B_y": 0. * tesla,
        #   "B_z": 0. * tesla,
        #     -> this is equivalent to no magnetic field
        #     + other properties used by the magnetic field factory
        #       e.g. OutputLevel etc.
        # }
    }

    _added_dets = []
    _added_hits_algs = []

    def embed(self, geo):
        """Takes care of setting up the right tools and factories responsible
        for the geometry. It is based on the properties provided in ``Shapes``,
        ``Sensitive``, ``Materials``, and ``World``. Properties correspond to
        the properites used by each factory.

        :param geo: Tool responsible for detector construction in Geant4.
            In Gaussino, it is ``GiGaMTDetectorConstructionFAC``.
        """
        if not geo:
            raise RuntimeError("ERROR: GeoService not provided")
        for name, props in self.getProp("Materials").items():
            self._check_props(name, props, required=[])
            if "Type" not in props:
                props["Type"] = "MaterialFromChemicalProperties"
            if "Name" not in props:
                props["Name"] = name
            tool_conf = getattr(Configurables, props["Type"])
            tool = tool_conf(props["Name"], **self._refine_props(props))
            geo.addTool(tool, name=props["Name"])
            geo.ExternalMaterials.append(props["Type"] + "/" + props["Name"])
            log.info(
                "Registered external material tool {} of type {}.".format(
                    props["Name"], props["Type"]
                )
            )

        for name, props in self.getProp("Shapes").items():
            self._check_props(name, props)
            tool_name = props["Type"] + "Embedder"
            self._embedding_tool(name, geo, tool_name, props)
            geo.ExternalDetectors.append(tool_name + "/" + name)
            log.info(
                "Registered external detector {} of type {}.".format(name, tool_name)
            )
            self._added_dets.append(name)

        world = self.getProp("World")
        if world:
            self._check_props(
                "World", world, required=["Type", "WorldMaterial"])
            svc_conf = getattr(Configurables, world["Type"])
            world_svc = svc_conf(**self._refine_props(world))
            geo.GiGaMTGeoSvc = world["Type"]
            log.info("Registered external world service of type {}.".format(
                world["Type"]))

            mag_field_props = self.getProp('MagneticField')
            if mag_field_props:
                self._check_props("MagneticField", mag_field_props)
                mag_field_conf = getattr(Configurables,
                                         mag_field_props['Type'])
                name = mag_field_props.get("Name", mag_field_props['Type'])
                mag_field_tool = mag_field_conf(
                    name,
                    **self._refine_props(
                        mag_field_props, keys_to_refine=['Type', 'Name']))
                from Configurables import MagneticFieldManager
                world_svc.FieldManager = "MagneticFieldManager/FieldMgr"
                world_svc.addTool(MagneticFieldManager("FieldMgr"), name="FieldMgr")
                world_svc.FieldMgr.StepperFactory = "G4ClassicalRK4"
                world_svc.FieldMgr.FieldFactory = mag_field_tool.getFullName()
                world_svc.FieldMgr.addTool(mag_field_tool, name=name)

    def activate_hits_alg(self, slot=""):
        """Takes care of setting up the right hit extraction algorithms.
        It is based on the properties provided in ``Hit``, but the volume must
        be created before as mentioned by ``Shapes``. Properties correspond to
        the properites used by each hit extraction factory.

        :param slot: additional naming for spill-over, not working for now
        """
        algs = []
        hit_algs = self.getProp("Hit")
        if type(hit_algs) is dict:
            for det_name, hit_alg_props in hit_algs.items():
                if det_name not in self._added_dets:
                    log.warning("External geometry not set for " + det_name)
                    continue
                self._check_props(det_name, hit_alg_props)
                alg_conf = getattr(Configurables, hit_alg_props["Type"])
                hit_alg_name = "Get" + det_name + "Hits" + slot
                alg = alg_conf(
                    hit_alg_name,
                    MCHitsLocation="MC/" + det_name + "/Hits",
                    CollectionName=det_name + "SDet/Hits",
                    **self._refine_props(hit_alg_props)
                )
                log.info("Registered external hit extraction " + hit_alg_name)
                self._added_hits_algs.append(det_name)
                algs.append(alg)
        return algs

    def activate_moni_alg(self, slot=""):
        """Takes care of setting up the right monitoring algorithms.
        It is based on the properties provided in ``Moni``, but the volume must
        be created before as mentioned by ``Shapes``. Properties correspond to
        the properites used by each hit extraction factory.

        :param slot: additional naming for spill-over, not working for now
        """
        algs = []
        moni_algs = self.getProp("Moni")
        if type(moni_algs) is dict:
            for det_name, moni_alg_props in moni_algs.items():
                if det_name not in self._added_hits_algs:
                    log.warning("External hit algorithm not set for " + det_name)
                    continue
                self._check_props(
                    det_name, moni_alg_props, required=["Type", "HitsPropertyName"]
                )
                moni_alg_props[moni_alg_props["HitsPropertyName"]] = (
                    "MC/" + det_name + "/Hits"
                )
                alg_conf = getattr(Configurables, moni_alg_props["Type"])
                moni_alg_name = det_name + moni_alg_props["Type"] + slot
                alg = alg_conf(
                    moni_alg_name,
                    **self._refine_props(
                        moni_alg_props, keys_to_refine=["Type", "HitsPropertyName"]
                    )
                )
                log.info("Registered external monitoring " + moni_alg_name)
                algs.append(alg)
        return algs

    def _check_props(self, name, props, required=["Type"]):
        if type(props) is not dict:
            raise RuntimeError(
                "ERROR: Dictionary of {} properties not provided.".format(name)
            )
        for req in required:
            if not props.get(req):
                raise RuntimeError(
                    "ERROR: Property {} for {} not provided.".format(req, name)
                )

    def _refine_props(self, props, keys_to_refine=["Type"]):
        return {key: prop for key, prop in props.items() if key not in keys_to_refine}

    def _register_prop(self, props, key, prop):
        if not props.get(key):
            props[key] = prop

    def _embedding_tool(self, name, geo, tool_name, props):
        log.info("Registering external {} as {}".format(name, tool_name))
        tool_conf = getattr(Configurables, tool_name)
        sens_det_props = self.getProp("Sensitive").get(name)
        self._register_prop(props, "LogicalVolumeName", name + "LVol")
        self._register_prop(props, "PhysicalVolumeName", name + "PVol")
        sens_det_tool = None
        if sens_det_props:
            self._check_props(name, sens_det_props)
            sens_det_conf = getattr(Configurables, sens_det_props["Type"])
            self._register_prop(sens_det_props, "SensDetName", name + "SDet")
            self._register_prop(
                props,
                "SensDet",
                sens_det_props["Type"] + "/" + sens_det_props["SensDetName"],
            )
            sens_det_tool = sens_det_conf(
                sens_det_props["SensDetName"],
                **self._refine_props(sens_det_props, ["Type", "SensDetName"])
            )
        tool = tool_conf(name, **self._refine_props(props))
        if sens_det_tool:
            tool.addTool(sens_det_tool)
        geo.addTool(tool, name=name)
