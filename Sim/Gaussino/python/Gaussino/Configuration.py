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

import time
from Gaudi.Configuration import (
    log,
    appendPostConfigAction,
)

# Configurables (do NOT use 'from Configurables' here)
from Gaussino.Utilities import GaussinoConfigurable
from Gaussino.Generation import GaussinoGeneration
from Gaussino.Simulation import GaussinoSimulation
from Gaussino.Geometry import GaussinoGeometry


class Gaussino(GaussinoConfigurable):
    """Main Configurable of Gaussino.

    .. warning::
        Some of these options have to be revisited as they
        might be inactive or redundant.

    :var Histograms: default: ``"DEFAULT"``
    :vartype Histograms: str, optional

    :var DatasetName: default: ``"Gaussino"``
    :vartype DatasetName: str, optional

    :var DatasetNameForced: default: ``False``
    :vartype DatasetNameForced: bool, optional

    :var DataType: default: ``""``
    :vartype DataType: str, optional

    :var SpilloverPaths: default: ``[]``
    :vartype SpilloverPaths: list, optional

    :var Phases: default: ``["Generator","Simulation"]``
    :vartype Phases: list, optional

    :var OutputType: default: ``'SIM'``
    :vartype OutputType: str, optional

    :var EnablePack: default: ``True``
    :vartype EnablePack: bool, optional

    :var DataPackingChecks: default: ``True``
    :vartype DataPackingChecks: bool, optional

    :var WriteFSR: default: ``True``
    :vartype WriteFSR: bool, optional

    :var MergeGenFSR: default: ``False``
    :vartype MergeGenFSR: bool, optional

    :var Debug: default: ``False``
    :vartype Debug: bool, optional

    :var BeamPipe: default: ``"BeamPipeOn"``
    :vartype BeamPipe: str, optional

    :var ReplaceWithGDML: default:
        ``[ { "volsToReplace": [], "gdmlFile": "" } ]``
    :vartype ReplaceWithGDML: list, optional

    :var RandomGenerator: default: ``'Ranlux'``
    :vartype RandomGenerator: str, optional

    :var EvtMax: default: ``-1``
    :vartype EvtMax: int, optional

    :var EnableHive: default: ``True``
    :vartype EnableHive: bool, optional

    :var ReDecay: default: ``False``
    :vartype ReDecay: bool, optional

    :var ThreadPoolSize: default: ``1``
    :vartype ThreadPoolSize: int, optional

    :var EventSlots: default: ``1``
    :vartype EventSlots: int, optional

    :var ConvertEDM: default: ``False``
    :vartype ConvertEDM: bool, optional

    :var ForceRandomEngine: default: ``'NONE'``
    :vartype ForceRandomEngine: str, optional

    :var ParticleTable: default: ``'$GAUSSINOROOT/data/ParticleTable.txt'``
    :vartype ParticleTable: str, optional
    """

    __used_configurables__ = [
        GaussinoGeneration,
        GaussinoSimulation,
        GaussinoGeometry,
    ]

    MT_PROPERTIES = {
        "EnableHive": True,
        "ThreadPoolSize": 1,
        "EventSlots": 1,
        "Geant4WorkerThreads": 1,
    }

    GENERAL_PROPERTIES = {
        "Histograms": "DEFAULT",
        "DatasetName": "Gaussino",
        "DatasetNameForced": False,
        # FIXME: Spillover not supported yet
        # "SpilloverPaths": [],
        "Phases": ["Generator", "Simulation"],
        "OutputType": "SIM",
        # FIXME: FSR not supported yet
        # "WriteFSR": True,
        # "MergeGenFSR": False,
        "EvtMax": -1,
        "ReDecay": False,
        "ConvertEDM": False,
        "ParticleTable": "$GAUSSINOROOT/data/ParticleTable.txt",
        "Debug": False,
    }

    __slots__ = {
        **GENERAL_PROPERTIES,
        **MT_PROPERTIES,
    }

    def __apply_configuration__(self):
        self._set_debug_mode()
        self._check_options_compatibility()

        # MT options
        self._setup_hive()
        self._setup_geant4MT()

        # Services
        self._set_particle_property_service()
        self._set_data_service()
        self._set_auditor_service()
        self._set_redecay_service()
        self._set_histogram_service()

        # Phases
        self._configure_generation_phase()
        self._configure_simulation_phase()

        # EDM conversion
        self._configure_edm_conversion()

        from Configurables import ApplicationMgr

        ApplicationMgr().EvtMax = self.getProp("EvtMax")
        ApplicationMgr().EvtSel = "NONE"

    def _set_debug_mode(self):
        if self.getProp("Debug"):
            log.setLevel("DEBUG")

            def debug_all_configurables():
                from GaudiKernel.Configurable import Configurable
                from Gaudi.Configuration import DEBUG

                for conf in Configurable.allConfigurables.values():
                    try:
                        conf.OutputLevel = DEBUG
                    except AttributeError:
                        pass

            appendPostConfigAction(debug_all_configurables)

    def _check_options_compatibility(self):
        if self.getProp("EvtMax") <= 0:
            msg = "EvtMax must be >= 0"
            log.error(msg)
            raise ValueError(msg)

    def _setup_hive(self):
        """Enable Hive event loop manager
        this is a very similar method as in LHCbApp
        """
        if not self.getProp("EnableHive"):
            # FIXME: Running without GaudiHive has not been tested
            #        and may lead to unexpected behaviour
            #        this is disabled for now
            log.error("EnableHive must be set. Running without "
                      "GaudiHive has not been tested and may lead to"
                      "unexpected behaviour")
            raise ValueError("EnableHive must be set.")
        from Configurables import (
            HiveWhiteBoard,
            ApplicationMgr,
            HiveSlimEventLoopMgr,
            AvalancheSchedulerSvc,
        )

        whiteboard = HiveWhiteBoard("EventDataSvc")
        whiteboard.EventSlots = self.getProp("EventSlots")
        ApplicationMgr().ExtSvc.insert(0, whiteboard)

        scheduler = AvalancheSchedulerSvc()
        eventloopmgr = HiveSlimEventLoopMgr(SchedulerName=scheduler)

        # initialize hive settings if not already set
        self.propagateProperty("ThreadPoolSize", eventloopmgr)
        scheduler.ThreadPoolSize = self.getProp("ThreadPoolSize")
        ApplicationMgr().EventLoop = eventloopmgr

    def _setup_geant4MT(self):
        from Configurables import GiGaMT

        GiGaMT().NumberOfWorkerThreads = self.getProp("Geant4WorkerThreads")

    def _set_particle_property_service(self):
        from Configurables import (
            ApplicationMgr,
            LHCb__ParticlePropertySvc,
        )

        log.debug("Configuring ParticlePropertySvc")
        ppservice = LHCb__ParticlePropertySvc()
        ppservice.ParticlePropertiesFile = self.getProp("ParticleTable")
        ApplicationMgr().ExtSvc += [ppservice]

    def _set_data_service(self):
        from Configurables import ApplicationMgr, EventDataSvc

        log.debug("Configuring EventDataSvc")
        datasvc = EventDataSvc("EventDataSvc")
        datasvc.ForceLeaves = True
        datasvc.RootCLID = 1
        ApplicationMgr().ExtSvc += [datasvc]

    def _set_auditor_service(self):
        from Configurables import (
            ApplicationMgr,
            AuditorSvc,
        )

        log.debug("Configuring AuditorSvc")
        ApplicationMgr().ExtSvc += ["AuditorSvc"]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += ["TimingAuditor"]

    def _set_redecay_service(self):
        if not self.getProp("ReDecay"):
            return
        from Configurables import (
            ApplicationMgr,
            ReDecaySvc,
        )

        log.debug("Configuring ReDecaySvc")
        redecaysvc = ReDecaySvc()
        redecaysvc.EvtMax = self.getProp("EvtMax")
        ApplicationMgr().ExtSvc += [redecaysvc]

    def _set_histogram_service(self):
        from Configurables import (
            RootHistCnv__PersSvc,
            ApplicationMgr,
        )

        log.debug("Configuring HistogramPersistencySvc")
        ApplicationMgr().HistogramPersistency = "ROOT"
        RootHistCnv__PersSvc().ForceAlphaIds = True
        hist_opt = self.getProp("Histograms").upper()
        if hist_opt not in ["NONE", "DEFAULT"]:
            msg = f"Unknown Histograms option '{hist_opt}'."
            log.error(msg)
            raise ValueError(msg)
        if hist_opt == "NONE":
            log.warning("No histograms will be produced")
            return

        # Use a default histogram file name if not already set
        from Gaudi.Configuration import HistogramPersistencySvc

        hst_prs_svc = HistogramPersistencySvc()
        if not hst_prs_svc.isPropertySet("OutputFile"):
            histos_name = self._get_output_name() + "-histos.root"
            hst_prs_svc.OutputFile = histos_name

    def _configure_edm_conversion(self):
        if not self.getProp("ConvertEDM"):
            return
        log.debug("Configuring EDM conversion.")
        from Configurables import ApplicationMgr

        ApplicationMgr().TopAlg += self.edm_algorithms(self.getProp("ReDecay"))

    @staticmethod
    def edm_algorithms(redecay=False):
        """Simple utility function to create and configure the
        EDM conversion algorithms, it is static as it can be use
        by externa apps
        """
        from Configurables import (
            CheckMCStructure,
            MCTruthMonitor,
        )

        if redecay:
            log.debug("Adding ReDecayMCTruthToEDM")
            from Configurables import ReDecayMCTruthToEDM

            conv = ReDecayMCTruthToEDM()
        else:
            log.debug("Adding MCTruthToEDM")
            from Configurables import MCTruthToEDM

            conv = MCTruthToEDM()
        return [
            conv,
            CheckMCStructure(),
            MCTruthMonitor("MainMCTruthMonitor", HistoProduce=True),
        ]

    def _configure_generation_phase(self):
        phases = self.getProp("Phases")
        if "Generator" not in phases:
            msg = "Must have Generator phase"
            log.error(msg)
            raise ValueError(msg)
        if "Simulation" not in phases:
            GaussinoGeneration.only_generation_phase = True
        self.propagateProperty("EvtMax", GaussinoGeneration())
        GaussinoGeneration.redecay = self.getProp("ReDecay")
        GaussinoGeneration.output_name = self._get_output_name()

    def _configure_simulation_phase(self):
        GaussinoSimulation.redecay = self.getProp("ReDecay")
        if "Simulation" not in self.getProp("Phases"):
            GaussinoSimulation._only_generation_phase = True

    def _get_output_name(self):
        """
        Build a name for the output file, based on input options.
        Combines DatasetName, EventType, Number of events and Date
        """
        output_name = self.getProp("DatasetName")
        if self.getProp("DatasetNameForced"):
            return output_name
        if not output_name:
            output_name = "Gaussino"
        evt_type = GaussinoGeneration.eventType()
        if evt_type:
            output_name += "-" + self.eventType()
        if self.getProp("EvtMax") > 0:
            output_name += f"-{self.getProp('EvtMax')}ev"
        file_id = str(time.localtime().tm_year)
        if time.localtime().tm_mon < 10:
            file_id += "0"
        file_id += str(time.localtime().tm_mon)
        if time.localtime().tm_mday < 10:
            file_id += "0"
        file_id += str(time.localtime().tm_mday)
        output_name += "-" + file_id
        return output_name
