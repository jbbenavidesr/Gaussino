################################################################################
# Package: FCCTest
# Adapted from Gaussino FCC TestGeometry package: https://gitlab.cern.ch/jlingema/Gauss/tree/refactor/
# Original author: Anna Zaborowska
#
################################################################################
gaudi_subdir(FCCTest v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                         GaudiAlg
                         Sim/GiGaMTFactories
                         Sim/GiGaMTCore
                         Sim/SimInterfaces
                         Sim/GiGaMTGeo)

add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

find_package(XercesC)
#find_package(ROOT)

#find_package(ROOT COMPONENTS MathCore GenVector Geom RIO Hist REQUIRED)

#gaudi_install_headers(TestGeometry)

gaudi_add_library(FCCTest
          src/lib/*.cpp
          INCLUDE_DIRS Geant4 ROOT XercesC SimInterfaces AIDA
          LINK_LIBRARIES GaudiKernel GaudiAlgLib Geant4 ROOT XercesC GiGaMTCoreRunLib
          PUBLIC_HEADERS FCCTest)

gaudi_add_module(FCCTestPlugins
          src/components/*.cpp
          INCLUDE_DIRS Geant4 FCCTest GaudiKernel
          LINK_LIBRARIES GaudiKernel Geant4 FCCTest ROOT)

include(CTest)
gaudi_add_test(GeantGdmlWithSD
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_fullsim_gdml.py)
