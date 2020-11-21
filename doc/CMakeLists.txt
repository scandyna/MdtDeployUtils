# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#######################
# CMake API doc
#######################

configure_file(sphinx/conf.py.in cmake-api/conf.py @ONLY)

set(doc_format "html")
set(doc_format_output "doc_${doc_format}")
set(doc_format_log "doc_${doc_format}.log")
add_custom_command(
    OUTPUT ${doc_format_output}
    COMMAND
        Sphinx::Build
        -c ${CMAKE_CURRENT_BINARY_DIR}/cmake-api
        -d ${CMAKE_CURRENT_BINARY_DIR}/cmake-api/doctrees
        -b ${doc_format}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/cmake-api/${doc_format}
        > ${doc_format_log} # log stdout, pass stderr
    ${${doc_format}_extra_commands}
    COMMENT "sphinx-build ${doc_format}: see ${CMAKE_CURRENT_BINARY_DIR}/${doc_format_log}"
    VERBATIM
)
set_property(SOURCE ${doc_format_output} PROPERTY SYMBOLIC 1)

add_custom_target(CMakeApiDoc ALL DEPENDS ${doc_format_output})


#######################
# C++ API doc
#######################

configure_file(
  "${PROJECT_SOURCE_DIR}/doc/Doxyfile.in"
  "${PROJECT_BINARY_DIR}/doc/Doxyfile"
)

add_custom_command(
    OUTPUT cpp-api-doc
    COMMAND
      Doxygen::doxygen "${PROJECT_BINARY_DIR}/doc/Doxyfile"
    COMMENT "Build C++ API doc"
    VERBATIM
)
set_property(SOURCE cpp-api-doc PROPERTY SYMBOLIC 1)
add_custom_target(CppApiDoc ALL DEPENDS cpp-api-doc)
