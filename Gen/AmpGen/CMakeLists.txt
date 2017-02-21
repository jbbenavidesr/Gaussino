################################################################################
# Package: AmpGen
################################################################################
gaudi_subdir(AmpGen v1r0)

find_package(ROOT COMPONENTS RIO Hist Matrix Graf Minuit Tree MathCore Physics)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -Wno-unused-function -Wno-unused-parameter -ldl")

gaudi_add_library(AmpGen
                  src/*.cpp
                  NO_PUBLIC_HEADERS
		  INCLUDE_DIRS ROOT
		  LINK_LIBRARIES ROOT)

foreach(app Generator Fitter ConvertToSourceCode)
  gaudi_add_executable(${app} apps/${app}.cpp
                       LINK_LIBRARIES AmpGen)
endforeach()

      
