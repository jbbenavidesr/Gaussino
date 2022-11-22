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
from Gaussino.Utilities import (
    add_constructors_with_names,
    GaussinoConfigurable,
)

# Configurables (do NOT use 'from Configurables' here)
from ExternalDetector.Configuration import ExternalDetectorEmbedder
from ParallelGeometry.Configuration import ParallelGeometry


class GaussinoGeometry(GaussinoConfigurable):
    """Configurable for the geometry in Gaussino.
    nstead, all member functions are
    explicitly called during the configuration of Gaussino()

    General properties

    :var DebugCommunication: default: ``False``
    :vartype DebugCommunication: bool, optional

    :var TrackTruth: default: ``False``
    :vartype TrackTruth: bool, optional

    :var G4BeginRunCommand: default:
        ``["/tracking/verbose 0", "/process/eLoss/verbose 0"]``
    :vartype G4BeginRunCommand: bool, optional

    :var G4EndRunCommand: default: ``[]``
    :vartype G4EndRunCommand: bool, optional

    Physics related properties

    :var CutForElectron: default: ``-1. * km``
    :vartype CutForElectron: float, optional

    :var CutForGamma: default: ``-1. * km``
    :vartype CutForGamma: float, optional

    :var CutForPositron: default: ``-1. * km``
    :vartype CutForPositron: float, optional

    :var DumpCutsTable: default: ``False``
    :vartype DumpCutsTable: bool, optional

    :var PhysicsConstructors: default: ``[]``, list of the factories used
        to attach physics to the main modular list
    :vartype PhysicsConstructors: list, optional

    Geometry related properties

    :var GeometryService: default: ``""``, name of the geometry service, if
        not provided then some custom geometry must be provided or using the
        external detector package
    :vartype GeometryService: str, optional

    :var SensDetMap: default: ``{}``, additional map of  sensitive volumes
        to volumes added on top of any geometry service
    :vartype SensDetMap: dict, optional

    :var ExtraGeoTools: default: ``[]``, additional list of tools related to
        the geometry
    :vartype ExtraGeoTools: list, optional

    :var ExportGDML: default: ``{}``
    :vartype ExportGDML: dict, optional

    :var ImportGDML: default: ``[]``
    :vartype ImportGDML: list, optional

    :var ExternalDetectorEmbedder: default: ``""``, name of the embedder used
        when creating external geometry
    :vartype ExternalDetectorEmbedder: str, optional

    :var ParallelGeometry: default: ``False``
    :vartype ParallelGeometry: bool, optional
    """

    __slots__ = {
        "GeometryService": "",
        "SensDetMap": {},
        "ExtraGeoTools": [],
        "ExportGDML": {},
        "ImportGDML": [],
        "ExternalDetectorEmbedder": "",
    }

    __required_configurables__ = [
        "Gaussino",
    ]

    __used_configurables__ = []

    def __apply_configuration__(self):
        log.debug("Configuring GaussinoGeometry")
        from Configurables import (
            GiGaMTDetectorConstructionFAC,
            GiGaMT,
        )

        giga = GiGaMT()
        dettool = giga.addTool(
            GiGaMTDetectorConstructionFAC(),
            name="DetConst",
        )
        giga.DetectorConstruction = getattr(giga, "DetConst")

        dettool.GiGaMTGeoSvc = self.getProp("GeometryService")
        dettool.SensDetVolumeMap = self.getProp("SensDetMap")
        extra_tools = self.getProp("ExtraGeoTools")
        dettool.AfterGeoConstructionTools = extra_tools
        add_constructors_with_names(dettool, extra_tools)

        self._set_external_detector(dettool)
        self._set_parallel_geometry(dettool)
        self._set_gdml_import(dettool)
        self._set_gdml_export(dettool)

    def _set_external_detector(self, dettool):
        # Add external detectors geometries
        # TODO: external geometry was prepared to operate with spillover
        # but it is not available yet
        # so for now there are no 'slot' param in the algos
        embedder_name = self.getProp("ExternalDetectorEmbedder")
        if not embedder_name:
            return
        log.debug(f"-> Configuring external detector: {embedder_name}")
        algs = []
        embedder = ExternalDetectorEmbedder(embedder_name)
        embedder.embed(dettool)
        algs += embedder.activate_hits_alg()  # no slot for now!
        algs += embedder.activate_moni_alg()  # no slot for now!
        from Configurables import ApplicationMgr

        ApplicationMgr().TopAlg += algs

    def _set_parallel_geometry(self, dettool):
        par_geo = ParallelGeometry()
        if not par_geo.getProp("ParallelWorlds"):
            return
        log.debug("-> Configuring geometry in parallel worlds")
        algs = []
        algs += par_geo.attach(dettool)
        # for par_ext_emd in par_geo._external_embedders:
        #    self._external_embedders.append(par_ext_emd)
        # par_geo.world_to_gdml(giga.RunSeq)
        from Configurables import ApplicationMgr

        ApplicationMgr().TopAlg += algs

    def _set_gdml_export(self, dettool):
        gdml_export = self.getProp("ExportGDML")
        if type(gdml_export) is not dict:
            raise RuntimeError("ExportGDML should be a dictionary of options")
        for name, value in gdml_export.items():
            if name.startswith("GDML"):
                setattr(dettool, name, value)
            else:
                raise RuntimeError("GDML options start with GDML")

    def _set_gdml_import(self, dettool):
        gdml_imports = self.getProp("ImportGDML")
        if type(gdml_imports) is not list:
            raise RuntimeError("ImportGDML should be a list of dicts")
        from Configurables import GDMLReader

        for gdml_import in gdml_imports:
            if type(gdml_import) is not dict:
                raise RuntimeError("Elements of ImportGDML should be dicts")
            name = gdml_import["GDMLFileName"] + "Reader"
            reader = GDMLReader(name, **gdml_import)
            dettool.addTool(reader, name=name)
            dettool.GDMLReaders.append("GDMLReader/" + name)
