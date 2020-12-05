/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2020 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "BashCompletionGenerator.h"
#include "BashCompletionGenerator_Impl.h"

#include <iostream>

TEST_CASE("option")
{
  SECTION("short name and name")
  {
    BashCompletionGeneratorOption option('h', "help");
    REQUIRE( option.hasShortName() );
    REQUIRE( option.shortNameWithDash() == "-h" );
    REQUIRE( option.nameWithDashes() == "--help" );
  }

  SECTION("name only")
  {
    BashCompletionGeneratorOption option("help");
    REQUIRE( !option.hasShortName() );
    REQUIRE( option.nameWithDashes() == "--help" );
  }
}

TEST_CASE("Command_isEmpty")
{
  BashCompletionGeneratorCommand command("command");
  REQUIRE( command.isEmpty() );

  command.addArgument("arg-a");
  REQUIRE( !command.isEmpty() );
}

TEST_CASE("ApplicationName")
{
  BashCompletionGenerator generator;
  REQUIRE( generator.applicationName().empty() );

  generator.setApplicationName("mytool");
  REQUIRE( generator.applicationName() == std::string("mytool") );
}

TEST_CASE("sandbox")
{
  BashCompletionGenerator generator;
  generator.setApplicationName("mytool");

  BashCompletionGeneratorCommand mainCommand("main");
  mainCommand.addArgument("copy");
  mainCommand.addOption('h', "help");
  generator.setMainCommand(mainCommand);

  BashCompletionGeneratorCommand copyCommand("copy");
  copyCommand.addArgument("source");
  copyCommand.addArgument("destination");
  copyCommand.setDirectoryCompletionEnabled(true);
  copyCommand.addOption('h', "help");
  copyCommand.addOption('i', "interactive");
  generator.addSubCommand(copyCommand);


  std::cout << generator.generateScript() << std::endl;
  
  REQUIRE(false);
}
