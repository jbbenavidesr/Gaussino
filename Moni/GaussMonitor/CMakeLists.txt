################################################################################
# Package: GaussMonitor
################################################################################
gaudi_subdir(GaussMonitor v8r2)

gaudi_depends_on_subdirs(Event/GenEvent
                         GaudiAlg
                         Kernel/PartProp)

find_package(HepMC)

gaudi_add_module(GaussMonitor
                 src/*.cpp
                 INCLUDE_DIRS AIDA HepMC
                 LINK_LIBRARIES GaudiAlgLib GenEvent PartPropLib HepMC)
