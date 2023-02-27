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
from Gaudi.Configuration import ConfigurableUser
from Gaudi.Configuration import log
import Configurables

__author__ = "Michal Mazurek"
__email__ = "michal.mazurek@cern.ch"


class CustomSimulation(ConfigurableUser):
    """This class provides all the necessary tools that are needed to create a
    custom simulation model in a specific region and attach the corrsponding
    custom simulation physics. Custom simulation model is chosen via the
    (``Model``) property (don't forget to specify the ``Type`` i.e. the
    factory name). The model will be then specified in a region created based
    on the properties in ``Region`` property. Finally, each custom simulation
    model needs a dedicated physics factory where you specify what kind of
    particles will be tracked in that model.

    :var Model: Properties of the custom simulation model used.
    :vartype Model: dict, required

    :var Region: Properties of the G4Region where the custom simulation will take
        place.
    :vartype Region: dict, required

    :var Physics: Properties of the physics factory used for all the custom
        simulation models in that creator.
    :vartype Physics: dict, required

    :Example:

        .. highlight:: python
        .. code-block:: python

            from Gaussino.Simulation import GaussinoSimulation
            GaussinoSimulation().CustomSimulation = "MyCustomSimulationCreator"
            from Configurables import CustomSimulation
            custom = CustomSimulation("MyCustomSimulationCreator")

            customsim.Model = {
                'MyCustomSimModel': {
                    'Type': 'ImmediateDepositModel',
                }
            }

            custsim.Region = {
                'VacuumCubeImmediateDeposit': {
                    'SensitiveDetectorName': 'VacuumCubeSDet',
                }
            }

            custsim.Physics = {
                'ParticlePIDs': [22],
            }

    """

    __slots__ = {
        "Model": {},
        #
        # ex. ImmediateDepositModel
        #
        # "MyImmediateDepositModel": {
        #      -> type of the factory model
        #     "Type": "ImmediateDepositModel",
        #      -> name of the model (optional, done automatically)
        #     "Name": "MySuperModel",
        #      -> name of the region (optional, found automatically)
        #     "RegionName": "MySuperRegion",
        #     + other properties used by the factory
        #       e.g. OutputLevel etc.
        # },
        "Region": {},
        #
        # ex.
        #
        # "MyImmediateDepositModel": {
        #      -> type of the region factory (optional, default value)
        #     "Type": "CustomSimulationRegionFactory",
        #      -> name (optional, done automatically)
        #     "Name": "MySuperRegion",
        #      -> name of the sensitive detector
        #     "SensitiveDetectorName": "DetectorSDet",
        #      -> list of the logical volumes
        #         (optional, if SensitiveDetectorName is not specified)
        #     "Volumes": ["VolumeA", "VolumeB"],
        #     + other properties used by the factory
        #       e.g. OutputLevel etc.
        # },
        "Physics": {},
        #
        #   -> type of the physics factory (optional, default value)
        #   "Type": "DefaultCustomPhysics",
        #   -> list of PIDs of particles to custom-simulate
        #   "ParticlePIDs": [22, 11],
        #   -> list of the parallel worlds correspoinding to the particles
        #      (optional, done automatically, empty string for the mass geo)
        #   "ParticleWorlds": ["ParallelWorld1", "ParallelWorld1"],
        #   + other properties used by the factory
        #   e.g. OutputLevel etc.
        #
    }

    def create(self, dettool):
        """Takes care of setting up the right tools and factories responsible
        for setting up the custom simulation model and it's region. It is based
        on the properties provided in ``Model`` and ``Region``. Properties
        correspond to the properites used by each factory.

        :param dettool: Tool responsible for detector construction in Geant4.
            In Gaussino, it is ``GiGaMTDetectorConstructionFAC``.
        """
        if not dettool:
            raise RuntimeError("Detector Construction tool not provided")
        models_props = self.getProp("Model")
        if type(models_props) is not dict:
            raise RuntimeError("Model property must be a dict")
        regions_props = self.getProp("Region")
        if type(regions_props) is not dict:
            raise RuntimeError("Region property must be a dict")

        for name, props in models_props.items():
            self._check_props(name, props)
            if "Name" not in props:
                props["Name"] = name

            # setting up the region
            region_props = regions_props.get(name)
            self._check_props(
                name + "Region", region_props, required=[]
            )  # there are required, but have to be handled a bit differently
            if not region_props.get("SensitiveDetectorName") and not region_props.get(
                "Volumes"
            ):
                raise RuntimeError(
                    "Property 'SensitiveDetectorName' or 'Volumes' must be provided for "
                    + name
                )
            if not region_props.get("Type"):
                log.info("Assigning default region factory to " + name)
                region_props["Type"] = "CustomSimulationRegionFactory"
            if not region_props.get("Name"):
                region_props["Name"] = name + "Region"
            region_conf = getattr(Configurables, region_props["Type"])
            region_tool = region_conf(
                region_props["Name"], **self._refine_props(region_props)
            )
            dettool.addTool(region_tool, name=region_props["Name"])
            dettool.CustomSimulationRegionFactories.append(
                getattr(dettool, region_props["Name"])
            )
            log.info(
                "Registered a custom simulation region tool {} of type {}.".format(
                    region_props["Name"], region_props["Type"]
                )
            )

            # setting up the model
            if not props.get("RegionName"):
                props["RegionName"] = region_props["Name"]
            model_name = props["Name"] + "Model"
            model_conf = getattr(Configurables, props["Type"])
            model_tool = model_conf(model_name, **self._refine_props(props))
            dettool.addTool(model_tool, name=model_name)
            dettool.CustomSimulationModelFactories.append(getattr(dettool, model_name))
            log.info(
                "Registered a custom simulation model tool {} of type {}.".format(
                    model_name, props["Type"]
                )
            )

    def attach_physics(self, modular_list, world_name=""):
        """Takes care of setting up the right tools and factories responsible
        for creating the physics factory for all the custom simulation models.
        All the properties should be provided in the ``Physics`` property.

        :param modular_list: Modular physics list tool, should be
            ``GiGaMTModularPhysListFAC``
        :param world_name: world where the physics will take action,
            empty means the mass world
        """
        phys_props = self.getProp("Physics")
        name = world_name + "CustomPhysics"
        self._check_props(name, phys_props, required=["ParticlePIDs"])
        factype = phys_props.get("Type")
        if not factype:
            log.info(
                "No factory type specified for {}. Using default physics world factory".format(
                    name
                )
            )
            factype = "DefaultCustomPhysics"
        if factype == "DefaultCustomPhysics":
            if not phys_props.get("ParticleWorlds"):
                # if no parallel worlds fill the list with empty strings
                phys_props["ParticleWorlds"] = [world_name] * len(
                    phys_props["ParticlePIDs"]
                )
        fac_conf = getattr(Configurables, factype)
        fsph = fac_conf(name, **self._refine_props(phys_props))
        modular_list.addTool(fsph)
        modular_list.PhysicsConstructors.append(getattr(modular_list, name))

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
