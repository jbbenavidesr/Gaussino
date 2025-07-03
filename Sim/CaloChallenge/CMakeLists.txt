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
#[=======================================================================[.rst:
Examples/CaloChallenge
----------------------
#]=======================================================================]

gaudi_add_module(CaloChallenge
    SOURCES
        src/CaloG4Hit.cpp
        src/CaloSensitiveDetector.cpp
        src/CaloHitsSplitter.cpp
        src/CollectorG4Hit.cpp
        src/CollectorSensitiveDetector.cpp
        src/GetCaloHitsAlg.cpp
        src/GetCollectorHitsAlg.cpp
        src/GetMLCaloHitsAlg.cpp
	      src/ImmediateDepositWithConditionsModel.cpp
        src/Monitoring.cpp
        src/TrainingDataCollector.cpp
    LINK
        Gaussino::CustomSimulationLib
        Gaussino::EDMLib
        Gaussino::MCCollectorLib
        Gaussino::GiGaMTCoreRunLib
)

if(GSINO_USE_TORCH OR GSINO_USE_ONNXRUNTIME)
  target_link_libraries(CaloChallenge PRIVATE Gaussino::GaussinoMLLib)
  target_sources(CaloChallenge PRIVATE
    src/VAEFromGeant4Model.cpp
    src/VAEWithProfilesModel.cpp
  )
endif()

if(GSINO_USE_TORCH)
  target_link_libraries(CaloChallenge PRIVATE Gaussino::GaussinoTorchLib)
  set(CaloChallenge_Torch_TESTS
    tests/pytest/test_cylindrical_torch_VAE_from_geant4.py
    tests/pytest/test_cylindrical_torch_VAE_from_geant4_mt.py
  )
endif()

if(GSINO_USE_ONNXRUNTIME)
  target_link_libraries(CaloChallenge PRIVATE Gaussino::GaussinoONNXLib)
  set(CaloChallenge_ONNX_TESTS
    tests/pytest/test_cylindrical_onnx_VAE_from_geant4.py
    tests/pytest/test_cylindrical_onnx_VAE_from_geant4_mt.py
  )
endif()

gaudi_install(PYTHON)
gaudi_add_pytest(
  tests/pytest/test_cylindrical_g4.py
  tests/pytest/test_cylindrical_g4_mt.py
  tests/pytest/test_planar_g4.py
  tests/pytest/test_planar_g4_mt.py
  ${CaloChallenge_Torch_TESTS}
  ${CaloChallenge_ONNX_TESTS}
)
