################################################################################
# Package: HepMCUser
#
#################################################################################

gaudi_subdir(HepMCUser v1r0)

gaudi_depends_on_subdirs(Defaults
                         GaudiAlg
                         HepMC3)

gaudi_install_headers(HepMCUser)

find_package(ROOT)
AddHepMC3()

gaudi_add_library(HepMCUtils
                  src/*.cpp
                  PUBLIC_HEADERS HepMCUtils
                  INCLUDE_DIRS HepMCUser Defaults ${RANGEV3_INCLUDE_DIRS} ROOT
                  LINK_LIBRARIES ${HEPMC3_LIBRARIES} GaudiAlgLib ROOT)

# FIXME: Force the external build before the Utils here. Hack it time
add_dependencies(HepMCUtils HepMC3Ext)

gaudi_add_executable(compareHepMCEvents
                     exec/*.cpp
                     LINK_LIBRARIES HepMCUtils)
