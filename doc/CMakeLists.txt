# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

configure_file(sphinx/conf.py.in conf.py @ONLY)

set(doc_format "html")
set(doc_format_output "doc_${doc_format}")
set(doc_format_log "doc_${doc_format}.log")
add_custom_command(
    OUTPUT ${doc_format_output}
    COMMAND
        Sphinx::Build
        -c ${CMAKE_CURRENT_BINARY_DIR}
        -d ${CMAKE_CURRENT_BINARY_DIR}/doctrees
        -b ${doc_format}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/${doc_format}
        > ${doc_format_log} # log stdout, pass stderr
    ${${doc_format}_extra_commands}
    COMMENT "sphinx-build ${doc_format}: see ${CMAKE_CURRENT_BINARY_DIR}/${doc_format_log}"
    VERBATIM
)
set_property(SOURCE ${doc_format_output} PROPERTY SYMBOLIC 1)

add_custom_target(documentation ALL DEPENDS ${doc_format_output})
