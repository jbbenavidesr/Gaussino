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
import os

from Configurables import GaussinoGeometry, MCCollectorSensDet

# Import GDML
root = os.getenv("GIGAMTGEOROOT")
MCCollectorSensDet(
    "GiGaMT.DetConst.CubeToImportSDet",
    PrintStats=True,
)
# here embedding of the geometry takes place
GaussinoGeometry(
    SensDetMap={
        "MCCollectorSensDet/CubeToImportSDet": ["CubeToImportLVol"],
    },
    ImportGDML=[
        {
            "GDMLFileName": root + "/tests/data/cube_to_import.gdml",
        },
    ],
    # Export GDML
    ExportGDML={
        "GDMLFileName": "cube_to_export.gdml",
        "GDMLFileNameOverwrite": True,
        "GDMLExportEnergyCuts": True,
        "GDMLExportSD": True,
        "GDMLAddReferences": True,
    },
)
