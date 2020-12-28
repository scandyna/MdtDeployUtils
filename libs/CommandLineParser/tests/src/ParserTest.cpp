/****************************************************************************
 **
 ** Copyright (C) 2020-2020 Philippe Steinmann.
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
#include "Mdt/CommandLineParser/Parser.h"
#include "Mdt/CommandLineParser/Parser_Impl.h"
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/ParserResult.h"
#include <QLatin1String>
#include <QStringList>
#include <cassert>

#include <QDebug>

using namespace Mdt::CommandLineParser;

class SimpleCopyAppCommandLineParser
{
 public:

  SimpleCopyAppCommandLineParser()
  {
    mDefinition.addHelpOption();
    mDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    mDefinition.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
  }

  bool parse(const QStringList & arguments)
  {
    Parser parser;
    mResult = parser.parse(mDefinition, arguments);

    return !mResult.hasError();
  }

  bool helpRequested() const
  {
    return mResult.isHelpOptionSet();
  }

  QString sourceFile() const
  {
    assert( mResult.hasPositionalArgumentAt(0) );

    return mResult.positionalArgumentAt(0);
  }

  QString destinationDirectory() const
  {
    assert( mResult.hasPositionalArgumentAt(1) );

    return mResult.positionalArgumentAt(1);
  }

  const Mdt::CommandLineParser::ParserDefinition & definition() const noexcept
  {
    return mDefinition;
  }

  const Mdt::CommandLineParser::ParserResult & result() const noexcept
  {
    return mResult;
  }

 private:

  Mdt::CommandLineParser::ParserDefinition mDefinition;
  Mdt::CommandLineParser::ParserResult mResult;
};


class AppWithSubCommandCommandLineParser
{
 public:

  AppWithSubCommandCommandLineParser()
   : mCopyCommandDefinition( QLatin1String("copy") )
  {
    mDefinition.addHelpOption();

    mCopyCommandDefinition.addHelpOption();
    mCopyCommandDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    mCopyCommandDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("destination"), QLatin1String("Destination directory") );
    mDefinition.addSubCommand(mCopyCommandDefinition);
  }

  bool parse(const QStringList & arguments)
  {
    Parser parser;
    mResult = parser.parse(mDefinition, arguments);

    return !mResult.hasError();
  }

  bool helpRequested() const
  {
    return mResult.isHelpOptionSet();
  }

  bool copyHelpRequested() const
  {
    const auto copyCommand = mResult.findSubCommand(mCopyCommandDefinition);
    assert(copyCommand);

    return copyCommand->isHelpOptionSet();
  }

  QString copySourceFile() const
  {
    const auto copyCommand = mResult.findSubCommand(mCopyCommandDefinition);
    assert( copyCommand );
    assert( copyCommand->hasPositionalArgumentAt(0) );

    return copyCommand->positionalArgumentAt(0);
  }

  QString copyDestinationDirectory() const
  {
    const auto copyCommand = mResult.findSubCommand(mCopyCommandDefinition);
    assert( copyCommand );
    assert( copyCommand->hasPositionalArgumentAt(1) );

    return copyCommand->positionalArgumentAt(1);
  }

  const Mdt::CommandLineParser::ParserDefinition & definition() const noexcept
  {
    return mDefinition;
  }

  const Mdt::CommandLineParser::ParserResult & result() const noexcept
  {
    return mResult;
  }

 private:

  Mdt::CommandLineParser::ParserDefinitionCommand mCopyCommandDefinition;
  Mdt::CommandLineParser::ParserDefinition mDefinition;
  Mdt::CommandLineParser::ParserResult mResult;
};


TEST_CASE("SimpleCopyApp")
{
  SimpleCopyAppCommandLineParser parser;
  QStringList arguments{QLatin1String("myapp")};

  SECTION("myapp")
  {
    REQUIRE( !parser.parse(arguments) );
  }

  SECTION("myapp -h")
  {
    arguments << QLatin1String("-h");
    REQUIRE( parser.parse(arguments) );
    REQUIRE( parser.helpRequested() );
  }

  SECTION("myapp file.txt /tmp")
  {
    arguments << QLatin1String("file.txt") << QLatin1String("/tmp");
    REQUIRE( parser.parse(arguments) );
    REQUIRE( parser.sourceFile() == QLatin1String("file.txt") );
    REQUIRE( parser.destinationDirectory() == QLatin1String("/tmp") );
  }
}

TEST_CASE("AppWithSubCommand")
{
  AppWithSubCommandCommandLineParser parser;
  QStringList arguments{QLatin1String("myapp")};

  SECTION("myapp")
  {
    REQUIRE( !parser.parse(arguments) );
  }

  SECTION("myapp -h")
  {
    arguments << QLatin1String("-h");
    REQUIRE( parser.parse(arguments) );
    REQUIRE( parser.helpRequested() );
  }

  SECTION("myapp copy")
  {
    arguments << QLatin1String("copy");
    REQUIRE( !parser.parse(arguments) );
  }

  SECTION("myapp copy -h")
  {
    arguments << QLatin1String("copy") << QLatin1String("-h");
    REQUIRE( parser.parse(arguments) );
    REQUIRE( parser.copyHelpRequested() );
  }

  SECTION("myapp copy file.txt /tmp")
  {
    arguments << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
    REQUIRE( parser.parse(arguments) );
    REQUIRE( parser.copySourceFile() == QLatin1String("file.txt") );
    REQUIRE( parser.copyDestinationDirectory() == QLatin1String("/tmp") );
  }
}

TEST_CASE("containsSubCommand")
{
  using Impl::containsSubCommand;

  std::vector<ParserDefinitionCommand> subCommands;

  SECTION("No sub-command")
  {
    REQUIRE( !containsSubCommand(subCommands, QLatin1String("copy")) );
  }

  SECTION("copy,list")
  {
    subCommands.emplace_back( QLatin1String("copy") );
    subCommands.emplace_back( QLatin1String("list") );

    SECTION("rem")
    {
      REQUIRE( !containsSubCommand(subCommands, QLatin1String("rem")) );
    }

    SECTION("copy")
    {
      REQUIRE( containsSubCommand(subCommands, QLatin1String("copy")) );
    }

    SECTION("list")
    {
      REQUIRE( containsSubCommand(subCommands, QLatin1String("list")) );
    }
  }
}

TEST_CASE("splitToMainAndSubCommandArguments")
{
  using Impl::splitToMainAndSubCommandArguments;

  std::vector<ParserDefinitionCommand> subCommands;
  QStringList arguments{QLatin1String("myapp")};
  QStringList mainArguments;
  QStringList subCommandArguments;
  QStringList expectedMainArguments;
  QStringList expectedSubCommandArguments;

  SECTION("No sub-commands")
  {
    SECTION("myapp")
    {
      expectedMainArguments << QLatin1String("myapp");
      splitToMainAndSubCommandArguments(arguments, subCommands, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-h")
    {
      arguments << QLatin1String("-h");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-h");
      splitToMainAndSubCommandArguments(arguments, subCommands, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-f,copy,file.txt,/tmp")
    {
      arguments << QLatin1String("-f") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-f") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      splitToMainAndSubCommandArguments(arguments, subCommands, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }
  }

  SECTION("copy and list sub-commands")
  {
    subCommands.emplace_back( QLatin1String("copy") );
    subCommands.emplace_back( QLatin1String("list") );

    SECTION("myapp")
    {
      expectedMainArguments << QLatin1String("myapp");
      splitToMainAndSubCommandArguments(arguments, subCommands, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-h")
    {
      arguments << QLatin1String("-h");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-h");
      splitToMainAndSubCommandArguments(arguments, subCommands, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-f,copy,file.txt,/tmp")
    {
      arguments << QLatin1String("-f") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-f");
      expectedSubCommandArguments << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      splitToMainAndSubCommandArguments(arguments, subCommands, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-f,copy,file.txt,/tmp,copy")
    {
      arguments << QLatin1String("-f") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp") << QLatin1String("copy");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-f");
      expectedSubCommandArguments << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp") << QLatin1String("copy");
      splitToMainAndSubCommandArguments(arguments, subCommands, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }
  }
}

TEST_CASE("qtParserOptionFromOptionDefinition")
{
  using Impl::qtParserOptionFromOptionDefinition;

  QStringList expectedNames;

  SECTION("force")
  {
    ParserDefinitionOption forceOption( QLatin1String("force"), QLatin1String("Force option") );
    expectedNames << QLatin1String("force");

    const auto qtOption = qtParserOptionFromOptionDefinition(forceOption);
    REQUIRE( qtOption.names() == expectedNames );
  }

  SECTION("-f,force")
  {
    ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
    expectedNames << QLatin1String("f") << QLatin1String("force");

    const auto qtOption = qtParserOptionFromOptionDefinition(forceOption);
    REQUIRE( qtOption.names() == expectedNames );
  }
}

TEST_CASE("setupQtParser")
{
  using Impl::setupQtParser;

  QCommandLineParser qtParser;
  ParserDefinitionCommand command;
  QStringList arguments{QLatin1String("myapp")};
  QStringList expectedPositionalArguments;

  SECTION("No option")
  {
    setupQtParser(qtParser, command);

    arguments << QLatin1String("file.txt") << QLatin1String("/tmp");
    expectedPositionalArguments << QLatin1String("file.txt") << QLatin1String("/tmp");
    REQUIRE( qtParser.parse(arguments) );
    REQUIRE( qtParser.positionalArguments() == expectedPositionalArguments );
  }

  SECTION("options:help")
  {
    command.addHelpOption();
    setupQtParser(qtParser, command);

    arguments << QLatin1String("-h");
    REQUIRE( qtParser.parse(arguments) );
    REQUIRE( qtParser.isSet( QLatin1String("help") ) );
  }

  SECTION("options:verbose,force")
  {
    command.addOption( 'v', QLatin1String("verbose"), QLatin1String("Verbose option") );
    command.addOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
    setupQtParser(qtParser, command);

    SECTION("parse -v -f")
    {
      arguments << QLatin1String("-v") << QLatin1String("-f");
      REQUIRE( qtParser.parse(arguments) );
      REQUIRE( qtParser.isSet( QLatin1String("verbose") ) );
      REQUIRE( qtParser.isSet( QLatin1String("force") ) );
    }

    SECTION("parse --verbose --force")
    {
      arguments << QLatin1String("--verbose") << QLatin1String("--force");
      REQUIRE( qtParser.parse(arguments) );
      REQUIRE( qtParser.isSet( QLatin1String("verbose") ) );
      REQUIRE( qtParser.isSet( QLatin1String("force") ) );
    }
  }

  SECTION("copy, options:force, pargs:source,destination")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.addOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
    setupQtParser(qtParser, copyCommand);

    SECTION("parse copy -f file.txt /tmp")
    {
      arguments <<  QLatin1String("copy") << QLatin1String("-f") << QLatin1String("file.txt") << QLatin1String("/tmp");
      expectedPositionalArguments <<  QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( qtParser.parse(arguments) );
      REQUIRE( qtParser.isSet( QLatin1String("force") ) );
      REQUIRE( qtParser.positionalArguments() == expectedPositionalArguments );
    }
  }
}
