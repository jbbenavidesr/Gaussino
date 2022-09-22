###############################################################################
# (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################


# copied & modified from LHCbConfigUtils.cmake
set(GAUSSINO_UNUSED_SUBDIR_MESSAGE_TYPE "WARNING"
    CACHE STRING "Message type for detected unused subdirs")
# copied & modified from LHCbConfigUtils.cmake
set(GAUSSINO_UNUSED_SOURCE_MESSAGE_TYPE "WARNING"
    CACHE STRING "Message type for detected unused source files")


function(gaussino_env)
    # copied & modified from lhcb_env in LHCbConfigUtils.cmake
    message(DEBUG "gaussino_env(${ARGV})")
    if(ARGC GREATER 0 AND ARGV0 STREQUAL "PRIVATE")
        # drop the flag and do not propagate the change downstream
        list(REMOVE_AT ARGV 0)
    else()
        # not PRIVATE, so propagate the change downstream
        set_property(GLOBAL APPEND PROPERTY ${PROJECT_NAME}_ENVIRONMENT ${ARGV})
    endif()
    if(NOT TARGET target_runtime_paths)
        # this usually means we are not using GaudiToolbox.cmake, like in old
        # style projects
        return()
    endif()
    while(ARGV)
        list(POP_FRONT ARGV action variable value)
        if(action STREQUAL "SET")
            set_property(TARGET target_runtime_paths APPEND_STRING
                PROPERTY extra_commands "export ${variable}=${value}\n")
        elseif(action STREQUAL "PREPEND")
            set_property(TARGET target_runtime_paths APPEND_STRING
                PROPERTY extra_commands "export ${variable}=${value}:\$${variable}\n")
        elseif(action STREQUAL "APPEND")
            set_property(TARGET target_runtime_paths APPEND_STRING
                PROPERTY extra_commands "export ${variable}=\$${variable}:${value}\n")
        elseif(action STREQUAL "DEFAULT")
            set_property(TARGET target_runtime_paths APPEND_STRING
                PROPERTY extra_commands "export ${variable}=\${${variable}:-${value}}\n")
        else()
            message(FATAL_ERROR "invalid environment action ${action}")
        endif()
    endwhile()
endfunction()

