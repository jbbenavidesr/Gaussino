################################################################################
# Package: LbAmpGen
################################################################################
gaudi_subdir(LbAmpGen v1r0)

gaudi_add_module(LbDtoKpipipi_v1
                 src/DtoKpipipi_v1.cpp)

add_custom_target(ampgen_source_files 
                  COMMAND ${env_cmd} --xml ${env_xml}
                      ${CMAKE_CURRENT_SOURCE_DIR}/utils/makeModels.py
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmt)
