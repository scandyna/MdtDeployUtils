########################################################################################
#
# Copyright (C) 2019-2021 Philippe Steinmann.
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
include(MdtAddTest)

mdt_add_test(
  NAME AlgorithmTest_MdtApplication
  TARGET algorithmTest_MdtApplication
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main
  SOURCE_FILES
    src/AlgorithmTest.cpp
)

mdt_add_test(
  NAME ParserDefinitionHelpUtilsTest
  TARGET parserDefinitionHelpUtilsTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ParserDefinitionHelpUtilsTest.cpp
)

mdt_add_test(
  NAME ParserDefinitionCommandTest
  TARGET parserDefinitionCommandTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ParserDefinitionCommandTest.cpp
)

mdt_add_test(
  NAME ParserDefinitionTest
  TARGET parserDefinitionTest
  DEPENDENCIES Mdt::CommandLineParser Mdt0::CommandLineArguments Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ParserDefinitionTest.cpp
)

mdt_add_test(
  NAME CommandLineTest
  TARGET commandLineTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CommandLineTest.cpp
)

mdt_add_test(
  NAME CommandLineArgumentListAttributesTest
  TARGET commandLineArgumentListAttributesTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CommandLineArgumentListAttributesTest.cpp
)

mdt_add_test(
  NAME ParserResultTest
  TARGET parserResultTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ParserResultTest.cpp
)

mdt_add_test(
  NAME ParserResultFromCommandLineTest
  TARGET parserResultFromCommandLineTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ParserResultFromCommandLineTest.cpp
)

mdt_add_test(
  NAME ParserTest
  TARGET parserTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ParserTest.cpp
)

mdt_add_test(
  NAME ParserImplTest
  TARGET parserImplTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ParserImplTest.cpp
)

mdt_add_test(
  NAME BashCompletionCompgenCommandTest
  TARGET bashCompletionCompgenCommandTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BashCompletionCompgenCommandTest.cpp
)

mdt_add_test(
  NAME BashCompletionActionTest
  TARGET bashCompletionActionTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BashCompletionActionTest.cpp
)

mdt_add_test(
  NAME BashCompletionScriptTest
  TARGET bashCompletionScriptTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BashCompletionScriptTest.cpp
)

mdt_add_test(
  NAME BashCompletionGeneratorTest
  TARGET bashCompletionGeneratorTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BashCompletionGeneratorTest.cpp
)

mdt_add_test(
  NAME BashCompletionParserTest
  TARGET bashCompletionParserTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BashCompletionParserTest.cpp
)

mdt_add_test(
  NAME BashCompletionParserQueryTest
  TARGET bashCompletionParserQueryTest
  DEPENDENCIES Mdt::CommandLineParser Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BashCompletionParserQueryTest.cpp
)
