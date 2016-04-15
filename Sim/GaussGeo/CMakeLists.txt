################################################################################
# Package: GaussGeo
################################################################################
gaudi_subdir(GaussGeo v1r0)

gaudi_depends_on_subdirs(Det/DetDesc
                         Geant4/G4readout
                         Sim/GiGa)

gaudi_add_module(GaussGeo
                 src/component/*.cpp
                 INCLUDE_DIRS Tools/ClhepTools
                 LINK_LIBRARIES DetDescLib G4readoutLib GiGaLib)
