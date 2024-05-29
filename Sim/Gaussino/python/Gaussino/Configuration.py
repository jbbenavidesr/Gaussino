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

from Gaudi.Configuration import appendPostConfigAction, log
from Gaussino.Generation import GaussinoGeneration
from Gaussino.Geometry import GaussinoGeometry
from Gaussino.Simulation import GaussinoSimulation

# Configurables (do NOT use 'from Configurables' here)
from Gaussino.Utilities import GaussinoConfigurable
from Gaussino.Visualization import GaussinoVisualization


class Gaussino(GaussinoConfigurable):
    """
    Main Configurable of Gaussino. It is dedicated to the
    confiuration of general properties. Please, visit other
    configurables for more options:

        - :class:`GaussinoGeneration <Gaussino.Generation.GaussinoGeneration>` (configuration of the generation phase)
        - :class:`GaussinoSimulation <Gaussino.Simulation.GaussinoSimulation>` (configuration of the simulation phase)
        - :class:`GaussinoGeometry <Gaussino.Geometry.GaussinoGeometry>` (configuration of the geometry)

    **Main**

    :var EvtMax: default: ``-1``,
        no. of event to produce, must be > 0
    :vartype EvtMax: int, required

    :var Phases: default: ``["Generator","Simulation"]``,
        possible only: ``["Generator", "Simulation"]``
        or ``["Generation"]``
    :vartype Phases: list, optional

    :var FirstEventNumber: default: ``1``
    :vartype FirstEventNumber: int, optional

    :var RunNumber: default: ``1``
    :vartype RunNumber: int, optional

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

    :var FirstTimingEvent: default: ``1``
    :vartype FirstTimingEvent: int, optional

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
        GaussinoVisualization,
    ]

    __slots__ = {
        # MAIN
        "EvtMax": -1,
        "Phases": ["Generator", "Simulation"],
        "FirstEventNumber": 1,
        "RunNumber": 1,
        # OUTPUT
        "Histograms": "DEFAULT",
        "DatasetName": "Gaussino",
        "DatasetNameForced": False,
        "OutputType": "SIM",
        # Multi-threading
        "EnableHive": True,
        "ThreadPoolSize": 1,
        "EventSlots": 1,
        "FirstTimingEvent": 1,
        # OTHER
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
        """Main configuration method for Gaussino. It is called as the first one, and
        then propagates the properties to:
        - :class:`GaussinoGeneration <Gaussino.Generation.GaussinoGeneration>`,
        - :class:`GaussinoSimulation <Gaussino.Simulation.GaussinoSimulation>`,
        - :class:`GaussinoGeometry <Gaussino.Geometry.GaussinoGeometry>`.
        """
        self._set_debug_mode()
        self._check_options_compatibility()

        # MT options
        self._setup_hive()
        self._setup_geant4MT()

        # Seed
        self._configure_rnd_init()

        # Services
        self._configure_services()

        # Phases
        self._configure_generation_phase()
        # self._configure_simulation_phase()

        # EDM conversion
        self._configure_edm_conversion()

        from Configurables import ApplicationMgr

        ApplicationMgr().EvtMax = self.getProp("EvtMax")
        ApplicationMgr().EvtSel = "NONE"

        # ensure the configurables are called
        for conf in self.__used_configurables__:
            conf()

    def _set_debug_mode(self):
        """Sets up the debug mode in python logger and all the configurables."""
        if self.getProp("Debug"):
            log.setLevel("DEBUG")

            def debug_all_configurables():
                from Gaudi.Configuration import DEBUG
                from GaudiKernel.Configurable import Configurable

                for conf in Configurable.allConfigurables.values():
                    try:
                        conf.OutputLevel = DEBUG
                    except AttributeError:
                        pass

            appendPostConfigAction(debug_all_configurables)

    def _check_options_compatibility(self):
        """Checks the general compatibility of the properties.

        Raises:
            ValueError: if ``EvtMax`` is not provided
        """
        evtMax = self.getProp("EvtMax")
        if evtMax <= 0:
            raise ValueError("EvtMax must be > 0")

        timing_event = self.getProp("FirstTimingEvent")
        if timing_event != 1:
            if timing_event >= evtMax:
                raise ValueError("FirstTimingEvent must be < EvtMax")
            if timing_event < self.getProp("ThreadPoolSize") + 2:
                raise ValueError("FirstTimingEvent must be > ThreadPoolSize + 2")

    def _setup_hive(self):
        """Enables Hive event loop manager.
        this is a very similar method as in LHCbApp

        Raises:
            ValueError: if ``EnableHive`` is disabled
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
            ApplicationMgr,
            AvalancheSchedulerSvc,
            GenRndInit,
            HiveSlimEventLoopMgr,
            HiveWhiteBoard,
        )

        whiteboard = HiveWhiteBoard(
            "EventDataSvc",
            EventSlots=self.getProp("EventSlots"),
            ForceLeaves=True,
            RootCLID=1,
        )

        ApplicationMgr().ExtSvc.insert(0, whiteboard)

        scheduler = AvalancheSchedulerSvc()
        eventloopmgr = HiveSlimEventLoopMgr(SchedulerName=scheduler)

        # initialize hive settings if not already set
        self.propagateProperty("ThreadPoolSize", eventloopmgr)
        scheduler.ThreadPoolSize = self.getProp("ThreadPoolSize")
        ApplicationMgr().EventLoop = eventloopmgr

        # propagate the barrier to GenRndInit
        GenRndInit().FirstTimingEvent = self.getProp("FirstTimingEvent")

    def _configure_rnd_init(self):
        """Creates the algorithm responsible for the seed generation. It is either
        ``GenRndInit`` or ``GenReDecayInit``
        """
        from Configurables import (
            ApplicationMgr,
            GenReDecayInit,
            GenRndInit,
            SeedingTool,
        )

        conf = GenRndInit
        if self.getProp("ReDecay"):
            conf = GenReDecayInit

        if conf().isPropertySet("RunNumber"):
            # disable direct setting of the GenRndInit().RunNumber
            # -> the reason is that it is also needed by the ODIN generator in Gauss
            msg = "'RunNumber' must be set internally via GaussinoGeneration()!"
            log.error(msg)
            raise ValueError(msg)
        conf(
            RunNumber=self.getProp("RunNumber"),
            FirstEventNumber=self.getProp("FirstEventNumber"),
        )
        conf().addTool(SeedingTool, name="SeedingTool")
        ApplicationMgr().TopAlg.append(conf())

    def _setup_geant4MT(self):
        """Sets up the Geant4 multi-threading options."""
        from Configurables import GiGaMT

        GiGaMT().NumberOfWorkerThreads = self.getProp("ThreadPoolSize")

    def _configure_services(self):
        """Sets up the general Gaudi services needed in Gaussino."""
        from Configurables import ApplicationMgr

        log.debug("Configuring services")

        self._set_particle_property_service()
        self._set_auditor_service()
        self._set_redecay_service()
        self._set_histogram_service()

        # other services
        ApplicationMgr().ExtSvc += [
            "Gaudi::Monitoring::MessageSvcSink",
        ]

    def _set_particle_property_service(self):
        """Sets up the particle property service.

        .. todo ::
            LHCb project dependency!
        """
        from Configurables import ApplicationMgr, LHCb__ParticlePropertySvc

        log.debug("Configuring ParticlePropertySvc")
        ppservice = LHCb__ParticlePropertySvc()
        ppservice.ParticlePropertiesFile = self.getProp("ParticleTable")
        ApplicationMgr().ExtSvc += [ppservice]

    def _set_auditor_service(self):
        """Sets up the auditor service: ``AuditorSvc``."""
        from Configurables import ApplicationMgr, AuditorSvc

        log.debug("Configuring AuditorSvc")
        ApplicationMgr().ExtSvc += ["AuditorSvc"]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += ["TimingAuditor"]

    def _set_redecay_service(self):
        """Sets up a dedicated service when using ReDecay: ``ReDecaySvc``."""
        if not self.getProp("ReDecay"):
            return
        from Configurables import ApplicationMgr, ReDecaySvc

        log.debug("Configuring ReDecaySvc")
        redecaysvc = ReDecaySvc()
        redecaysvc.EvtMax = self.getProp("EvtMax")
        ApplicationMgr().ExtSvc += [redecaysvc]

    def _set_histogram_service(self):
        """Sets up the service responsible for producing histograms.

        Raises:
            ValueError: when unknown option in the list of ``Historgrams``
        """
        from Configurables import (
            ApplicationMgr,
            HistogramPersistencySvc,
            RootHistCnv__PersSvc,
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
        """Sets up EDM algorithms for Gaussino."""
        if not self.getProp("ConvertEDM"):
            return
        log.debug("Configuring EDM conversion.")
        from Configurables import ApplicationMgr

        ApplicationMgr().TopAlg += self.edm_algorithms(self.getProp("ReDecay"))

    @staticmethod
    def edm_algorithms(redecay=False):
        """Sets up a special algorithm responsible for linking ``MCParticles``
        and ``MCVertices``  to ``MCHits`` via ``LinkedParticle``. It is either:

        - ``ReDecayMCTruthToEDM`` when in ReDeacay mode,
        - ``MCTruthToEDM`` otherwise.

        Args:
            redecay (bool, optional): Using Redecay or not. Defaults to False.

        Returns:
            list: list of needed EDM algorithms
        """
        from Configurables import (
            CheckMCStructure,
            MCTruthMonitor,
            MCTruthToEDM,
            ReDecayMCTruthToEDM,
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
        """Configures a subset of properties of the generation phase and propagates
        them to :class:`GaussinoGeneration <Gaussino.Generation.GaussinoGeneration>`.

        Raises:
            ValueError: if the ``Generator`` phase is not provided
        """
        self.propagateProperty("EvtMax", GaussinoGeneration())

    def _get_output_name(self):
        """Build a name for the output file, based on input options.
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
