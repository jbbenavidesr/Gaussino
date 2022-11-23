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

    :var B2Momentum: default: ``3.5 * units.TeV``
    :vartype B2Momentum: float, optional

    :var B1Particle: default: ``'p'``
    :vartype B1Particle: str, optional

    :var B2Particle: default: ``'p'``
    :vartype B2Particle: str, optional

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
        "B2Momentum": 3.5 * units.TeV,
        "B1Particle": "p",
        "B2Particle": "p",
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
        # Algorithm that produces the actual HepMC by talking to stuff
        SampleGenerationTool = self.getProp("SampleGenerationTool")
        ProductionTool = self.getProp("ProductionTool")
        DecayTool = self.getProp("DecayTool")
        CutTool = self.getProp("CutTool")
        FullGenEventCutTool = self.getProp("FullGenEventCutTool")
        PileUpTool = self.getProp("PileUpTool")

        beaminfoService()
        from Configurables import Gaussino

        if Gaussino().getProp("ReDecay"):
            from Configurables import ReDecayGeneration

            gen_alg = ReDecayGeneration()
        else:
            from Configurables import Generation

            gen_alg = Generation()
        sgt = get_set_configurable(
            gen_alg, "SampleGenerationTool", SampleGenerationTool
        )
        sgt_opts = self.getProp("SampleGenerationToolOpts")
        for n, v in sgt_opts.items():
            sgt.setProp(n, v)
        try:
            sgt.DecayTool = DecayTool
        except:
            pass
        try:
            if CutTool != "":
                ct = get_set_configurable(sgt, "CutTool", CutTool)
                ct_opts = self.getProp("CutToolOpts")
                for n, v in ct_opts.items():
                    ct.setProp(n, v)
            else:
                sgt.CutTool = ""
        except Exception as e:
            log.error("Could not configure CutTool", e)
        if FullGenEventCutTool != "":
            ct = get_set_configurable(
                gen_alg, "FullGenEventCutTool", FullGenEventCutTool
            )
            ct_opts = self.getProp("FullGenEventCutToolOpts")
            for n, v in ct_opts.items():
                ct.setProp(n, v)
        else:
            gen_alg.FullGenEventCutTool = ""
        prod = get_set_configurable(sgt, "ProductionTool", ProductionTool)
        if ProductionTool in ["Pythia8Production", "Pythia8ProductionMT"]:
            # For now keep it only for Pythia, but potentially in future we
            # want to do this for all possible production tools
            prot_opts = self.getProp("ProductionToolOpts")
            for n, v in prot_opts.items():
                prod.setProp(n, v)
            prod.BeamToolName = "CollidingBeamsWithSvc"

        if ProductionTool == "Pythia8ProductionMT":
            from Configurables import Gaussino

            prod.NThreads = Gaussino().ThreadPoolSize

        gen_alg.PileUpTool = PileUpTool
        gen_alg.VertexSmearingTool = "BeamSpotSmearVertexWithSvc"
        gen_alg.DecayTool = DecayTool

        seq += [gen_alg]

        # Now do it all again for the signal part
        if Gaussino().getProp("ReDecay"):
            from Configurables import ReDecaySignalGeneration

            siggen_alg = ReDecaySignalGeneration()

            siggen_alg.HepMCEventLocation = "Gen/SignalDecayTree"
            siggen_alg.GenCollisionLocation = "Gen/SignalCollisions"
            siggen_alg.GenHeaderOutputLocation = "Gen/SignalGenHeader"

            seq += [siggen_alg]
            sgt = get_set_configurable(
                siggen_alg, "SampleGenerationTool", "SignalPlain"
            )
            sgt.RevertWhenBackward = False  # Don't invert in the redecay part
            siggen_alg.GenFSRLocation = ""
            sgt.GenFSRLocation = ""
            sgt_opts = self.getProp("SampleGenerationToolOpts")
            if "SignalPIDList" in sgt_opts:
                sgt.setProp("SignalPIDList", sgt_opts["SignalPIDList"])
            else:
                # FIXME: First only support signal like org tool
                log.error("Original sample generation tool not of signal type")
            try:
                sgt.DecayTool = DecayTool
            except:
                pass
            try:
                if CutTool != "":
                    ct = get_set_configurable(sgt, "CutTool", CutTool)
                    ct_opts = self.getProp("CutToolOpts")
                    for n, v in ct_opts.items():
                        ct.setProp(n, v)
                else:
                    sgt.CutTool = ""
            except Exception as e:
                log.error("Could not configure CutTool", e)
            if FullGenEventCutTool != "":
                ct = get_set_configurable(
                    siggen_alg, "FullGenEventCutTool", FullGenEventCutTool
                )
                ct_opts = self.getProp("FullGenEventCutToolOpts")
                for n, v in ct_opts.items():
                    ct.setProp(n, v)
            else:
                siggen_alg.FullGenEventCutTool = ""
            prod = get_set_configurable(sgt, "ProductionTool", "ReDecayProduction")

            siggen_alg.PileUpTool = "ReDecayPileUp"
            siggen_alg.VertexSmearingTool = ""
            siggen_alg.DecayTool = DecayTool
        return seq

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
        filename = self.output_name + "-HepMC"
        if hasattr(alg, "Writer"):
            writer = alg.Writer
        else:
            writer = "WriterRootTree"
        print("writer={}".format(writer))
        if writer in ["WriterRootTree", "WriterRoot"]:
            print("Setting root file")
            alg.OutputFileName = filename + ".root"
        elif writer in ["WriterAscii"]:
            alg.OutputFileName = filename + ".txt"
        elif writer in ["WriterHEPEVT"]:
            alg.OutputFileName = filename + ".evt"
        else:
            print("Unknown writer name specified, not going to write")
            alg.OutputFileName = ""
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
