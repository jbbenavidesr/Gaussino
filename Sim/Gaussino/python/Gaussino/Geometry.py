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
from GaudiKernel.ConfigurableMeta import ConfigurableMeta
from Gaussino.Utilities import (
    add_constructors_with_names,
    GaussinoConfigurable,
)

# Configurables (do NOT use 'from Configurables' here)
from ExternalDetector.Configuration import ExternalDetectorEmbedder
from ParallelGeometry.Configuration import ParallelGeometry


class GaussinoGeometry(GaussinoConfigurable):
    """Configurable for the geometry in Gaussino.

    **Main**

    :var GeometryService: default: ``""``, name of the geometry service, if
        not provided then some custom geometry must be provided or using the
        external detector package
    :vartype GeometryService: str, optional

    :var SensDetMap: default: ``{}``, additional map of sensitive volumes
        to volumes added on top of any geometry service
    :vartype SensDetMap: dict, optional

    :var ExtraGeoTools: default: ``[]``, additional list of tools related to
        the geometry
    :vartype ExtraGeoTools: list, optional

    **Handling GDML files**

    :var ExportGDML: default: ``{}``
    :vartype ExportGDML: dict, optional

    :var ImportGDML: default: ``[]``
    :vartype ImportGDML: list, optional

    **External Detector**

    :var ExternalDetectorEmbedder: default: ``""``, name of the embedder used
        when creating external geometry
    :vartype ExternalDetectorEmbedder: str, optional
    """

    __required_configurables__ = [
        "Gaussino",
    ]

    __slots__ = {
        # MAIN
        "GeometryService": "",
        "SensDetMap": {},
        "ExtraGeoTools": [],
        # HANDLING GDML FILES
        "ExportGDML": {},
        "ImportGDML": [],
        # EXTERNAL DETECTOR
        "ExternalDetectorEmbedder": "",
    }

    # internal options to be set by Gaussino
    only_generation_phase = False
    """ options set internally by Gaussino() """

    def __apply_configuration__(self):
        """Main configuration method for the geometry to be used in Gaussino.
        It applies the properties of the geoemtry right after the simulation configurable
        :class:`GaussinoSimulation <Gaussino.Simulation.GaussinoSimulation>`, generation
        :class:`GaussinoGeneration <Gaussino.Generation.GaussinoGeneration>` and the main configurable:
        :class:`Gaussino <Gaussino.Configuration.Gaussino>`.
        """
        log.debug("Configuring GaussinoGeometry")
        if GaussinoGeometry.only_generation_phase:
            log.debug("-> Only the generation phase, skipping.")
            return
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

        algs = []
        algs += self._set_external_detector(dettool)
        algs += self._set_parallel_geometry(dettool)
        self._set_gdml_import(dettool)
        self._set_gdml_export(dettool)

        from Configurables import ApplicationMgr
        ApplicationMgr().TopAlg += algs

    def _set_external_detector(self, dettool: ConfigurableMeta) -> list:
        """Sets up the external detector package if requested.
        See more info in a dedicated section below.

        Args:
            dettool (ConfigurableMeta): detector constructor
                ``GiGaMTDetectorConstructionFAC("DetConst")``

        Returns:
            list: list of algorithms
        """
        # Add external detectors geometries
        # TODO: external geometry was prepared to operate with spillover
        # but it is not available yet
        # so for now there are no 'slot' param in the algos
        embedder_name = self.getProp("ExternalDetectorEmbedder")
        if not embedder_name:
            return []
        log.debug(f"-> Configuring external detector: {embedder_name}")
        algs = []
        embedder = ExternalDetectorEmbedder(embedder_name)
        embedder.embed(dettool)
        algs += embedder.activate_hits_alg()  # no slot for now!
        algs += embedder.activate_moni_alg()  # no slot for now!
        return algs

    def _set_parallel_geometry(self, dettool: ConfigurableMeta) -> list:
        """Sets up the parallel geoemtry package if requested. It works only
        with the external detector package. See more info in a dedicated section below.

        Args:
            dettool (ConfigurableMeta): detector constructor
                ``GiGaMTDetectorConstructionFAC("DetConst")``

        Returns:
            list: list of algorithms
        """
        par_geo = ParallelGeometry()
        if not par_geo.getProp("ParallelWorlds"):
            return []
        log.debug("-> Configuring geometry in parallel worlds")
        return par_geo.attach(dettool)

    def _set_gdml_export(self, dettool: ConfigurableMeta):
        """Sets up the properties needed to export the geometry to a GDML file.
        See more info in a dedicated section below.

        Args:
            dettool (ConfigurableMeta): detector constructor
                ``GiGaMTDetectorConstructionFAC("DetConst")``

        Raises:
            RuntimeError: if ``ExportGDML`` not a dictionary
            RuntimeError: if options of ``ExportGDML`` do not start with `GDML*`
        """
        gdml_export = self.getProp("ExportGDML")
        if type(gdml_export) is not dict:
            raise RuntimeError("ExportGDML should be a dictionary of options")
        for name, value in gdml_export.items():
            if name.startswith("GDML"):
                setattr(dettool, name, value)
            else:
                raise RuntimeError("GDML options start with GDML")

    def _set_gdml_import(self, dettool):
        """Sets up the properties needed to import geometry from a GDML file.
        The geometry can be added to the geometry created with a geometry service.
        See more info in a dedicated section below.

        Args:
            dettool (ConfigurableMeta): detector constructor
                ``GiGaMTDetectorConstructionFAC("DetConst")``

        Raises:
            RuntimeError: if ``ImportGDML`` not a dictionary
            RuntimeError: if values of ``ImportGDML`` are not dictionaries either
       """
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
