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

/** \todo Maybe return a custom struct with the app specific data
 *
 * This could be re-used for a example
 */
/*
class SimpleCopyAppCommandLineParserResult
{
 public:

  bool isValid() const
  {
  }

 private:

};
*/

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

    if( mResult.hasError() ){
      return false;
    }
    if( !mResult.hasOptions() && !mResult.hasPositionalArguments() ){
      mResult.setErrorText( QLatin1String("Expected at least arguments or a option") );
      return false;
    }

    return true;
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

    if( mResult.hasError() ){
      return false;
    }
    if( mResult.hasSubCommand() ){
      if( !mResult.subCommand().hasOptions() && !mResult.subCommand().hasPositionalArguments() ){
        mResult.setErrorText( QLatin1String("Expected at least arguments or a option") );
        return false;
      }
    }else{
      if( !mResult.hasOptions() && !mResult.hasPositionalArguments() ){
        mResult.setErrorText( QLatin1String("Expected at least arguments or a option") );
        return false;
      }
    }

    return true;
  }

  bool helpRequested() const
  {
    return mResult.isHelpOptionSet();
  }

  bool copyHelpRequested() const
  {
    if( !isCopyCommand() ){
      return false;
    }
    return mResult.subCommand().isHelpOptionSet();
  }

  QString copySourceFile() const
  {
    assert( isCopyCommand() );
    assert( mResult.subCommand().hasPositionalArgumentAt(0) );

    return mResult.subCommand().positionalArgumentAt(0);
  }

  QString copyDestinationDirectory() const
  {
    assert( isCopyCommand() );
    assert( mResult.subCommand().hasPositionalArgumentAt(1) );

    return mResult.subCommand().positionalArgumentAt(1);
  }

 private:

  bool isCopyCommand() const
  {
    return mResult.subCommand().name() == QLatin1String("copy");
  }

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

TEST_CASE("splitToMainAndSubCommandArguments")
{
  using Impl::splitToMainAndSubCommandArguments;

  ParserDefinition parserDefinition;
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
      splitToMainAndSubCommandArguments(arguments, parserDefinition, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-h")
    {
      arguments << QLatin1String("-h");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-h");
      splitToMainAndSubCommandArguments(arguments, parserDefinition, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-f,copy,file.txt,/tmp")
    {
      arguments << QLatin1String("-f") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-f") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      splitToMainAndSubCommandArguments(arguments, parserDefinition, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }
  }

  SECTION("copy and list sub-commands")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    ParserDefinitionCommand listCommand( QLatin1String("list") );
    parserDefinition.addSubCommand(copyCommand);
    parserDefinition.addSubCommand(listCommand);

    SECTION("myapp")
    {
      expectedMainArguments << QLatin1String("myapp");
      splitToMainAndSubCommandArguments(arguments, parserDefinition, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-h")
    {
      arguments << QLatin1String("-h");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-h");
      splitToMainAndSubCommandArguments(arguments, parserDefinition, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-f,copy,file.txt,/tmp")
    {
      arguments << QLatin1String("-f") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-f");
      expectedSubCommandArguments << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      splitToMainAndSubCommandArguments(arguments, parserDefinition, mainArguments, subCommandArguments);
      REQUIRE( mainArguments == expectedMainArguments );
      REQUIRE( subCommandArguments == expectedSubCommandArguments );
    }

    SECTION("myapp,-f,copy,file.txt,/tmp,copy")
    {
      arguments << QLatin1String("-f") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp") << QLatin1String("copy");
      expectedMainArguments << QLatin1String("myapp") << QLatin1String("-f");
      expectedSubCommandArguments << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp") << QLatin1String("copy");
      splitToMainAndSubCommandArguments(arguments, parserDefinition, mainArguments, subCommandArguments);
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

TEST_CASE("resultOptionFromName")
{
  using Impl::resultOptionFromName;

  std::vector<ParserDefinitionOption> knownOptions;
  knownOptions.emplace_back( 'h', QLatin1String("help"), QLatin1String("Help option") );
  knownOptions.emplace_back( 'f', QLatin1String("force"), QLatin1String("Force option") );

  SECTION("help")
  {
    REQUIRE( resultOptionFromName( QLatin1String("help"), knownOptions ).name() == QLatin1String("help") );
  }

  SECTION("h")
  {
    REQUIRE( resultOptionFromName( QLatin1String("h"), knownOptions ).name() == QLatin1String("help") );
  }

  SECTION("force")
  {
    REQUIRE( resultOptionFromName( QLatin1String("force"), knownOptions ).name() == QLatin1String("force") );
  }

  SECTION("f")
  {
    REQUIRE( resultOptionFromName( QLatin1String("f"), knownOptions ).name() == QLatin1String("force") );
  }
}

TEST_CASE("fillResultCommandFromQtParser")
{
  using Impl::fillResultCommandFromQtParser;
  using Impl::setupQtParser;

  ParserDefinitionCommand defCommand;
  ParserResultCommand resultCommand;
  QCommandLineParser qtParser;
  QStringList arguments{QLatin1String("myapp")};

  SECTION("myapp --help")
  {
    defCommand.addHelpOption();
    setupQtParser(qtParser, defCommand);
    arguments << QLatin1String("--help");
    REQUIRE( qtParser.parse(arguments) );
    fillResultCommandFromQtParser(resultCommand, qtParser, defCommand);
    REQUIRE( resultCommand.isHelpOptionSet() );
  }

  SECTION("myapp -h")
  {
    defCommand.addHelpOption();
    setupQtParser(qtParser, defCommand);
    arguments << QLatin1String("-h");
    REQUIRE( qtParser.parse(arguments) );
    fillResultCommandFromQtParser(resultCommand, qtParser, defCommand);
    REQUIRE( resultCommand.isHelpOptionSet() );
  }

  SECTION("myapp --force file.txt /tmp")
  {
    defCommand.addOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
    setupQtParser(qtParser, defCommand);
    arguments << QLatin1String("--force") << QLatin1String("file.txt") << QLatin1String("/tmp");
    REQUIRE( qtParser.parse(arguments) );
    fillResultCommandFromQtParser(resultCommand, qtParser, defCommand);
    REQUIRE( resultCommand.isSet( QLatin1String("force") ) );
    REQUIRE( resultCommand.positionalArgumentCount() == 2 );
    REQUIRE( resultCommand.positionalArgumentAt(0) == QLatin1String("file.txt") );
    REQUIRE( resultCommand.positionalArgumentAt(1) == QLatin1String("/tmp") );
  }
}
