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
from Gaudi.Configuration import log

from Gaussino.Utilities import (
    GaussinoConfigurable,
    get_set_configurable,
)


class GaussinoGeneration(GaussinoConfigurable):
    """Configurable for the Generation phase in Gaussino.

    :var BeamMomentum: default: ``3.5 * units.TeV``
    :vartype BeamMomentum: float, optional

    :var BeamHCrossingAngle: default: ``-0.520 * units.mrad``
    :vartype BeamHCrossingAngle: float, optional

    :var BeamVCrossingAngle: default: ``0.0``
    :vartype BeamVCrossingAngle: float, optional

    :var BeamEmittance: default: ``0.0037 * units.mm``
    :vartype BeamEmittance: float, optional

    :var BeamBetaStar: default: ``3.1 * units.m``
    :vartype BeamBetaStar: float, optional

    :var BeamLineAngles: default:
        ``[-0.075 * units.mrad, 0.035 * units.mrad]``
    :vartype BeamLineAngles: list, optional

    :var InteractionPosition: default:
        ``[0.459 * units.mm, -0.015 * units.mm,
        0.5 * units.mm]``
    :vartype InteractionPosition: list, optional

    :var BunchRMS: default: ``82.03 * units.mm``
    :vartype BunchRMS: float, optional

    :var Luminosity: default:
        ``2.47e29 / (units.cm2 * units.s)``
    :vartype Luminosity: float, optional

    :var TotalCrossSection: default: ``91.1 * units.millibarn``
    :vartype TotalCrossSection: float, optional

    :var WriteHepMC: default: ``False``
    :vartype WriteHepMC: bool, optional

    :var GenMonitor: default: ``False``
    :vartype GenMonitor: bool, optional

    :var ParticleGun: default: ``False``
    :vartype ParticleGun: bool, optional

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

    """

    __required_configurables__ = [
        "Gaussino",
    ]

    __slots__ = {
        "BeamMomentum": 3.5 * units.TeV,
        "BeamHCrossingAngle": -0.520 * units.mrad,
        "BeamVCrossingAngle": 0.0,
        "BeamEmittance": 0.0037 * units.mm,
        "BeamBetaStar": 3.1 * units.m,
        "BeamLineAngles": [-0.075 * units.mrad, 0.035 * units.mrad],
        "InteractionPosition": [
            0.459 * units.mm,
            -0.015 * units.mm,
            0.5 * units.mm,
        ],
        "BunchRMS": 82.03 * units.mm,
        "Luminosity": 2.47e29 / (units.cm2 * units.s),
        "TotalCrossSection": 91.1 * units.millibarn,
        # FIXME: beam particles are now fixed in LHCbDefaults.cmd
        #        to be meade generic!
        # "B2Momentum": 3.5 * units.TeV,
        # "B1Particle": "p",
        # "B2Particle": "p",
        "WriteHepMC": False,
        "GenMonitor": False,
        "ParticleGun": False,
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
    }

    # internal options to be set by Gaussino
    only_generation_phase = False
    redecay = False
    output_name = False

    def __apply_configuration__(self):
        """Main configuration method for the generation phase."""
        seq = []
        if self.getProp("ParticleGun"):
            seq += self._configure_pgun()
        else:
            seq += self._configure_generation()

        seq += self._configure_rnd_init()
        seq += self._configure_gen_monitor()
        seq += self._configure_hepmc_writer()

        if self.only_generation_phase:
            seq += self._configure_genonly()

        from Configurables import ApplicationMgr

        ApplicationMgr().TopAlg += seq

    def _configure_generation(self) -> list:
        """Configuration method for the generation other than
        a particle gun.
        """
        from Configurables import (
            Generation,
            ReDecayGeneration,
        )

        seq = []
        self._set_beam_properties()
        gen_alg = Generation
        if self.redecay:
            gen_alg = ReDecayGeneration()
        gen_alg = gen_alg()

        # -> sample generation
        sgt = self._set_sample_generation_tool(gen_alg)
        # -> decay tool
        self._set_decay_tool(gen_alg)
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

        if self.redecay:
            seq.append(self._set_redecay_signal_generation())
        return seq

    def _set_beam_parameters(self):
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
        )
        ApplicationMgr().ExtSvc.append(svc)

    def _set_sample_generation_tool(self, gen_alg):
        sgt = get_set_configurable(
            gen_alg, "SampleGenerationTool", self.getProp("SampleGenerationTool")
        )
        sgt_opts = self.getProp("SampleGenerationToolOpts")
        for option, value in self.getProp("SampleGenerationToolOpts").items():
            sgt.setProp(option, value)
        return sgt

    def _set_cut_tool(self, signal_tool):
        cut_tool = self.getProp("CutTool")
        if cut_tool:
            ct = get_set_configurable(signal_tool, "CutTool", cut_tool)
            ct_opts = self.getProp("CutToolOpts")
            for option, value in ct_opts.items():
                ct.setProp(option, value)
        else:
            signal_tool.CutTool = ""

    def _set_decay_tool(self, gen_alg):
        gen_alg.DecayTool = self.getProp("DecayTool")
        try:
            gen_alg.SampleGenerationTool.DecayTool = self.getProp("DecayTool")
        except AttributeError:
            pass

    def _set_full_gen_evt_cut_tool(self, gen_alg):
        tool = self.getProp("FullGenEventCutTool")
        if tool:
            ct = get_set_configurable(gen_alg, "FullGenEventCutTool", tool)
            ct_opts = self.getProp("FullGenEventCutToolOpts")
            for option, value in ct_opts.items():
                ct.setProp(option, value)
        else:
            gen_alg.FullGenEventCutTool = ""

    def _set_production_tool(self, signal_tool):
        tool = self.getProp("ProductionTool")
        prod = get_set_configurable(signal_tool, "ProductionTool", tool)
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
            prod.NThreads = self.threads

    def _set_pileup_tool(self, gen_alg):
        gen_alg.PileUpTool = self.getProp("PileUpTool")

    def _set_vertex_smearing_tool(self, gen_alg):
        gen_alg.VertexSmearingTool = "BeamSpotSmearVertexWithSvc"

    def _set_redecay_signal_generation(self):
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
        self._set_decay_tool(siggen_alg)
        # -> cut tool
        self._set_cut_tool(sgt)
        # -> full gen event cut tool
        self._set_full_gen_evt_cut_tool(siggen_alg)
        # -> production tool
        prod = get_set_configurable(sgt, "ProductionTool", "ReDecayProduction")
        # -> pileup tool
        siggen_alg.PileUpTool = "ReDecayPileUp"
        # -> vertex smearing tool
        siggen_alg.VertexSmearingTool = ""
        return siggen_alg

    def _configure_pgun(self) -> list:
        """Simple utility function to create and configure an instance of particle gun"""
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

    def _configure_rnd_init(self) -> list:
        """Simple utility function to create and configure an instance GenRndInit"""
        from Configurables import (
            GenRndInit,
            GenReDecayInit,
            SeedingTool,
        )

        conf = GenRndInit
        if self.redecay:
            conf = GenReDecayInit

        conf().addTool(SeedingTool, name="SeedingTool")
        return [conf()]

    def _configure_gen_monitor(self) -> list:
        """Simple utility function to create and configure a GenMonitorAlg instance"""
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
        """Simple utility function to create and configure a HepMCinstance"""
        if not self.getProp("WriteHepMC"):
            return []
        from Configurables import HepMCWriter

        alg = HepMCWriter()
        alg.Input = "/Event/Gen/HepMCEvents"
        if hasattr(alg, "Writer"):
            writer = alg.Writer
        else:
            writer = "WriterRootTree"
        log.debug(f"Using HepMCWriter: {writer}")
        if writer in ["WriterRootTree", "WriterRoot"]:
            alg.OutputFileName = f"{self.output_name}-HepMC.root"
        elif writer == "WriterAscii":
            alg.OutputFileName = f"{self.output_name}-HepMC.txt"
        elif writer == "WriterHEPEVT":
            alg.OutputFileName = f"{self.output_name}-HepMC.evt"
        else:
            msg = "Unknown HepMCWriter file extension."
            log.error(msg)
            raise ValueError(msg)
        return [alg]

    def _configure_genonly(self) -> list:
        """Method that is used when only the generation phase
        is used.
        """
        from Configurables import (
            SkipSimAlg,
            ReDecaySkipSimAlg,
        )

        alg_conf = SkipSimAlg
        if self.redecay:
            alg_conf = ReDecaySkipSimAlg
        tool = get_set_configurable(alg_conf(), "HepMCConverter")
        try:
            # FIXME: Michal M. this needs a bit more of investigation
            tool.CheckParticle = False
        except AttributeError:
            pass
        return [alg_conf()]

    @staticmethod
    def eventType():
        from Configurables import Generation

        evtType = ""
        if Generation("Generation").isPropertySet("EventType"):
            evtType = str(Generation("Generation").EventType)
        return evtType
