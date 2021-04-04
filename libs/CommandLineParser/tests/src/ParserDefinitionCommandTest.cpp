/****************************************************************************
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
 **
 ** This file is part of MdtApplication library.
 **
 ** MdtApplication is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** MdtApplication is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with MdtApplication.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "Mdt/CommandLineParser/Impl/ParserDefinitionCommand.h"
#include "Mdt/CommandLineParser/ParserDefinitionCommand.h"
#include "Mdt/CommandLineParser/ParserDefinitionPositionalArgument.h"
#include <QString>
#include <QLatin1String>

using namespace Mdt::CommandLineParser;

QString getUsageText(const QString & applicationName, const ParserDefinitionCommand & command)
{
  return Impl::ParserDefinitionCommandHelp::getUsageText(applicationName, command);
}


TEST_CASE("getUsageText")
{
  const QString applicationName = QLatin1String("myapp");
  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  QString expectedText;

  ParserDefinitionPositionalArgument sourceFile( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );

  SECTION("No arguments and no options")
  {
    expectedText = QLatin1String("Usage: myapp copy");
    REQUIRE( getUsageText(applicationName, copyCommand) == expectedText );
  }

  SECTION("Arguments")
  {
    copyCommand.addPositionalArgument(sourceFile);

    expectedText = QLatin1String("Usage: myapp copy source");
    REQUIRE( getUsageText(applicationName, copyCommand) == expectedText );
  }

  SECTION("Arguments with syntax")
  {
    ParserDefinitionPositionalArgument argument( ValueType::File, QLatin1String("source"), QLatin1String("Source file"), QLatin1String("[file]") );
    copyCommand.addPositionalArgument(argument);

    expectedText = QLatin1String("Usage: myapp copy [file]");
    REQUIRE( getUsageText(applicationName, copyCommand) == expectedText );
  }

  SECTION("Options")
  {
    copyCommand.addHelpOption();

    expectedText = QLatin1String("Usage: myapp copy [options]");
    REQUIRE( getUsageText(applicationName, copyCommand) == expectedText );
  }

  SECTION("Options and arguments")
  {
    copyCommand.addHelpOption();
    copyCommand.addPositionalArgument(sourceFile);

    expectedText = QLatin1String("Usage: myapp copy [options] source");
    REQUIRE( getUsageText(applicationName, copyCommand) == expectedText );
  }
}
