################################################################################
# Package: LbPythia8
################################################################################
gaudi_subdir(LbPythia8 v12r0)

gaudi_depends_on_subdirs(Gen/GenInterfaces
                         Event/GenEvent
                         Kernel/PartProp
                         GaudiAlg
                         HepMCUser
                         Utils
                         Defaults)

find_package(Pythia8 COMPONENTS pythia8 pythia8tohepmc)
find_package(HepMC CONFIG)
find_package(LHAPDF)
find_package(Boost COMPONENTS filesystem system)
find_package(ROOT COMPONENTS GenVector)

if(EXISTS "${PYTHIA8_INCLUDE_DIR}/Pythia8/Pythia.h")
  # include directory changed from Pythia8 175 to 183
  include_directories(${PYTHIA8_INCLUDE_DIR}/Pythia8)
endif()

gaudi_add_library(Pythia8ToHepMC3Lib
                  src/interface/*.cpp
                  PUBLIC_HEADERS Pythia8HepMC
                  INCLUDE_DIRS Pythia8 HepMC
                  LINK_LIBRARIES Pythia8 HepMC)

gaudi_add_library(LbPythia8Lib
                  src/Lib/*.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS Boost Pythia8 ROOT HepMC Gen/GenInterfaces Defaults HepMCUser Utils
                  LINK_LIBRARIES Boost GaudiAlgLib Pythia8 HepMC ROOT PartPropLib LHAPDF Pythia8ToHepMC3Lib)

gaudi_add_module(LbPythia8
                 src/component/*.cpp
                 LINK_LIBRARIES LbPythia8Lib HepMC)

gaudi_env(SET PYTHIA8XML ${PYTHIA8_XML})

