###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
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


class ParallelGeometry(ConfigurableUser):
    """This class sets up external detectors (from ``ExternalDetector``
    package) in parallel worlds on top of mass geometry. This can be used
    in studies where we would like to have volumes / sensitive detectors
    overlapping each other.

    :var ParallelWorlds: Properties of the parallel worlds (there can be more
        than one parallel world).
    :vartype ParallelWorlds: dict, required

    :var ParallelPhysics: Properties of the physics factories that add a
        special behaviour of the particles tracked in the parallel worlds.
    :vartype ParallelPhysics: dict, required

    .. note::
        Note! Make sure that all your detectors have materials
        if you want to export a GDML ( in parallel geometry volumes do not
        have have materials defined)
        i.e. material != nullptr, as G4GDMLParser will most likely crash


    :Example:

        .. highlight:: python
        .. code-block:: python

            from Configurables import ParallelGeometry
            ParallelGeometry().ParallelWorlds = {
                'ParallelWorld1': {
                    'ExternalDetectorEmbedder': 'ParallelEmbedder',
                    'ExportGDML': {
                        'GDMLFileName': 'ParallelWorld1.gdml',
                        'GDMLFileNameOverwrite': True,
                        'GDMLExportSD': True,
                        'GDMLExportEnergyCuts': True,
                    },
                },
            }

            ParallelGeometry().ParallelPhysics = {
                'ParallelWorld1': {
                    'LayeredMass': False,
                    # -> False = see material of the world below in stack
                    'ParticlePIDs': [22],
                    # -> empty means track all particles
                },
            }

            # then define ParallelEmbedder as in the ``ExternalDetector``
            # section

    """

    __slots__ = {
        "ParallelWorlds": {},
        #
        #
        # Note! Make sure that all your detectors have materials
        # if you want to export a GDML
        # i.e. material != nullptr, as G4GDMLParser will most likely crash
        #
        # ex. {
        #     "ParWorld1": {
        #         "Type": "DefaultWorld", # default
        #         "ExternalDetectorEmbedder": "ExtDetEmb1",
        #         "CustomSimulation": "MyCustomSimCreator",
        #         "ExportGDML: {
        #             'GDMLFileName': 'ParWorld1.gdml',
        #             'GDMLFileNameOverwrite': True,
        #             'GDMLExportSD': True,
        #             'GDMLExportEnergyCuts': True,
        #         },
        #     },
        # },
        #
        "ParallelPhysics": {},
        #
        # ex. {
        #     "ParWorld1": {
        #         "Type": "DefaultParallelPhysics", # default
        #         "LayeredMass": "", # default
        #     },
        # },
    }

    _external_embedders = []

    def attach(self, dettool):
        """Takes care of setting up the right tools and factories responsible
        for the parallel geometries as defined in ``ParallelWorlds`` property.

        :param dettool: Detector construction tool, should be
            ``GiGaMTDetectorConstructionFAC``
        """
        algs = []
        worlds = self.getProp("ParallelWorlds")
        if type(worlds) is dict:
            par_worlds_tools = []
            for world_name, props in worlds.items():
                self._check_props(world_name, props)
                factype = props.get("Type")
                if not factype:
                    log.warning(
                        "No factory type specified for {}. Using default world factory".format(
                            world_name
                        )
                    )
                    factype = "DefaultParallelWorld"
                fac_conf = getattr(Configurables, factype)
                fac = fac_conf(
                    world_name,
                    **self._refine_props(
                        props,
                        [
                            "Type",
                            "ExternalDetectorEmbedder",
                            "CustomSimulation",
                            "ExportGDML",
                        ],
                    )
                )

                embedder_name = props.get("ExternalDetectorEmbedder")
                if embedder_name:
                    from Configurables import ExternalDetectorEmbedder

                    embedder = ExternalDetectorEmbedder(embedder_name)
                    embedder.embed(fac)
                    algs += embedder.activate_hits_alg()  # no slot for now!
                    algs += embedder.activate_moni_alg()  # no slot for now!
                    self._external_embedders.append(embedder_name)

                # Add custom simulation models for parallel geometry
                cust_sim_creator_name = props.get("CustomSimulation")
                if cust_sim_creator_name:
                    from Configurables import CustomSimulation

                    CustomSimulation(cust_sim_creator_name).create(fac)

                # Save as a GDML File
                gdml_export = props.get("ExportGDML")
                if gdml_export:
                    if type(gdml_export) is not dict:
                        raise RuntimeError(
                            "ExportGDML should be a dictionary of options"
                        )
                    else:
                        for name, value in gdml_export.items():
                            if name.startswith("GDML"):
                                setattr(fac, name, value)
                            else:
                                raise RuntimeError("GDML options start with GDML")

                dettool.addTool(fac, name=world_name)
                par_worlds_tools.append(getattr(dettool, world_name))
            dettool.ParallelWorlds = par_worlds_tools
        return algs

    def attach_physics(self, modular_list):
        """Takes care of setting up the right tools and factories responsible
        for the parallel physics factories that correspond to the parallel worlds.
        All these properties should be provided in ``ParallelPhysics`` property.

        :param modular_list: Modular physics list tool, should be
            ``GiGaMTModularPhysListFAC``
        """
        physics = self.getProp("ParallelPhysics")
        if type(physics) is dict:
            for world_name, phys_props in physics.items():
                name = world_name + "Physics"
                self._check_props(name, phys_props)
                factype = phys_props.get("Type")
                factype = "DefaultParallelPhysics"
                if not factype:
                    log.warning(
                        "No factory type specified for {}. Using default physics world factory".format(
                            world_name
                        )
                    )
                    factype = "DefaultParallelPhysics"
                if factype == "DefaultParallelPhysics":
                    phys_props["WorldName"] = world_name
                fac_conf = getattr(Configurables, factype)
                pwph = fac_conf(name, **self._refine_props(phys_props))
                modular_list.addTool(pwph)
                modular_list.PhysicsConstructors.append(getattr(modular_list, name))

                # Add custom simulation physics in the parallel world
                cust_sim_creator_name = self.getProp("ParallelWorlds")[world_name].get(
                    "CustomSimulation"
                )
                if cust_sim_creator_name:
                    from Configurables import CustomSimulation

                    CustomSimulation(cust_sim_creator_name).attach_physics(
                        modular_list, world_name
                    )

    def _refine_props(self, props, keys_to_refine=["Type"]):
        return {key: prop for key, prop in props.items() if key not in keys_to_refine}

    def _check_props(self, name, props):
        if type(props) is not dict:
            raise RuntimeError(
                "ERROR: Dictionary of {} properties not provided.".format(name)
            )
