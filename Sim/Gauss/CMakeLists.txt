################################################################################
# Package: Gauss
################################################################################
gaudi_subdir(Gauss v52r0)

find_package(HepMC)

gaudi_depends_on_subdirs(Det/DDDB
                         Det/DetSys
                         Event/EventPacker
                         Gaudi
                         GaudiConf
                         GaudiKernel
                         Kernel/KernelSys
                         Sim/SimComponents
                         Sim/GaussAlgs
                         Gen/LbPGuns
                         Gen/LbMIB
                         Gen/LbPythia
                         Gen/LbPythia8
                         Gen/LbHijing
                         Gen/LbCRMC
                         Sim/GaussAlgs
                         Sim/GaussKine
                         Sim/GaussRICH
                         Sim/GaussRedecay
                         Sim/GaussCherenkov
                         Sim/GaussCalo
                         Sim/GaussTracker
                         Muon/MuonMoniSim)

gaudi_install_python_modules()
gaudi_install_scripts()

gaudi_env(SET AlwaysKillLeadingHadron 1
          SET GAUSSOPTS \${GAUSSROOT}/options)

gaudi_add_test(QMTest QMTEST)
