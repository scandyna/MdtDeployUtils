########################################################################################
#
# Copyright (C) 2019-2020 Philippe Steinmann.
#
# This file is part of MdtApplication library.
#
# MdtApplication is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MdtApplication is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with MdtApplication. If not, see <http://www.gnu.org/licenses/>.
#
########################################################################################
include(GenerateExportHeader)
include(MdtInstallLibrary)

add_library(Mdt_CommandLineParser
  Mdt/CommandLineParser/Algorithm.cpp
  Mdt/CommandLineParser/ArgumentType.cpp
  Mdt/CommandLineParser/ParserDefinitionOption.cpp
  Mdt/CommandLineParser/ParserDefinitionPositionalArgument.cpp
  Mdt/CommandLineParser/ParserDefinitionCommand.cpp
  Mdt/CommandLineParser/ParserDefinition.cpp
  Mdt/CommandLineParser/Parser.cpp
  Mdt/CommandLineParser/ParserResultOption.cpp
  Mdt/CommandLineParser/ParserResultCommand.cpp
  Mdt/CommandLineParser/ParserResult.cpp
  Mdt/CommandLineParser/BashCompletionGeneratorPositionalArgument.cpp
  Mdt/CommandLineParser/BashCompletionGeneratorOption.cpp
  Mdt/CommandLineParser/BashCompletionGeneratorCommand.cpp
  Mdt/CommandLineParser/BashCompletionGenerator.cpp
  Mdt/CommandLineParser/BashCompletionParser.cpp
)
add_library(Mdt::CommandLineParser ALIAS Mdt_CommandLineParser)


if(BUILD_USE_IPO_LTO)
  set_target_properties(Mdt_CommandLineParser
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

target_link_libraries(Mdt_CommandLineParser
  PUBLIC
    Qt5::Core
    Boost::boost
)

target_include_directories(Mdt_CommandLineParser
  PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

generate_export_header(Mdt_CommandLineParser)

# mdt_install_library(
#   TARGET Mdt_ConsoleApplication
#   RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#   LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#   ARCHIVE_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#   INCLUDES_DIRECTORY .
#   INCLUDES_FILE_WITHOUT_EXTENSION
#   ADDITIONAL_INCLUDES_FILES "${CMAKE_CURRENT_BINARY_DIR}/mdt_consoleapplication_export.h"
#   INCLUDES_DESTINATION ${MDT_INSTALL_INCLUDEDIR}
#   EXPORT_NAME ConsoleApplication
#   EXPORT_NAMESPACE Mdt${PROJECT_VERSION_MAJOR}::
#   INSTALL_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
#   FIND_PACKAGE_PATHS ..
#   INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#   VERSION ${PROJECT_VERSION}
#   SOVERSION ${PROJECT_VERSION_MAJOR}
#   VERSION_COMPATIBILITY ExactVersion
#   RUNTIME_COMPONENT Mdt_MdtApplication_Runtime
#   DEVELOPMENT_COMPONENT MdtApplication_Dev
# )