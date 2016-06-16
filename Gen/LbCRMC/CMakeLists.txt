################################################################################
# Package: LbCRMC
################################################################################
gaudi_subdir(LbCRMC v1r2)

gaudi_depends_on_subdirs(Gen/Generators)

find_package(CRMC)
#find_package(ROOT COMPONENTS Matrix)

gaudi_add_library(LbCRMCLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS LbCRMC
                  INCLUDE_DIRS CRMC 
                  LINK_LIBRARIES GeneratorsLib CRMC)
#                  INCLUDE_DIRS CRMC ROOT
#                  LINK_LIBRARIES GeneratorsLib CRMC ROOT)

gaudi_add_module(LbCRMC
                 src/component/*.cpp 
                 LINK_LIBRARIES LbCRMCLib)

