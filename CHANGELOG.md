# Changelog
All notable changes to this project will be documented in this file.

Project Coordinators: Gloria Corti @gcorti, Michal Mazurek @mimazure

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).

## [v1r0](https://gitlab.cern.ch/gaussino/Gaussino/-/releases/v1r0) - 2024-02-12
This is the first release of the Gaussino experiment-independent core software simulation framework.
Gaussino is based on the [[Gaudi](https://cern.ch/gaudi) event data processing framework. It provide interfaces to generator and
an adaptor for [Pythia 8](https://pythia.org/) and uses the [Geant4](https://geant4.cern.ch) toolkit for detector simulation.
For more information refer to https://gaussino.docs.cern.ch/

This release is customised for the LHCb experiment.

The version of the libraries used are fixed in the
[GaussinoExtLib]((https://gitlab.cern.ch/gaussino/GaussinoExtLib) project.
It should be noted that at the moment Gaussino still depends on a few LHCb packages, hence it needs to be built
againt the core LHCb project. Work is in progress to remove this dependecy.

This release is based on
Gaudi [v37r2](https://gitlab.cern.ch/gaudi/Gaudi/-/releases/v37r2)
and LCG [104_LHCB_7](https://lcginfo.cern.ch/release_packages/104_LHCB_7/) with ROOT 6.28.04 and Pythia8 244.lhcb4

It uses GaussinoExtLib [v1r0](https://gitlab.cern.ch/Gaussino/gaussinoextlibs/-/releases/v1r0):
- HepMC [3.2.2](https://gitlab.cern.ch/hepmc/HepMC3/-/releases/3.2.2)
with [patches](https://gitlab.cern.ch/Gaussino/HepMC3/-/tree/official_patched?ref_type=heads)
- DD4hep [v01-26](https://github.com/AIDASoft/DD4hep/releases/tag/v01-26)
with [patches](https://gitlab.cern.ch/Gaussino/DD4hep/-/commits/v01-26-patches/?ref_type=heads)
- Geant4 [10.7.3 with patches](https://gitlab.cern.ch/lhcb/Geant4/-/releases/v10r7p3t1)



### Added
- Fix total cross section for given energy. (Gaussino/Gaussino!139)
- Make physics processes in parallel worlds optional (Gaussino/Gaussino!149)
- Distance to ZMaxPlane along a particular direction (Gaussino/Gaussino!148)
- GenRndInit cleanup & more precise timing info (Gaussino/Gaussino!115)
- Fix compilation with C++20 (Gaussino/Gaussino!158)
- [FASTSIM] Fast Simulation Interface (Gaussino/Gaussino!20)
- Update the particle table used by Gaussino to the latest version of DDDB (Gaussino/Gaussino!59)
- Fix docs typos (Gaussino/Gaussino!156)
- ML Model Serving (ONNXRuntime) (Gaussino/Gaussino!145)
- ML Model Serving (pyTorch) (Gaussino/Gaussino!55)
- CaloChallenge setup for planar calorimeters (Gaussino/Gaussino!146)
- Adapt Par04 to support ML models with pyTorch and ONNXRuntime (Gaussino/Gaussino!131)
- Fix standalone build of public headers (Gaussino/Gaussino!152)
- Apply formatting (Gaussino/Gaussino!144)
- Move GiGaMTExamples to Examples & drop unused examples (Gaussino/Gaussino!140)
- Adapt to GaudiAlg being optional (Gaussino/Gaussino!142)
- Fix gcc12 warnings (Gaussino/Gaussino!143)
- Remove UNLIKELY from Gaussino (Gaussino/Gaussino!141)
- Fix for gcc12 (Gaussino/Gaussino!137)
- Cylindrical calorimeter in Gaussino (following G4/Par04) (Gaussino/Gaussino!122)
- Use the new way of declaring pytests from Gaudi (Gaussino/Gaussino!132)
- Add total memory counters to GenRndInit (Gaussino/Gaussino!130)
- Do not print warnings if regions in CustomSimumlation are looked up again (Gaussino/Gaussino!123)
- Allow extra volumes in the external sensitive detector (Gaussino/Gaussino!127)
- Introduce rhoMax optimization for cylindrical detectors (Gaussino/Gaussino!126)
- Specify mother volumes of the ExternalDetector (optional) (Gaussino/Gaussino!121)
- Allow empty sens det tools in the ExternalDetector package (Gaussino/Gaussino!120)
- Add materials from G4/NIST in the ExternalDetector package (Gaussino/Gaussino!119)
- Add new shape Tube to the ExternalDetector package (Gaussino/Gaussino!118)
- Add MessageSvcSink to the list of services (Gaussino/Gaussino!114)
- Add a magnetic field option in ExternalDetector (Gaussino/Gaussino!84)
- Allow no phases for testing purposes & clean up configurables (Gaussino/Gaussino!116)
- Update docs to use LCG 103 (Gaussino/Gaussino!113)
- Give the possibility to choose the root volume of the exported GDML file (Gaussino/Gaussino!112)
- Custom Simulations with Geant4 10.7 (Gaussino/Gaussino!61)
- Use FindDD4hepDDG4.cmake (Gaussino/Gaussino!108)
- Do not add additional EventDataSvc (Gaussino/Gaussino!109)
- Generalize MagField from MagFieldFromSvc (Gaussino/Gaussino!81)
- Fix string replace in environment in *Config.cmake for projects with the same prefix (Gaussino/Gaussino!96)
- Merge Phoenix and Geant4 options and add more Phoenix options (Gaussino/Gaussino!89)
- Add custom Pseudorapidity Trajectory Filter (Gaussino/Gaussino!86)
- Enable more visualization drivers and provide extensive documentation (Gaussino/Gaussino!80)
- Produce LHCb::ODIN with GenRndInit (Gaussino/Gaussino!105)
- Do not force Gaussino from depending configurables (Gaussino/Gaussino!104)
- Allow only Gaussino to set up GenRndInit/GenReDecayInit (Gaussino/Gaussino!101)
- Update the docs (DD4hep in the main platform) (Gaussino/Gaussino!107)
- Port generic infrastructure of CaloCollector to Gaussino (Gaussino/Gaussino!99)
- Update Gaussino docs in preparation for v0r1 (Gaussino/Gaussino!103)
- Extend pages pipeline to remove old versions of the documentation (Gaussino/Gaussino!102)
- Internal compatibility of python configuration (Gaussino) (Gaussino/Gaussino!97)
- Update docs to LCG 101a (Gaussino/Gaussino!100)
- [FASTSIM] Adapt MCCollector to the changes in Sim10 and Fast Simulation Interface (Gaussino/Gaussino!52)
- Update to dd4hep v01-23 (Gaussino/Gaussino!98)
- Workaround for undefined symbol (Gaussino/Gaussino!53)
- Setting up options propagation for production tool - for now only Pythia8Production is supported (Gaussino/Gaussino!88)
- Fix crash for inclusive B events due to null pointer (Gaussino/Gaussino!85)
- Updates the docs to use lhcb-dd4hep for DD4hep developments (Gaussino/Gaussino!95)
- Python Conf Part 1.1a: Adapt options for G-on-G tests (Gaussino/Gaussino!68)
- Do not repeat RunActions for worker threads (Gaussino/Gaussino!73)
- Update LCG version to 101x in the docs (Gaussino/Gaussino!94)
- Fix production cuts not set in the modular physics list (Gaussino/Gaussino!92)
- Document getting list of MRs from GitLab API (Gaussino/Gaussino!93)
- New CMake & moving HepMC3 & DDG4 to ExtLibs (Gaussino/Gaussino!41)
- GiGaMTPhoenix - more trajectory properties (Gaussino/Gaussino!91)
- Add Phoenix trajectory models (Gaussino/Gaussino!90)
- Add GiGaMTPhoenix (Gaussino/Gaussino!78)
- Introduce UI InitCommands (Gaussino/Gaussino!76)
- Correctly pass the information from run managers to Geant4 (Gaussino/Gaussino!74)
- Add GiGaEventActionCommand and refactor GiGaRunActionCommand + UI session (Gaussino/Gaussino!69)
- Add GiGaUIMessage and refactor GiGaMessage (Gaussino/Gaussino!75)
- Do not ignore thread UI commands (Gaussino/Gaussino!72)
- Example of a custom TrajectoryFilter in Gaussino (Gaussino/Gaussino!82)
- Delete G4Events ONLY when postprocessing is done (Gaussino/Gaussino!77)
- Remove non-ASCII characters in concurrentqueue.h (Gaussino/Gaussino!57)
- Update logo (Gaussino/Gaussino!67)
- Port GDMLReader from Gauss (Gaussino/Gaussino!66)
- Remove /gaussino as the docs root (Gaussino/Gaussino!65)
- More detailed docs (Gaussino/Gaussino!62)
- Add logo to main documentation page (Gaussino/Gaussino!64)
- Move to gitlab pages (Gaussino/Gaussino!49)
- [FASTSIM] Fast Simulation Tutorial: Shower Model (Gaussino/Gaussino!44)
- Remove ->Geant4/<- prefix in all the headers coming from Geant4 with new cmake (Gaussino/Gaussino!54)
- Update the workspace in the docs (Gaussino/Gaussino!58)
- Fixing DDG4 build & use DD4HEP in a dedicated platform (Gaussino/Gaussino!43)
- Fix CMAKE_PREFIX_PATH to use for a local copy of the nightlies (Gaussino/Gaussino!51)
- Add G4OpticalPhysics list (Gaussino/Gaussino!47)
- Update platform in the docs (Gaussino/Gaussino!50)
- Update the platforms and the recommended installation (Gaussino/Gaussino!48)
- Update documentation to the new configuration of platforms (Gaussino/Gaussino!46)
- Move to LCG layer LHCB_7 (Gaussino/Gaussino!45)
- Support both 3.56 (LHCB_layer) and newer versions of Photos++ (Gaussino/Gaussino!33)
- Following changes in the modernization of GenCountersFSR and GenFSR (Gaussino/Gaussino!39)
- Remove std::array from the Gaudi::Functional constructors (Gaussino/Gaussino!40)
- Port detector related libraries from Gauss + documentation (Gaussino/Gaussino!37)
- ZMaxPlane (Gaussino/Gaussino!42)
- Skeleton of the documentation (Gaussino/Gaussino!26)
- Formatting in Sim (Gaussino/Gaussino!38)
- LHCb-based commit (Gaussino/Gaussino!9)
- Updating how LCG layers are introduced (Gaussino/Gaussino!21)
- Add copyright in Gaussino (Gaussino/Gaussino!30)
