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
from multiprocessing import cpu_count
from Configurables import GiGaMT, ApplicationMgr
giga = GiGaMT()

# Set of IronBox detector construction
from Configurables import GDMLConstructionFactory
giga.DetectorConstruction = "GDMLConstructionFactory"
dettool = giga.addTool(GDMLConstructionFactory,
                       "GDMLConstructionFactory")
dettool.GDML = "$LHCBFROMGDMLROOT/data/LHCb.gdml"