macro(gaussino_add_subdirectories)
    # copied & modified from lhcb_add_subdirectories in LHCbConfigUtils.cmake
    message(STATUS "Adding ${ARGC} subdirectories:")
    set(gaussino_add_subdirectories_index 0)
    foreach(subdir IN ITEMS ${ARGN})
        math(EXPR gaussino_add_subdirectories_index "${gaussino_add_subdirectories_index} + 1")
        math(EXPR gaussino_add_subdirectories_progress "${gaussino_add_subdirectories_index} * 100 / ${ARGC}")
        if(gaussino_add_subdirectories_progress LESS 10)
            set(gaussino_add_subdirectories_progress "  ${gaussino_add_subdirectories_progress}")
        elseif(gaussino_add_subdirectories_progress LESS 100)
            set(gaussino_add_subdirectories_progress " ${gaussino_add_subdirectories_progress}")
        endif()
        message(STATUS "    (${gaussino_add_subdirectories_progress}%) ${subdir}")
        add_subdirectory(${subdir})
        string(TOUPPER "${subdir}ROOT" root_var)
        string(REGEX REPLACE ".*/" "" root_var "${root_var}")
        gaussino_env(SET "${root_var}" "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
    endforeach()
endmacro()

function(gaussino_initialize_configuration)
    # copied & modified from lhcb_env in LHCbConfigUtils.cmake

    # Optionally enable compatibility with old-style CMake configurations, via helper module
    option(GAUDI_LEGACY_CMAKE_SUPPORT "Enable compatibility with old-style CMake builds" "$ENV{GAUDI_LEGACY_CMAKE_SUPPORT}")

    # default install prefix when building in legacy mode
    # note: this doplicates a bit of code in LegacyGaudiCMakeSupport.cmake, but we need to set CMAKE_INSTALL_PREFIX
    #       before we use it later in this macro (and before we can include LegacyGaudiCMakeSupport.cmake)
    if(GAUDI_LEGACY_CMAKE_SUPPORT)
        # make sure we have a BINARY_TAG CMake variable set
        if(NOT BINARY_TAG)
            if(NOT "$ENV{BINARY_TAG}" STREQUAL "")
                set(BINARY_TAG $ENV{BINARY_TAG})
            elseif(LHCB_PLATFORM)
                set(BINARY_TAG ${LHCB_PLATFORM})
            else()
                message(AUTHOR_WARNING "BINARY_TAG not set")
            endif()
        endif()

        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
            set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/InstallArea/${BINARY_TAG}"
                CACHE PATH "Install prefix" FORCE)
        endif()
    endif()

    # environment for the project
    gaussino_env(PREPEND PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
    gaussino_env(PREPEND LD_LIBRARY_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    if(NOT CMAKE_INSTALL_LIBDIR STREQUAL GAUDI_INSTALL_PLUGINDIR)
        gaussino_env(PREPEND LD_LIBRARY_PATH "${CMAKE_INSTALL_PREFIX}/${GAUDI_INSTALL_PLUGINDIR}")
    endif()
    gaussino_env(PREPEND PYTHONPATH "${CMAKE_INSTALL_PREFIX}/${GAUDI_INSTALL_PYTHONDIR}")
    gaussino_env(PREPEND ROOT_INCLUDE_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")

    # override GaudiToolbox internal macro to use gaussino_env
    macro(_gaudi_runtime_prepend runtime value)
        string(TOUPPER "${runtime}" _runtime_var)
        gaussino_env(PRIVATE PREPEND "${_runtime_var}" "${value}")
    endmacro()
endfunction()

function(gaussino_finalize_configuration)
    # copied & modified from gaussino_finalize_configuration in LHCbConfigUtils.cmake
    # environment for data packages
    get_property(data_packages_found GLOBAL PROPERTY DATA_PACKAGES_FOUND)
    foreach(data_package IN LISTS data_packages_found)
        if(data_package MATCHES "^([^:]*):(.*)\$")
            set(data_package ${CMAKE_MATCH_1})
        endif()
        string(TOUPPER "${data_package}" DP_NAME)
        string(REGEX REPLACE ".*/" "" DP_NAME "${DP_NAME}")
        gaussino_env(PRIVATE DEFAULT "${DP_NAME}ROOT" "${${data_package}_ROOT_DIR}")
        if(IS_DIRECTORY "${${data_package}_ROOT_DIR}/options")
            gaussino_env(PRIVATE SET "${DP_NAME}OPTS" "\${${DP_NAME}ROOT}/options")
            string(APPEND datapkg_env "export ${DP_NAME}OPTS=${${data_package}_ROOT_DIR}/options\n")
        endif()
        if(IS_DIRECTORY "${${data_package}_ROOT_DIR}/python")
            gaussino_env(PRIVATE PREPEND "PYTHONPATH" "\${${DP_NAME}ROOT}/python")
        endif()
    endforeach()

    # environment used by lbexec
    gaussino_env(PRIVATE DEFAULT "GAUDIAPPNAME" "${CMAKE_PROJECT_NAME}")
    gaussino_env(PRIVATE DEFAULT "GAUDIAPPVERSION" "${CMAKE_PROJECT_VERSION}")

    set_property(TARGET target_runtime_paths APPEND_STRING
        PROPERTY extra_commands "# remove duplicates\neval $(python3 -c \"import os; print(*('export {}={} ;'.format(n, ':'.join(dict.fromkeys(os.environ.get(n,'').split(':')))) for n in ('PATH','PYTHONPATH','LD_LIBRARY_PATH','ROOT_INCLUDE_PATH')),sep='\\n')\")\n")

    include(CMakePackageConfigHelpers)
    # special variables needed in the config file
    string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPCASE)
    get_property(packages_found GLOBAL PROPERTY PACKAGES_FOUND)
    get_property(ENVIRONMENT GLOBAL PROPERTY ${PROJECT_NAME}_ENVIRONMENT)
    string(REPLACE "${PROJECT_SOURCE_DIR}" "\${${PROJECT_NAME_UPCASE}_PROJECT_ROOT}"
        ENVIRONMENT "${ENVIRONMENT}")
    foreach(pack IN LISTS packages_found)
        string(TOUPPER "${pack}" PROJ)
        if(DEFINED ${PROJ}_PROJECT_ROOT)
            string(REPLACE "${${PROJ}_PROJECT_ROOT}" "\${${PROJ}_PROJECT_ROOT}"
                ENVIRONMENT "${ENVIRONMENT}")
        endif()
    endforeach()
    #  record "persistent options" for downstream projects
    set(CONFIG_OPTIONS "")
    foreach(option IN LISTS ${PROJECT_NAME}_PERSISTENT_OPTIONS)
        if(DEFINED ${option})
            string(APPEND CONFIG_OPTIONS "set(${option} ${${option}} CACHE BOOL \"value used at compile time for ${option}\" FORCE)\n")
        endif()
    endforeach()
    if(CONFIG_OPTIONS) # this is just to make the generated file nicer
        string(PREPEND CONFIG_OPTIONS "\n# Options used when compiling\n")
    endif()
    #  record versions of upstream LHCb projects
    set(DEPS_VERSIONS "")
    foreach(pack IN LISTS packages_found)
        # we want to record the versions of projects with /InstallArea/
        # as we usually have multiple versions available in the search path
        # and we want to pick up the same version that was used during the build
        if("${${pack}_DIR}${${pack}_ROOT_DIR}" MATCHES "/InstallArea/" AND "${${pack}_VERSION}" MATCHES "^[0-9.]+$")
            string(APPEND DEPS_VERSIONS
                "if(NOT DEFINED ${pack}_EXACT_VERSION)\n"
                "    set(${pack}_EXACT_VERSION ${${pack}_VERSION} CACHE STRING \"Version of ${pack} used in upstream builds\")\n"
                "    mark_as_advanced(${pack}_EXACT_VERSION)\n"
                "elseif(NOT ${pack}_EXACT_VERSION STREQUAL \"${${pack}_VERSION}\")\n"
                "    message(WARNING \"Requested version of ${pack} (\${${pack}_EXACT_VERSION}) differs from that used for build (${${pack}_VERSION})\")\n"
                "endif()\n")
        endif()
    endforeach()
    if(DEPS_VERSIONS)
        string(APPEND CONFIG_OPTIONS "\n# Versions of upstream projects used for the build\n${DEPS_VERSIONS}")
    endif()

    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT "${ARGV0}" STREQUAL "NO_EXPORT")
        # install exports only if we are a master project we were not given the "NO_EXPORT" option
        install(EXPORT ${PROJECT_NAME} NAMESPACE ${PROJECT_NAME}::
            FILE "${PROJECT_NAME}Targets.cmake"
            DESTINATION "${GAUDI_INSTALL_CONFIGDIR}")
        set(CONFIG_IMPORT_TARGETS "include(\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake)\n")
    endif()

    # generate config files
    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/GaussinoConfig.cmake.in ${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION "${GAUDI_INSTALL_CONFIGDIR}"
        PATH_VARS
            CMAKE_INSTALL_BINDIR
            CMAKE_INSTALL_LIBDIR
            CMAKE_INSTALL_INCLUDEDIR
            GAUDI_INSTALL_PLUGINDIR
            GAUDI_INSTALL_PYTHONDIR
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
    write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
        COMPATIBILITY AnyNewerVersion)

    gaudi_generate_version_header_file()

    gaudi_install(CMAKE
        cmake/${PROJECT_NAME}Dependencies.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    )
    if(EXISTS lhcbproject.yml)
        gaudi_install(CMAKE lhcbproject.yml)
    endif()

    # check that we actually build everything in the project
    # - check all subdirs are included (except for those listed in GAUSSINO_IGNORE_SUBDIRS)
    get_property(added_subdirs DIRECTORY PROPERTY SUBDIRECTORIES)
    file(GLOB_RECURSE detected_subdirs "*/CMakeLists.txt")
    if(detected_subdirs)
        set(missed_subdirs)
        list(TRANSFORM detected_subdirs REPLACE "/CMakeLists.txt" "")
        list(SORT detected_subdirs)
        foreach(subdir IN LISTS detected_subdirs)
            list(FIND added_subdirs ${subdir} idx)
            if(idx STREQUAL "-1" AND NOT subdir MATCHES "^${CMAKE_BINARY_DIR}/.*")
                file(GLOB subdir RELATIVE "${PROJECT_SOURCE_DIR}" ${subdir})
                list(APPEND missed_subdirs ${subdir})
            endif()
        endforeach()
        if(missed_subdirs AND GAUSSINO_IGNORE_SUBDIRS)
            list(REMOVE_ITEM missed_subdirs ${GAUSSINO_IGNORE_SUBDIRS})
        endif()
        if(missed_subdirs)
            list(JOIN missed_subdirs "\n - " missed_subdirs)
            message(${GAUSSINO_UNUSED_SUBDIR_MESSAGE_TYPE}
                "Project ${PROJECT_NAME} contains subdirectories that are not used:\n - ${missed_subdirs}\n")
        endif()
    endif()
    # - check for all files (*.cpp and *.cxx in source directories)
    if(NOT GAUSSINO_UNUSED_SOURCE_MESSAGE_TYPE STREQUAL "IGNORE")
        # for each directory (including the top one) we get the targets and for
        # each target we get the source files excluding absolute paths
        # (which means also those in the build directory), then we prefix the subdir
        # path
        set(used_sources)
        list(PREPEND added_subdirs ${PROJECT_SOURCE_DIR})
        foreach(subdir IN LISTS added_subdirs)
            get_property(targets DIRECTORY ${subdir} PROPERTY BUILDSYSTEM_TARGETS)
            foreach(target IN LISTS targets)
                get_target_property(target_type ${target} TYPE)
                if(target_type STREQUAL "INTERFACE_LIBRARY" AND CMAKE_VERSION VERSION_LESS "3.19")
                    continue()  # sources for interface libraries were introduced only in cmake 3.19
                endif()
                get_target_property(sources ${target} SOURCES)
                list(FILTER sources EXCLUDE REGEX "^/")
                list(TRANSFORM sources PREPEND "${subdir}/")
                list(APPEND used_sources ${sources})
            endforeach()
        endforeach()
        list(REMOVE_DUPLICATES used_sources)
        # from the list of of source files we guess the directories meant to contain
        # source files
        list(TRANSFORM used_sources REPLACE "/[^/]*$" ""
            OUTPUT_VARIABLE source_dirs)
        list(REMOVE_DUPLICATES source_dirs)
        # and we look for all *.cpp and *.cxx files in these directories
        list(TRANSFORM source_dirs APPEND "/*.cpp" OUTPUT_VARIABLE source_cpp_globs)
        list(TRANSFORM source_dirs APPEND "/*.cxx" OUTPUT_VARIABLE source_cxx_globs)
        file(GLOB all_sources ${source_cpp_globs} ${source_cxx_globs})
        # check if they are all used
        set(missed_sources)
        foreach(src IN LISTS all_sources)
            list(FIND used_sources ${src} idx)
            if(idx STREQUAL "-1")
                file(GLOB src RELATIVE "${PROJECT_SOURCE_DIR}" ${src})
                list(APPEND missed_sources ${src})
            endif()
        endforeach()
        # and report any missing file
        if(missed_sources)
            list(JOIN missed_sources "\n - " missed_sources)
            message(${GAUSSINO_UNUSED_SOURCE_MESSAGE_TYPE}
                "Project ${PROJECT_NAME} contains source files which are not used in any target:\n - ${missed_sources}\n")
        endif()
    endif()


    # Set the version of the project as a cache variable to be seen by other
    # projects in the same super-project.
    set(${PROJECT_NAME}_VERSION "${PROJECT_VERSION}" CACHE STRING "Version of ${PROJECT_NAME}" FORCE)

    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND GAUDI_LEGACY_CMAKE_SUPPORT)
        find_file(legacy_cmake_config_support NAMES LegacyGaudiCMakeSupport.cmake)
        if(legacy_cmake_config_support)
            include(${legacy_cmake_config_support})
        else()
            message(FATAL_ERROR "GAUDI_LEGACY_CMAKE_SUPPORT set to TRUE, but cannot find LegacyGaudiCMakeSupport.cmake")
        endif()
    endif()
endfunction()
