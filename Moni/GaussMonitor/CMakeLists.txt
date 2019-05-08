################################################################################
# Package: GaussMonitor
################################################################################
gaudi_subdir(GaussMonitor v8r2)

gaudi_depends_on_subdirs(Event/GenEvent
                         GaudiAlg
                         Defaults
                         HepMC3
                         Kernel/PartProp)

AddHepMC3()

gaudi_add_module(GaussMonitor
                 src/*.cpp
                 INCLUDE_DIRS AIDA ${HEPMC3_INCLUDE_DIR}
                 LINK_LIBRARIES GaudiAlgLib GenEvent PartPropLib ${HEPMC3_LIBRARIES})

add_dependencies(GaussMonitor HepMC3Ext)
