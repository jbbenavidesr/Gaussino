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
from Configurables import (
    Generation,
    Inclusive,
    MinimumBias,
    Pythia8Production,
    SignalPlain,
    SignalRepeatedHadronization,
    Special,
)

Pythia8TurnOffFragmentation = ["HadronLevel:all = off"]

# this is only a preliminary tuning that should probably need to be updated
Pythia8NewFrag = [
    "StringZ:useNonstandardB = on",
    "StringZ:aNonstandardB = 1.01",
    "StringZ:bNonstandardB = 0.46",
    "StringZ:rFactB = 0",
    "StringZ:useNonstandardC = on",
    "StringZ:aNonstandardC = 1.075",
    "StringZ:bNonstandardC = 0.694",
    "StringZ:rFactC = 0",
]

gen = Generation("Generation")

gen.addTool(MinimumBias, name="MinimumBias")
gen.MinimumBias.ProductionTool = "Pythia8Production"
gen.MinimumBias.addTool(Pythia8Production, name="Pythia8Production")
gen.MinimumBias.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.MinimumBias.Pythia8Production.Commands += Pythia8NewFrag

gen.addTool(Inclusive, name="Inclusive")
gen.Inclusive.ProductionTool = "Pythia8Production"
gen.Inclusive.addTool(Pythia8Production, name="Pythia8Production")
gen.Inclusive.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.Inclusive.Pythia8Production.Commands += Pythia8NewFrag

gen.addTool(SignalPlain, name="SignalPlain")
gen.SignalPlain.ProductionTool = "Pythia8Production"
gen.SignalPlain.addTool(Pythia8Production, name="Pythia8Production")
gen.SignalPlain.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.SignalPlain.Pythia8Production.Commands += Pythia8NewFrag

gen.addTool(SignalRepeatedHadronization, name="SignalRepeatedHadronization")
gen.SignalRepeatedHadronization.ProductionTool = "Pythia8Production"
gen.SignalRepeatedHadronization.addTool(Pythia8Production, name="Pythia8Production")
gen.SignalRepeatedHadronization.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.SignalRepeatedHadronization.Pythia8Production.Commands += (
    Pythia8TurnOffFragmentation
)
gen.SignalRepeatedHadronization.Pythia8Production.Commands += Pythia8NewFrag

gen.addTool(Special, name="Special")
gen.Special.ProductionTool = "Pythia8Production"
gen.Special.addTool(Pythia8Production, name="Pythia8Production")
gen.Special.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.Special.Pythia8Production.Commands += Pythia8NewFrag
gen.Special.PileUpProductionTool = "Pythia8Production/Pythia8PileUp"
gen.Special.addTool(Pythia8Production, name="Pythia8PileUp")
gen.Special.Pythia8PileUp.Tuning = "LHCbDefault.cmd"
gen.Special.Pythia8PileUp.Commands += Pythia8NewFrag
gen.Special.ReinitializePileUpGenerator = False

# Use same generator and configuration for spillover
from Configurables import Gauss

spillOverList = Gauss().getProp("SpilloverPaths")
for slot in spillOverList:
    genSlot = Generation("Generation" + slot)
    genSlot.addTool(MinimumBias, name="MinimumBias")
    genSlot.MinimumBias.ProductionTool = "Pythia8Production"
    genSlot.MinimumBias.addTool(Pythia8Production, name="Pythia8Production")
    genSlot.MinimumBias.Pythia8Production.Commands += Pythia8NewFrag
    genSlot.MinimumBias.Pythia8Production.Tuning = "LHCbDefault.cmd"
