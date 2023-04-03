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

from GaudiKernel import SystemOfUnits as units
from GaudiKernel.ConfigurableMeta import ConfigurableMeta
from Gaudi.Configuration import log

from Gaussino.Utilities import (
    GaussinoConfigurable,
    get_set_configurable,
)


class GaussinoGeneration(GaussinoConfigurable):
    """Configurable for the Generation phase in Gaussino.

    **Main**

    :var WriteHepMC: default: ``False``
    :vartype WriteHepMC: bool, optional

    **Generation modules**

    :var SampleGenerationTool: default: ``'SignalPlain'``
    :vartype SampleGenerationTool: str, optional

    :var SampleGenerationToolOpts: default: ``{}``
    :vartype SampleGenerationToolOpts: dict, optional

    :var PileUpTool: default: ``'FixedLuminosityWithSvc'``
    :vartype PileUpTool: str, optional

    :var ProductionTool: default: ``'Pythia8ProductionMT'``,
        for Pythia8 there are 2 possibilities: ``'Pythia8Production'`` (shared)
        and ``'Pythia8ProductionMT'`` (thread-local) interface
    :vartype ProductionTool: str, optional

    :var ProductionToolOpts: default: ``{}``
    :vartype ProductionToolOpts: dict, optional

    :var DecayTool: default: ``''``
    :vartype DecayTool: str, optional

    :var CutTool: default: ``''``
    :vartype CutTool: str, optional

    :var CutToolOpts: default: ``{}``
    :vartype CutToolOpts: dict, optional

    :var FullGenEventTool: default: ``''``
    :vartype FullGenEventTool: str, optional

    :var FullGenEventToolOpts: default: ``{}``
    :vartype FullGenEventToolOpts: dict, optional

    **Monitoring**

    :var GenMonitor: default: ``False``
    :vartype GenMonitor: bool, optional

    **Particle gun options**

    :var ParticleGun: default: ``False``
    :vartype ParticleGun: bool, optional

    **Beam options**

    .. todo::

        Custom ``B1Particle`` and ``B2Particle`` are not available yet!

    :var BeamMomentum: default: ``3.5 * units.TeV``
    :vartype BeamMomentum: float, optional

    :var BeamHCrossingAngle: default: ``0.0 * units.mrad``
    :vartype BeamHCrossingAngle: float, optional

    :var BeamVCrossingAngle: default: ``0.0 * units.mrad``
    :vartype BeamVCrossingAngle: float, optional

    :var BeamEmittance: default: ``0.0037 * units.mm``
    :vartype BeamEmittance: float, optional

    :var BeamBetaStar: default: ``3.1 * units.m``
    :vartype BeamBetaStar: float, optional

    :var BeamLineAngles: default:
        ``[0.0 * units.mrad, 0.0 * units.mrad]``
    :vartype BeamLineAngles: list, optional

    :var InteractionPosition: default:
        ``[0.0 * units.mm, 0.0 * units.mm,
        0.0 * units.mm]``
    :vartype InteractionPosition: list, optional

    :var BunchRMS: default: ``82.03 * units.mm``
    :vartype BunchRMS: float, optional

    :var Luminosity: default:
        ``2.47e29 / (units.cm2 * units.s)``
    :vartype Luminosity: float, optional

    :var TotalCrossSection: default: ``91.1 * units.millibarn``
    :vartype TotalCrossSection: float, optional

    :var RevolutionFrequency: default: ``11.245 * units.kilohertz``
    :vartype RevolutionFrequency: float, optional
    """


    __slots__ = {
        # MAIN
        "WriteHepMC": False,
        # GENERATION MODULES
        "SampleGenerationTool": "SignalPlain",
        "SampleGenerationToolOpts": {},
        "PileUpTool": "FixedLuminosityWithSvc",
        "ProductionTool": "Pythia8ProductionMT",
        "ProductionToolOpts": {},
        "DecayTool": "",
        "CutTool": "",
        "CutToolOpts": {},
        "FullGenEventCutTool": "",
        "FullGenEventCutToolOpts": {},
        # MONITORING
        "GenMonitor": False,
        # PARTICLE GUN OPTIONS
        "ParticleGun": False,
        # BEAM OPTIONS
        "BeamMomentum": 3.5 * units.TeV,
        "BeamHCrossingAngle": 0.0 * units.mrad,
        "BeamVCrossingAngle": 0.0 * units.mrad,
        "BeamEmittance": 0.0037 * units.mm,
        "BeamBetaStar": 3.1 * units.m,
        "BeamLineAngles": [0.0 * units.mrad, 0.0 * units.mrad],
        "InteractionPosition": [
            0.0 * units.mm,
            0.0 * units.mm,
            0.0 * units.mm,
        ],
        "BunchRMS": 82.03 * units.mm,
        "Luminosity": 2.47e29 / (units.cm2 * units.s),
        "TotalCrossSection": 91.1 * units.millibarn,
        "RevolutionFrequency": 11.245 * units.kilohertz,
        # FIXME: beam particles are now fixed in LHCbDefaults.cmd
        #        to be meade generic!
        # "B2Momentum": 3.5 * units.TeV,
        # "B1Particle": "p",
        # "B2Particle": "p",
    }

    def __apply_configuration__(self):
        """Main configuration method for the generation phase.
        It applies the properties of the generation phase right after the main
        :class:`Gaussino <Gaussino.Configuration.Gaussino>` configurable, but before
        the detector transport and geometry configurables:
        :class:`GaussinoSimulation <Gaussino.Simulation.GaussinoSimulation>` and
        :class:`GaussinoGeometry <Gaussino.Geometry.GaussinoGeometry>`.
        """
        from Configurables import ApplicationMgr, Gaussino

        if "Generator" not in Gaussino().getProp("Phases"):
            log.debug("-> No generation phase, skipping.")
            return

        seq = []
        if self.getProp("ParticleGun"):
            seq += self._configure_pgun()
        else:
            seq += self._configure_generation()

        seq += self._configure_gen_monitor()
        seq += self._configure_hepmc_writer()

        if "Simulation" not in Gaussino().getProp("Phases"):
            seq += self._configure_genonly()

        ApplicationMgr().TopAlg += seq

    def _configure_generation(self) -> list:
        """Configuration method for the generation other than
        a particle gun. The idea is to set all the modules required for
        the main generation algorithm.

            - sample generation tool
            - decay tool,
            - cut tool,
            - production tool,
            - pile-up tool,
            - vertex smearing tool,

        The main generation algorithm could be either ``Generation`` or ``ReDecayGeneration``.

        Returns:
            list: list of algorithms
        """
        from Configurables import (
            Gaussino,
            Generation,
            ReDecayGeneration,
        )

        seq = []
        self._set_beam_parameters()
        gen_alg = Generation
        if Gaussino().getProp("ReDecay"):
            gen_alg = ReDecayGeneration()
        gen_alg = gen_alg()

        # -> sample generation
        sgt = self._set_sample_generation_tool(gen_alg)
        # -> decay tool
        self._set_decay_tool(gen_alg, sgt)
        # -> cut tool
        self._set_cut_tool(sgt)
        # -> full gen event cut tool
        self._set_full_gen_evt_cut_tool(gen_alg)
        # -> production tool
        self._set_production_tool(sgt)
        # -> pileup tool
        self._set_pileup_tool(gen_alg)
        # -> vertex smearing tool
        self._set_vertex_smearing_tool(gen_alg)
        seq.append(gen_alg)

        if Gaussino().getProp("ReDecay"):
            seq.append(self._set_redecay_signal_generation())
        return seq

    def _set_beam_parameters(self):
        """Sets up all the beam parametres of ``BeamInfoSvc`` service.
        """
        from Configurables import BeamInfoSvc, ApplicationMgr

        log.debug("Configuring BeamInfoSvc")
        xAngleBeamLine, yAngleBeamLine = self.getProp("BeamLineAngles")
        meanX, meanY, meanZ = self.getProp("InteractionPosition")
        svc = BeamInfoSvc(
            BeamEnergy=self.getProp("BeamMomentum"),
            HorizontalCrossingAngle=self.getProp("BeamHCrossingAngle"),
            VerticalCrossingAngle=self.getProp("BeamVCrossingAngle"),
            NormalizedEmittance=self.getProp("BeamEmittance"),
            BetaStar=self.getProp("BeamBetaStar"),
            HorizontalBeamlineAngle=xAngleBeamLine,
            VerticalBeamlineAngle=yAngleBeamLine,
            Luminosity=self.getProp("Luminosity"),
            TotalCrossSection=self.getProp("TotalCrossSection"),
            XLuminousRegion=meanX,
            YLuminousRegion=meanY,
            ZLuminousRegion=meanZ,
            BunchLengthRMS=self.getProp("BunchRMS"),
            RevolutionFrequency=self.getProp("RevolutionFrequency")
        )
        ApplicationMgr().ExtSvc.append(svc)

    def _set_sample_generation_tool(self, gen_alg) -> ConfigurableMeta:
        """Sets up the sample generation tool, i.e. ``MinimumBias``, ``Inclusive``, etc.

        Args:
            gen_alg (ConfigurableMeta): ``Generation`` or ``ReDecayGeneration`` algorithm

        Returns:
            ConfigurableMeta: ``SampleGenerationTool``
        """
        sgt = get_set_configurable(
            gen_alg, "SampleGenerationTool", self.getProp("SampleGenerationTool")
        )
        sgt_opts = self.getProp("SampleGenerationToolOpts")
        for option, value in sgt_opts.items():
            sgt.setProp(option, value)
        return sgt

    def _set_cut_tool(self, sgt):
        """Sets up the cut tool.

        Args:
            sgt (ConfigurableMeta): Sample generation tool configured earlier.
        """
        cut_tool = self.getProp("CutTool")
        if cut_tool:
            ct = get_set_configurable(sgt, "CutTool", cut_tool)
            ct_opts = self.getProp("CutToolOpts")
            for option, value in ct_opts.items():
                ct.setProp(option, value)
        else:
            sgt.CutTool = ""

    def _set_decay_tool(self, gen_alg, sgt):
        """Sets up the decay tool.

        Args:
            gen_alg (ConfigurableMeta): ``Generation`` or ``ReDecayGeneration`` algorithm
            sgt (ConfigurableMeta): Sample generation tool configured earlier
        """
        gen_alg.DecayTool = self.getProp("DecayTool")
        try:
            sgt.DecayTool = self.getProp("DecayTool")
        except AttributeError:
            pass

    def _set_full_gen_evt_cut_tool(self, gen_alg):
        """Sets up the ``FullGenEventCutTool``.

        Args:
            gen_alg (ConfigurableMeta): ``Generation`` or ``ReDecayGeneration`` algorithm
        """
        tool = self.getProp("FullGenEventCutTool")
        if tool:
            ct = get_set_configurable(gen_alg, "FullGenEventCutTool", tool)
            ct_opts = self.getProp("FullGenEventCutToolOpts")
            for option, value in ct_opts.items():
                ct.setProp(option, value)
        else:
            gen_alg.FullGenEventCutTool = ""

    def _set_production_tool(self, sgt):
        """Sets up the production tool.

        .. todo::

            So far, only ``Pythia8Production`` and ``Pythia8ProductionMT``
            are supported.

        Args:
            sgt (ConfigurableMeta): Sample generation tool configured earlier.

        Raises:
            ValueError: if unsupported ``ProductionTool`` provided.
        """
        tool = self.getProp("ProductionTool")
        prod = get_set_configurable(sgt, "ProductionTool", tool)
        if tool in ["Pythia8Production", "Pythia8ProductionMT"]:
            # FIXME: For now keep it only for Pythia, but potentially in future we
            # want to do this for all possible production tools
            prot_opts = self.getProp("ProductionToolOpts")
            for option, value in prot_opts.items():
                prod.setProp(option, value)
            prod.BeamToolName = "CollidingBeamsWithSvc"
        else:
            msg = "Unsupported production tool."
            log.error(msg)
            raise ValueError(msg)
        if tool == "Pythia8ProductionMT":
            from Configurables import Gaussino
            prod.NThreads = Gaussino().getProp("ThreadPoolSize")

    def _set_pileup_tool(self, gen_alg):
        """Sets up the pile-up tool.

        .. todo::

            Pass on the pile-up tool options via ``PileUpToolOpts``.

        Args:
            gen_alg (ConfigurableMeta): ``Generation`` or ``ReDecayGeneration`` algorithm
        """
        gen_alg.PileUpTool = self.getProp("PileUpTool")

    def _set_vertex_smearing_tool(self, gen_alg):
        """Sets up the vertex smearing tool.

        .. todo::

            Investigate if any options need to be passed via ``GaussinoGeneration``.

        Args:
            gen_alg (ConfigurableMeta): ``Generation`` or ``ReDecayGeneration`` algorithm
        """
        gen_alg.VertexSmearingTool = "BeamSpotSmearVertexWithSvc"

    def _set_redecay_signal_generation(self):
        """Sets up the ReDecay equivalent of the signal generation tool.

        .. todo::

            Needs testing and revising!

        Raises:
            ValueError: if ``SignalPIDList`` not in the options

        Returns:
            ConfigurableMeta: ``ReDecaySignalGeneration``
        """
        from Configurables import ReDecaySignalGeneration

        siggen_alg = ReDecaySignalGeneration()
        siggen_alg.HepMCEventLocation = "Gen/SignalDecayTree"
        siggen_alg.GenCollisionLocation = "Gen/SignalCollisions"
        siggen_alg.GenHeaderOutputLocation = "Gen/SignalGenHeader"
        siggen_alg.GenFSRLocation = ""

        # -> sample generation
        sgt = get_set_configurable(siggen_alg, "SampleGenerationTool", "SignalPlain")
        sgt.RevertWhenBackward = False  # Don't invert in the redecay part
        sgt.GenFSRLocation = ""
        sgt_opts = self.getProp("SampleGenerationToolOpts")
        if "SignalPIDList" in sgt_opts:
            sgt.setProp("SignalPIDList", sgt_opts["SignalPIDList"])
        else:
            # FIXME: First only support signal like org tool
            msg = "Original sample generation tool not of signal type"
            log.error(msg)
            raise ValueError(msg)
        # -> decay tool
        self._set_decay_tool(siggen_alg, sgt)
        # -> cut tool
        self._set_cut_tool(sgt)
        # -> full gen event cut tool
        self._set_full_gen_evt_cut_tool(siggen_alg)
        # -> production tool
        get_set_configurable(sgt, "ProductionTool", "ReDecayProduction")
        # -> pileup tool
        siggen_alg.PileUpTool = "ReDecayPileUp"
        # -> vertex smearing tool
        siggen_alg.VertexSmearingTool = ""
        return siggen_alg

    def _configure_pgun(self) -> list:
        """Sets up the ParticleGun algorithm.

        .. todo::
            Set up & test passing of the options to particle gun tools
            in a similar way that is done for the main generation algorithm.

        Raises:
            AttributeError: if ParticleGun chosen to be the main
                generation algorithm, but no options were passed to the
                configurable ``ParticleGun``

        Returns:
            list: list of algorithms
        """
        from Configurables import ParticleGun

        if "ParticleGun" not in ParticleGun.configurables:
            msg = (
                "The generation algorithm is set to use a ParticleGun, but no "
                "ParticleGun() configurable was registered! Make sure to include "
                "all the required tools!"
            )
            log.error(msg)
            raise AttributeError(msg)
        return [ParticleGun()]

    def _configure_gen_monitor(self) -> list:
        """Creates the monitoring algorithm for the generation step: ``GenMonitorAlg``.

        Returns:
            list: list of algorithms
        """
        if not self.getProp("GenMonitor"):
            return []
        from Configurables import GenMonitorAlg

        alg = GenMonitorAlg(
            "GenMonitorAlg",
            HistoProduce=True,
            Input="/Event/Gen/HepMCEvents",
        )
        return [alg]

    def _configure_hepmc_writer(self) -> list:
        """Creates an algorithm responsible for wrtiting the generator
        data in HepMC format to a file:

            - ``WriterRootTree`` to a root file,
            - ``WriterAsciiTree`` to a txt file,
            - ``WriterHEPEVT`` to an evt file.

        Raises:
            ValueError: if unknown writer provided

        Returns:
            list: list of algorithms
        """
        if not self.getProp("WriteHepMC"):
            return []
        from Configurables import Gaussino, HepMCWriter

        alg = HepMCWriter()
        alg.Input = "/Event/Gen/HepMCEvents"
        if hasattr(alg, "Writer"):
            writer = alg.Writer
        else:
            writer = "WriterRootTree"
        log.debug(f"Using HepMCWriter: {writer}")
        output_name = Gaussino()._get_output_name()
        if writer in ["WriterRootTree", "WriterRoot"]:
            alg.OutputFileName = f"{output_name}-HepMC.root"
        elif writer == "WriterAscii":
            alg.OutputFileName = f"{output_name}-HepMC.txt"
        elif writer == "WriterHEPEVT":
            alg.OutputFileName = f"{output_name}-HepMC.evt"
        else:
            msg = "Unknown HepMCWriter file extension."
            log.error(msg)
            raise ValueError(msg)
        return [alg]

    def _configure_genonly(self) -> list:
        """Configures a dedicated algorithm in case we skip the simulation part

            - ``ReDecaySkipSimAlg`` when used in ReDecay mode,
            - ``SkipSimAlg``  otherwise.

        Returns:
            list: list of algorithms
        """
        from Configurables import (
            Gaussino,
            SkipSimAlg,
            ReDecaySkipSimAlg,
        )

        alg_conf = SkipSimAlg
        if Gaussino().getProp("ReDecay"):
            alg_conf = ReDecaySkipSimAlg
        tool = get_set_configurable(alg_conf(), "HepMCConverter")
        try:
            # FIXME: Michal M. this needs a bit more of investigation
            tool.CheckParticle = False
        except AttributeError:
            pass
        return [alg_conf()]

    @staticmethod
    def eventType() -> str:
        """Checks is the event type was set, otherwise returns
        an empty string

        .. todo::

            Rethink this.

        Returns:
            str: event type
        """
        from Configurables import Generation

        evtType = ""
        if Generation("Generation").isPropertySet("EventType"):
            evtType = str(Generation("Generation").EventType)
        return evtType
