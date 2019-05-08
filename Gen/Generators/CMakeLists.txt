################################################################################
# Package: Generators
################################################################################
gaudi_subdir(Generators v16r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         GaudiAlg
                         NewRnd
                         #FIXME: Gen/EvtGen
                         #FIXME: Gen/EvtGenExtras
                         #FIXME: Kernel/MCInterfaces
                         #FIXME: Kernel/LHCbKernel
                         Kernel/PartProp
                         Defaults
                         HepMC3
                         HepMCUser
                         Gen/GenInterfaces)

find_package(Boost COMPONENTS filesystem system)
find_package(Photos++)
AddHepMC3()

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")

gaudi_add_library(GeneratorsLib
                  src/Lib/*.cpp src/Lib/*.F
                  PUBLIC_HEADERS Generators
                  INCLUDE_DIRS Kernel/MCInterfaces Gen/GenInterfaces HepMCUser ${HEPMC3_INCLUDE_DIR}
                  LINK_LIBRARIES GenEvent GaudiAlgLib PartPropLib ${HEPMC3_LIBRARIES} NewRnd)

add_dependencies(GeneratorsLib HepMC3Ext)

gaudi_add_module(Generators
                 src/component/*.cpp
                 INCLUDE_DIRS Boost Kernel/MCInterfaces  Gen/GenInterfaces Photos++
                 LINK_LIBRARIES Boost GenEvent GaudiAlgLib LHCbKernel GeneratorsLib Photos++) 

