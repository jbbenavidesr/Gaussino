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
# example file for a user-defined momentum spectrum
# Michel De Cian, 15.11.2013


from Gauss.Configuration import *

# --Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber = 1082

# --Number of events
nEvts = 10000
LHCbApp().EvtMax = nEvts

##############################################

# --Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *

importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam4000GeV-md100-2012-nu2.5.py")

from Configurables import LHCbApp

# --Set database tags using those for Sim08
LHCbApp().DDDBtag = "Sim08-20130503-1"
LHCbApp().CondDBtag = "Sim08-20130503-1-vc-md100"

#############################################

from Gauss.Configuration import *

Gauss().Production = "PGUN"
Gauss().DatasetName = "test"

Gauss().Phases = ["Generator", "GenToMCTree"]

#####################################################################

from Configurables import FlatNParticles, MomentumSpectrum, ParticleGun

pgun = ParticleGun()

pgun.ParticleGunTool = "MomentumSpectrum"
pgun.addTool(MomentumSpectrum, name="MomentumSpectrum")

pgun.MomentumSpectrum.PdgCodes = [511, -511]
pgun.MomentumSpectrum.InputFile = "BSpectrum.root"
pgun.MomentumSpectrum.HistogramPath = "BSpectrum"
pgun.MomentumSpectrum.OutputLevel = 3

pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.addTool(FlatNParticles, name="FlatNParticles")
