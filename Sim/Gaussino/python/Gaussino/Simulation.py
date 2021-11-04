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
"""
Utilities to configure the Simulation step of Gaussino
"""
import Configurables
from Gaudi.Configuration import ConfigurableUser, Configurable, ApplicationMgr
from Gaussino.Utilities import gigaService
from Gaussino.SimUtils import configure_giga_alg, append_truth_actions
from GaudiKernel.SystemOfUnits import mm, km


class SimPhase(ConfigurableUser):
    """Configurable for the Simulation phase in Gaussino. Does not implement
    a self.__apply_configuration__ itself. Instead, all member functions are
    explicitly called during the configuration of Gaussino()"""

    __slots__ = {
        "DebugCommunication": False,
        "TrackTruth": True,
        "G4BeginRunCommand":
        ["/tracking/verbose 0", "/process/eLoss/verbose 0"],
        "G4EndRunCommand": [],
        # physics related properties
        "PhysicsConstructors": [],
        "CutForElectron": -1. * km,
        "CutForPositron": -1 * km,
        "CutForGamma": -1 * km,
        "DumpCutsTable": False,
        # geometry related properties
        "GeometryService": "",
        "SensDetMap": {},
        "ExtraGeoTools": [],
        "ExportGDML": {},
        "ExternalDetectorEmbedder": "",
        "ParallelGeometry": False,
    }

    def __init__(self, name=Configurable.DefaultName, **kwargs):
        kwargs["name"] = name
        super(SimPhase, self).__init__(*(), **kwargs)

    def _addConstructorsWithNames(self, tool, joint_names):
        for joint_name in joint_names:
            if '/' in joint_name:
                template, name = joint_name.split('/')
                tool.addTool(getattr(Configurables, template), name=name)

    # @brief Set the given property in another configurable object
    #  @param other The other configurable to set the property for
    #  @param name  The property name
    def setOtherProp(self, other, name):
        self.propagateProperty(name, other)

    # @brief Set the given properties in another configurable object
    #  @param other The other configurable to set the property for
    #  @param names The property names
    def setOtherProps(self, other, names):
        self.propagateProperties(names, other)

    def configure_phase(self):
        seq = []
        gigaService(debugcommunication=self.getProp('DebugCommunication'))

        from Configurables import GiGaMT
        giga = GiGaMT()

        giga_alg = configure_giga_alg()
        seq += [giga_alg]

        self.set_base_physics(giga)
        geo_algs = self.set_base_detector_geometry(giga)
        seq += geo_algs

        ApplicationMgr().TopAlg += seq
        if self.getProp('TrackTruth'):
            if self.getProp('DebugCommunication'):
                append_truth_actions(OutputLevel=-10)
            else:
                append_truth_actions()

    def set_base_physics(self, giga):
        from Configurables import GiGaMTModularPhysListFAC
        gmpl = giga.addTool(GiGaMTModularPhysListFAC(), name="ModularPL")
        giga.PhysicsListFactory = getattr(giga, "ModularPL")

        phys_list = self.getProp('PhysicsConstructors')
        gmpl.PhysicsConstructors = phys_list
        self._addConstructorsWithNames(gmpl, phys_list)

        # Add parallel physics
        par_geo = self.getProp("ParallelGeometry")
        if par_geo:
            from Configurables import ParallelGeometry
            ParallelGeometry().attach_physics(gmpl)

    def set_base_detector_geometry(self, giga):
        from Configurables import GiGaMTDetectorConstructionFAC
        algs = []
        dettool = giga.addTool(
            GiGaMTDetectorConstructionFAC(), name="DetConst")
        giga.DetectorConstruction = getattr(giga, "DetConst")

        dettool.GiGaMTGeoSvc = self.getProp("GeometryService")
        dettool.SensDetVolumeMap = self.getProp("SensDetMap")
        extra_tools = self.getProp("ExtraGeoTools")
        dettool.AfterGeoConstructionTools = extra_tools
        self._addConstructorsWithNames(dettool, extra_tools)

        # Add external detectors geometries
        # TODO: external geometry was prepared to operate with spillover
        # but it is not available yet
        # so for now there are no 'slot' param in the algos
        embedder_name = self.getProp("ExternalDetectorEmbedder")
        if embedder_name:
            from Configurables import ExternalDetectorEmbedder
            embedder = ExternalDetectorEmbedder(embedder_name)
            embedder.embed(dettool)
            algs += embedder.activate_hits_alg()  # no slot for now!
            algs += embedder.activate_moni_alg()  # no slot for now!

        # Add parallel geometry
        par_geo = self.getProp("ParallelGeometry")
        if par_geo:
            from Configurables import ParallelGeometry
            par_geo = ParallelGeometry()
            algs += par_geo.attach(dettool)
            #for par_ext_emd in par_geo._external_embedders:
            #    self._external_embedders.append(par_ext_emd)
            #par_geo.world_to_gdml(giga.RunSeq)

        # Save as a GDML File
        gdml_export = self.getProp("ExportGDML")
        if type(gdml_export) is not dict:
            raise RuntimeError("ExportGDML should be a dictionary of options")
        else:
            for name, value in gdml_export.items():
                if name.startswith('GDML'):
                    setattr(dettool, name, value)
                else:
                    raise RuntimeError("GDML options start with GDML")
        return algs
