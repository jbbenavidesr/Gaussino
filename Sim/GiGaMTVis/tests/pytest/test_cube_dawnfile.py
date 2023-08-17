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
from Gaussino.pytest.helpers import run_gaudi
from Gaussino.pytest.options import cube, debug, em_physics, events_1, photon


def config():
    from Configurables import ExternalDetectorEmbedder, GaussinoVisualization
    from ExternalDetector.Materials import SILICA
    from GaudiKernel import SystemOfUnits as units

    external = ExternalDetectorEmbedder("ExternalDetectorEmbedder_0")
    external.Shapes["ExternalDetectorEmbedder_0_Cube"]["MaterialName"] = "Si"
    external.Materials["Si"] = SILICA

    GaussinoVisualization(
        Framework=["Geant4"],
        Driver="DAWNFILE",
        DrawGeometry=True,
        DrawTrajectories=True,
        DrawG4Hits=True,
        CameraPhi=205,
        CameraTheta=40,
        TrajectoryModel="drawByParticleID",
        TrajectoryType="smooth",
        TrajectoryFilters=[
            {"FilterType": "momentumMagnitudeFilter", "MinValue": 2 * units.MeV}
        ],
        StoreTrajectories="All",
        Debug=True,
    )


@events_1
@debug
@em_physics
@photon
@cube
def test_cube_dawnfile():
    ex = run_gaudi(
        # additional options
        f"{__file__}:config",
    )
    assert ex.returncode == 0
    expected_strings = [
        "Graphics system set to DAWNFILE (DAWNFILE)",
        "execute '/vis/filtering/trajectories/gaussinoIMagF/setAttribute IMag'",
        "execute '/vis/filtering/trajectories/gaussinoIMagF/addInterval 0.0 keV 2.0 MeV'",
        "execute '/vis/scene/add/hits '",
        ".prim  is generated.",
    ]
    for expected_string in expected_strings:
        assert expected_string in ex.stdout
