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
from Configurables import LHCbConfigurableUser
from Gaudi.Configuration import log
import Configurables


class ParallelGeometry(LHCbConfigurableUser):

    __slots__ = {
        "ParallelWorlds": {},
        #
        # ex. {
        #     "ParWorld1": {
        #         "Type": "DefaultWorld", # default
        #         "ExternalDetectorEmbedder": "ExtDetEmb1",
        #     },
        # },
        #
        "ParallelPhysics": {},
        #
        # ex. {
        #     "ParWorld1": {
        #         "Type": "DefaultParallelPhysics", # default
        #     },
        # },
        #
        "SaveGDML": {}
        #
        # Note! Make sure that all your detectors have materials
        # i.e. material != nullptr, as G4GDMLParser will most likely crash
        #
        # ex. {
        #     "ParWorld1": {
        #         "Output": "ParWorld1.gdml",
        #         "ExportSD": True, # more options in GDMLRunAction
        #     },
        # }
    }

    _external_embedders = []

    def attach(self, dettool):
        algs = []
        worlds = self.getProp("ParallelWorlds")
        if type(worlds) is dict:
            par_worlds_tools = []
            for world_name, props in worlds.items():
                self._check_props(world_name, props)
                factype = props.get('Type')
                if not factype:
                    log.warning(
                        "No factory type specified for {}. Using default world factory"
                        .format(world_name))
                    factype = "DefaultParallelWorld"
                fac_conf = getattr(Configurables, factype)
                fac = fac_conf(
                    world_name,
                    **self._refine_props(props,
                                         ['Type', 'ExternalDetectorEmbedder']))

                embedder_name = props.get("ExternalDetectorEmbedder")
                if embedder_name:
                    from Configurables import ExternalDetectorEmbedder
                    embedder = ExternalDetectorEmbedder(embedder_name)
                    embedder.embed(fac)
                    algs += embedder.activate_hits_alg()  # no slot for now!
                    algs += embedder.activate_moni_alg()  # no slot for now!
                    self._external_embedders.append(embedder_name)

                dettool.addTool(fac, name=world_name)
                par_worlds_tools.append(getattr(dettool, world_name))
            dettool.ParallelWorlds = par_worlds_tools
        return algs

    def attach_physics(self, modular_list):
        physics = self.getProp("ParallelPhysics")
        if type(physics) is dict:
            for world_name, phys_props in physics.items():
                name = world_name + "Physics"
                self._check_props(name, phys_props)
                factype = phys_props.get('Type')
                # TODO: maybe there'll be a need to set a different physics list
                # for now it'll be always "DefaultParallelPhysicsFactory"
                log.warning(
                    "Physics Factory is fixed for now: DefaultParallelPhysics")
                factype = "DefaultParallelPhysics"
                # if not factype:
                #     log.warning("No factory type specified for {}. Using default physics world factory".format(world_name))
                #     factype = "DefaultParallelPhysics"
                if factype == "DefaultParallelPhysics":
                    phys_props["WorldName"] = world_name
                fac_conf = getattr(Configurables, factype)
                pwph = fac_conf(name, **self._refine_props(phys_props))
                modular_list.addTool(pwph)
                modular_list.PhysicsConstructors.append(
                    getattr(modular_list, name))

    def world_to_gdml(self, run_seq):
        world_gdmls = self.getProp("SaveGDML")
        if type(world_gdmls) is dict:
            from Configurables import GDMLRunAction
            for world_name, gdml_props in world_gdmls.items():
                name = "GDMLRunAction" + world_name
                self._check_props(name, gdml_props)
                gdml_props["ParallelWorldName"] = world_name
                gdml_tool = GDMLRunAction(name, **gdml_props)
                run_seq.addTool(gdml_tool, name)
                run_seq.Members.append(getattr(run_seq, name))

    def _refine_props(self, props, keys_to_refine=['Type']):
        return {
            key: prop
            for key, prop in props.items() if key not in keys_to_refine
        }

    def _check_props(self, name, props):
        if type(props) is not dict:
            raise RuntimeError(
                "ERROR: Dictionary of {} properties not provided.".format(
                    name))
