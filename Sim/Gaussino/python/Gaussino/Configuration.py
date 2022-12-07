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
    """
    Main Configurable of Gaussino. It is dedicated to the
    confiuration of general properties. Please, visit other
    configurables for more options:

        - GaussinoGeneration (configuration of the generation phase)
        - GaussinoSimulation (configuration of the simulation phase)
        - GaussinoGeometry (configuration of the geometry)

    **Main**

    :var EvtMax: default: ``-1``,
        no. of event to produce, must be > 0
    :vartype EvtMax: int, required

    :var Phases: default: ``["Generator","Simulation"]``,
        possible only: ``["Generator", "Simulation"]``
        or ``["Generation"]``
    :vartype Phases: list, optional

    **Output**

    :var Histograms: default: ``"DEFAULT"``
    :vartype Histograms: str, optional

    :var DatasetName: default: ``"Gaussino"``
    :vartype DatasetName: str, optional

    :var DatasetNameForced: default: ``False``
    :vartype DatasetNameForced: bool, optional

    :var OutputType: default: ``'SIM'``
    :vartype OutputType: str, optional

    **Multi-threading**

    :var EnableHive: default: ``True``,
        must be always set (for now)
    :vartype EnableHive: bool, optional

    :var ThreadPoolSize: default: ``1``
    :vartype ThreadPoolSize: int, optional

    :var EventSlots: default: ``1``
    :vartype EventSlots: int, optional

    :var TimingSkipAtStart: default: ``1``
    :vartype TimingSkipAtStart: int, optional

    **Other**

    :var Debug: default: ``False``,
        increase verbosity for the whole application
    :vartype Debug: bool, optional

    :var ReDecay: default: ``False``
    :vartype ReDecay: bool, optional

    :var ConvertEDM: default: ``False``
    :vartype ConvertEDM: bool, optional

    :var ParticleTable: default: ``'$GAUSSINOROOT/data/ParticleTable.txt'``
    :vartype ParticleTable: str, optional
    """

    __used_configurables__ = [
        GaussinoGeneration,
        GaussinoSimulation,
        GaussinoGeometry,
    ]

    __slots__ = {
        # Main
        "EvtMax": -1,
        "Phases": ["Generator", "Simulation"],
        # Output
        "Histograms": "DEFAULT",
        "DatasetName": "Gaussino",
        "DatasetNameForced": False,
        "OutputType": "SIM",
        # Multi-threading
        "EnableHive": True,
        "ThreadPoolSize": 1,
        "EventSlots": 1,
        "TimingSkipAtStart": 1,
        # Other
        "Debug": False,
        "ReDecay": False,
        "ConvertEDM": False,
        "ParticleTable": "$GAUSSINOROOT/data/ParticleTable.txt",
        # FIXME: Spillover not supported yet
        # "SpilloverPaths": [],
        # FIXME: FSR not supported yet
        # "WriteFSR": True,
        # "MergeGenFSR": False,
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

        # ensure the configurables are called
        for conf in self.__used_configurables__:
            conf()

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
            msg = "EvtMax must be > 0"
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
            log.error(
                "EnableHive must be set. Running without "
                "GaudiHive has not been tested and may lead to"
                "unexpected behaviour"
            )
            raise ValueError("EnableHive must be set.")
        from Configurables import (
            HiveWhiteBoard,
            ApplicationMgr,
            HiveSlimEventLoopMgr,
            AvalancheSchedulerSvc,
            GenRndInit,
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

        # propagate the barrier to GenRndInit
        GenRndInit().TimingSkipAtStart = self.getProp("TimingSkipAtStart")

    def _setup_geant4MT(self):
        from Configurables import GiGaMT

        GiGaMT().NumberOfWorkerThreads = self.getProp("ThreadPoolSize")

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
            HistogramPersistencySvc,
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
            ReDecayMCTruthToEDM,
            MCTruthToEDM,
        )

        conv = MCTruthToEDM
        if redecay:
            conv = ReDecayMCTruthToEDM
        return [
            conv(),
            CheckMCStructure(),
            MCTruthMonitor("MainMCTruthMonitor", HistoProduce=True),
        ]

    def _configure_generation_phase(self):
        phases = self.getProp("Phases")
        if "Generator" not in phases:
            msg = "Must have the generator phase"
            log.error(msg)
            raise ValueError(msg)
        if "Simulation" not in phases:
            GaussinoGeneration.only_generation_phase = True
        self.propagateProperty("EvtMax", GaussinoGeneration())
        GaussinoGeneration.redecay = self.getProp("ReDecay")
        GaussinoGeneration.output_name = self._get_output_name()
        GaussinoGeneration.threads = self.getProp("ThreadPoolSize")

    def _configure_simulation_phase(self):
        GaussinoSimulation.redecay = self.getProp("ReDecay")
        if "Simulation" not in self.getProp("Phases"):
            GaussinoSimulation.only_generation_phase = True
            GaussinoGeometry.only_generation_phase = True

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
            output_name += "-" + evt_type
        if self.getProp("EvtMax") > 0:
            output_name += f"-{self.getProp('EvtMax')}ev"
        return f"{output_name}-{time.strftime('%Y%m%d')}"
