cmake_minimum_required(VERSION 3.18)
project(pingshaft)
set(CMAKE_CXX_STANDARD 17)
file(GLOB SOURCES "*.cpp" "*.hpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# For tags
# execute_process(
#   COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/update_tag.py
# )
# execute_process(
#   COMMAND git describe --abbrev=0 --tags
#   OUTPUT_VARIABLE tag
# )
# string(LENGTH ${tag} tag_len)
# math(EXPR new_tag_len "${tag_len} - 1")
# string(SUBSTRING "${tag}" 0 ${new_tag_len} clear_tag)
# string(REPLACE "." ";" list_tag "${clear_tag}")
# list(GET list_tag 0 MAJOR_VERSION)
# list(GET list_tag 1 MINOR_VERSION)
# list(GET list_tag 2 PATCH_VERSION)

# For packages
# set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
# set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
# set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
# set(CPACK_PACKAGE_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
# set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
# set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
# set(CPACK_PACKAGE_CONTACT elochkaan@gmail.com)
# set(CPACK_PACKAGE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# if (MSVC OR MSYS)
#   set(CPACK_GENERATOR ZIP)
# else ()
#   set(CPACK_GENERATOR DEB)
# endif()
# include(CPack)  
