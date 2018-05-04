################################################################################
# Package: HepMCUser
#
#################################################################################

gaudi_subdir(HepMCUser v1r0)

gaudi_depends_on_subdirs(Defaults
                         GaudiAlg)

gaudi_install_headers(HepMCUser)

find_package(HepMC CONFIG)

gaudi_add_library(HepMCUtils
                  src/*.cpp
                  PUBLIC_HEADERS HepMCUtils
                  INCLUDE_DIRS HepMC HepMCUser Defaults ${RANGEV3_INCLUDE_DIRS}
                  LINK_LIBRARIES HepMC GaudiAlgLib)

gaudi_add_executable(compareHepMCEvents
                     exec/*.cpp
                     LINK_LIBRARIES HepMCUtils)
