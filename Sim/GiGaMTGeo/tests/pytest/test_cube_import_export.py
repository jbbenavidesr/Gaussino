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
from Gaussino.pytest.helpers import *
from Gaussino.pytest.options import *

@reset_configurables
@events_1
@em_physics
@photon
@empty_world
@debug
def test_cube_import_export():
    ex = run_gaudi(
        # additional options
        "$GIGAMTGEOROOT/tests/options/cube_import_export.py",
    )
    assert ex.returncode == 0
    try:
        with open('cube_to_export.gdml') as gdml:
            data = gdml.read()
            if "CubeToImportLVol" not in data:
                raise AssertionError('Imported GDML volumes were not correctly exported.')
            if "CubeToImportSDet" not in data:
                raise AssertionError('Sensitive information not exported correctly.')
    except FileNotFoundError as e:
        raise FileNotFoundError('GDML not exported!') from e

    # check if sensitive detector is attached correctly to the imported volume
    hits_reg = "Hits=    1 Energy=       0[GeV] #Particles=    1 in CubeToImportSDet"
    if not hits_reg in ex.stdout:
        raise AssertionError('Imported sensitive detector class not activated!')
