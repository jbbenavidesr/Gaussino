################################################################################
# Package: LbPythia8
################################################################################
gaudi_subdir(LbPythia8 v12r0)

gaudi_depends_on_subdirs(Gen/GenInterfaces
                         Event/GenEvent
                         Kernel/PartProp
                         GaudiAlg
                         NewRnd
                         HepMC3
                         HepMCUser
                         Utils
                         Defaults)

find_package(Pythia8 COMPONENTS pythia8 pythia8tohepmc)
AddHepMC3()
find_package(LHAPDF)
find_package(Boost COMPONENTS filesystem system)
find_package(ROOT)
find_package(CLHEP)

if(EXISTS "${PYTHIA8_INCLUDE_DIR}/Pythia8/Pythia.h")
  # include directory changed from Pythia8 175 to 183
  include_directories(${PYTHIA8_INCLUDE_DIR}/Pythia8)
endif()

gaudi_add_library(Pythia8ToHepMC3Lib
                  src/interface/*.cpp
                  PUBLIC_HEADERS Pythia8HepMC
                  INCLUDE_DIRS Pythia8 ${HEPMC3_INCLUDE_DIR}
                  LINK_LIBRARIES Pythia8 ${HEPMC3_LIBRARIES})

add_dependencies(Pythia8ToHepMC3Lib HepMC3Ext)

gaudi_add_library(LbPythia8Lib
                  src/Lib/*.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS Boost Pythia8 ROOT ${HEPMC3_INCLUDE_DIR} Gen/GenInterfaces Defaults HepMCUser Utils NewRnd
                  LINK_LIBRARIES Boost GaudiAlgLib Pythia8 ${HEPMC3_LIBRARIES} ROOT PartPropLib LHAPDF Pythia8ToHepMC3Lib)

add_dependencies(LbPythia8Lib HepMC3Ext)

gaudi_add_module(LbPythia8
                 src/component/*.cpp
                 LINK_LIBRARIES LbPythia8Lib)

gaudi_env(SET PYTHIA8XML ${PYTHIA8_XML})

gaudi_add_executable(Pythia8Reproducibility
                     exec/minimal.cxx
                     INCLUDE_DIRS Pythia8 ${HEPMC3_INCLUDE_DIR} Pythia8ToHepMC3Lib CLHEP HepMCUtils
                     LINK_LIBRARIES Pythia8 ${HEPMC3_LIBRARIES} Pythia8ToHepMC3Lib CLHEP HepMCUtils)
