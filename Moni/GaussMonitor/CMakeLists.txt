################################################################################
# Package: GaussMonitor
################################################################################
gaudi_subdir(GaussMonitor v8r2)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/MCEvent
                         GaudiAlg
                         Defaults
                         HepMC3
                         HepMCUser
                         Kernel/PartProp)

AddHepMC3()

gaudi_add_module(GaussMonitor
                 src/*.cpp
                 INCLUDE_DIRS AIDA ${HEPMC3_INCLUDE_DIR} HepMCUser
                 LINK_LIBRARIES GaudiAlgLib GenEvent PartPropLib ${HEPMC3_LIBRARIES} MCEvent)

add_dependencies(GaussMonitor HepMC3Ext)
