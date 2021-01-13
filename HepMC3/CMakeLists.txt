gaudi_subdir(HepMC3 v3r1)

find_package(ROOT)

# Define EvtGen external project
include(ExternalProject)
set(HEPMC3_GIT_SOURCE "https://gitlab.cern.ch/Gaussino/HepMC3.git" CACHE STRING "URL of the HepMC3 git repository")
set(HEPMC3_GIT_TAG "official_patched" CACHE STRING "Tag of HepMC3 to be used")
message(STATUS "HepMC3 will use tag \"${HEPMC3_GIT_TAG}\" from repository ${HEPMC3_GIT_SOURCE}")
message(STATUS "Binary dir = ${PROJECT_BINARY_DIR}")
message(STATUS "Build command = ${BUILD_COMMAND}")
message(STATUS "CMake command = ${CMAKE_COMMAND}")
string(REPLACE "c++" "" HEPMC_CXX_STANDARD "${GAUDI_CXX_STANDARD}")

ExternalProject_Add(HepMC3Ext
  GIT_REPOSITORY ${HEPMC3_GIT_SOURCE}
  GIT_TAG ${HEPMC3_GIT_TAG}
  CMAKE_ARGS 
       -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
       -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_LIST_DIR}/hepmc3_toolchain.cmake
       -DHEPMC3_ENABLE_PYTHON=OFF
       -DHEPMC3_INSTALL_INTERFACES=ON
       -DCMAKE_BUILD_TYPE=Debug
       -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
       #-DROOT_DIR=${ROOTSYS}
       -DCMAKE_INSTALL_LIBDIR=lib
       -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
       -DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}
       -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
       -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
       -DCMAKE_CXX_STANDARD=${HEPMC_CXX_STANDARD}
  CMAKE_COMMAND ${env_cmd} --xml ${env_xml} cmake
  BUILD_COMMAND ${env_cmd} --xml ${env_xml} cmake --build .
)
