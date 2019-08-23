################################################################################
# Package: GenInterface
################################################################################
gaudi_subdir(GenInterfaces v1r0)

gaudi_depends_on_subdirs(HepMC3
                         NewRnd)

AddHepMC3()

gaudi_install_headers(GenInterfaces)

gaudi_install_headers(
  Event)
