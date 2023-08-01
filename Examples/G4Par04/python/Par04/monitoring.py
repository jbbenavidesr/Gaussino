###############################################################################
# (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Configurables import (
    ApplicationMgr,
    Gaussino__G4Par04__Monitoring as moni,
    NTupleSvc,
)


def set_monitoring(
    opts: dict,
    max_energy_hist: float = 51.0 * units.GeV,
    ntuple_name: str = "Par04Hits.root",
    training_data: bool = True,
):
    ApplicationMgr().TopAlg.append(
        moni(
            "Par04Monitoring",
            CollectorHitsLocation="MC/Par04/Collector/Hits",
            CaloHitsLocation="MC/Par04/Calo/Hits",
            CellSizeRho=opts["size_of_rho_cells"],
            CellSizeZ=opts["size_of_z_cells"],
            CellSizePhi=2.0 * constants.pi / opts["nb_of_phi_cells"],
            CellNumRho=opts["nb_of_rho_cells"],
            CellNumPhi=opts["nb_of_phi_cells"],
            CellNumZ=opts["nb_of_z_cells"],
            MaxEnergyHist=max_energy_hist,
            TrainingData=training_data,
        )
    )
    if training_data:
        NTupleSvc().Output = [f"FILE1 DATAFILE='{ntuple_name}' TYP='ROOT' OPT='NEW'"]
