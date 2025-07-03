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

from GaudiKernel import PhysicalConstants as constants
from GaudiKernel import SystemOfUnits as units


def set_monitoring(
    opts: dict,
    max_energy_hist: float = 51.0 * units.GeV,
    ntuple_name: str = "",
    training_data: bool = True,
    fast_simulation: bool = False,
    collector_hits_location: str = "MC/CaloChallenge/Collector/Hits",
    fast_hits_location: str = "MC/CaloChallenge/Calo/FastHits",
    detailed_hits_location: str = "MC/CaloChallenge/Calo/Hits",
):
    from Configurables import ApplicationMgr, Gaussino

    moni_conf = None
    appMgr = ApplicationMgr()
    if fast_simulation:
        from Configurables import CaloChallengeDetailedAndFastSimMonitoring

        moni_conf = CaloChallengeDetailedAndFastSimMonitoring
    else:
        from Configurables import CaloChallengeDetailedSimMonitoring

        moni_conf = CaloChallengeDetailedSimMonitoring
    moni = moni_conf(
        "CaloChallengeMonitoring",
        CollectorHitsLocation=collector_hits_location,
        CaloHitsLocation0=detailed_hits_location,
        CellSizeRho=opts["size_of_rho_cells"],
        CellSizeZ=opts["size_of_z_cells"],
        CellSizePhi=2.0 * constants.pi / opts["nb_of_phi_cells"],
        CellNumRho=opts["nb_of_rho_cells"],
        CellNumPhi=opts["nb_of_phi_cells"],
        CellNumZ=opts["nb_of_z_cells"],
        MaxEnergyHist=max_energy_hist,
    )
    if training_data:
        from Configurables import (
            Gsino__CaloChallenge__TrainingDataCollector as CaloChallengeCollector,
        )
        from Configurables import NTupleSvc

        if not ntuple_name:
            ntuple_name = f"{Gaussino()._get_output_name()}-TrainingData.root"
        collector = CaloChallengeCollector(
            CollectorHitsLocation=collector_hits_location,
            CaloHitsLocation=detailed_hits_location,
        )
        NTupleSvc().Output = [f"FILE1 DATAFILE='{ntuple_name}' TYP='ROOT' OPT='NEW'"]
        appMgr.TopAlg.append(collector)
    if fast_simulation:
        moni.CaloHitsLocation1 = fast_hits_location
        if training_data:
            collector.CaloHitsLocation = fast_hits_location
    appMgr.TopAlg.append(moni)
