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
#include "TestUtils.h"
#include "Mdt/CommandLineParser/Parser.h"
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/ParserResult.h"
#include <QLatin1String>
#include <QStringList>
#include <cassert>

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
    mDefinition.addPositionalArgument( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
    mDefinition.addPositionalArgument( ValueType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
  }

  bool parseWithoutPostChecks(const QStringList & arguments)
  {
    Parser parser(mDefinition);
    if( !parser.parse(arguments) ){
      return false;
    }
    mResult = parser.toParserResult();

    return true;
  }

  bool parse(const QStringList & arguments)
  {
    if( !parseWithoutPostChecks(arguments) ){
      return false;
    }

    if( !mResult.hasOptions() && !mResult.hasPositionalArguments() ){
//       mResult.setErrorText( QLatin1String("Expected at least arguments or a option") );
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
    mCopyCommandDefinition.addPositionalArgument( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
    mCopyCommandDefinition.addPositionalArgument( ValueType::File, QLatin1String("destination"), QLatin1String("Destination directory") );
    mDefinition.addSubCommand(mCopyCommandDefinition);
  }

  bool parseWithoutPostChecks(const QStringList & arguments)
  {
    Parser parser(mDefinition);
    if( !parser.parse(arguments) ){
      return false;
    }
    mResult = parser.toParserResult();

    return true;
  }

  bool parse(const QStringList & arguments)
  {
    if( !parseWithoutPostChecks(arguments) ){
      return false;
    }

    if( mResult.hasSubCommand() ){
      if( !mResult.subCommand().hasOptions() && !mResult.subCommand().hasPositionalArguments() ){
//         mResult.setErrorText( QLatin1String("Expected at least arguments or a option") );
        return false;
      }
    }else{
      if( !mResult.hasOptions() && !mResult.hasPositionalArguments() ){
//         mResult.setErrorText( QLatin1String("Expected at least arguments or a option") );
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
    REQUIRE( parser.parseWithoutPostChecks(arguments) );
  }

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

  SECTION("myapp file.txt /tmp arg3 arg4")
  {
    arguments << QLatin1String("file.txt") << QLatin1String("/tmp") << QLatin1String("arg3") << QLatin1String("arg4");
    REQUIRE( parser.parseWithoutPostChecks(arguments) );
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
    REQUIRE( parser.parseWithoutPostChecks(arguments) );
  }

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

  SECTION("myapp unknown")
  {
    arguments << QLatin1String("unknown");
    REQUIRE( parser.parseWithoutPostChecks(arguments) );
  }

  SECTION("myapp arg1 arg2")
  {
    arguments << QLatin1String("arg1") << QLatin1String("arg2");
    REQUIRE( parser.parseWithoutPostChecks(arguments) );
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

TEST_CASE("Parser_parse")
{
  ParserDefinition parserDefinition;
  ParserResult result;
  QStringList arguments{QLatin1String("myapp")};

  SECTION("Simple app")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
    parserDefinition.addPositionalArgument( ValueType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );

    SECTION("myapp")
    {
      Parser parser(parserDefinition);
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !parser.hasError() );
      result = parser.toParserResult();
      REQUIRE( result.positionalArgumentCount() == 0 );
    }

    SECTION("myapp file1.txt")
    {
      arguments << QLatin1String("file1.txt");
      Parser parser(parserDefinition);
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !parser.hasError() );
      result = parser.toParserResult();
      REQUIRE( result.positionalArgumentCount() == 1 );
      REQUIRE( result.positionalArgumentAt(0) == QLatin1String("file1.txt") );
    }

    SECTION("myapp file1.txt /tmp other-positional-argument")
    {
      arguments << QLatin1String("file1.txt") << QLatin1String("/tmp") << QLatin1String("other-positional-argument");
      Parser parser(parserDefinition);
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !parser.hasError() );
      result = parser.toParserResult();
      REQUIRE( result.positionalArgumentCount() == 3 );
      REQUIRE( result.positionalArgumentAt(0) == QLatin1String("file1.txt") );
      REQUIRE( result.positionalArgumentAt(1) == QLatin1String("/tmp") );
      REQUIRE( result.positionalArgumentAt(2) == QLatin1String("other-positional-argument") );
    }

    SECTION("destination (option with value)")
    {
      ParserDefinitionOption destinationOption( QLatin1String("destination"), QLatin1String("Destination directory") );
      destinationOption.setValueName( QLatin1String("directory") );
      destinationOption.setDefaultValue( QLatin1String("/usr/bin") );
      parserDefinition.addOption(destinationOption);

      SECTION("myapp --destination /tmp file1.txt")
      {
        arguments << QLatin1String("--destination") << QLatin1String("/tmp") << QLatin1String("file1.txt");
        Parser parser(parserDefinition);
        REQUIRE( parser.parse(arguments) );
        REQUIRE( !parser.hasError() );
        result = parser.toParserResult();
        REQUIRE( result.getValues(destinationOption) == QStringList{QLatin1String("/tmp")} );
        REQUIRE( result.positionalArgumentCount() == 1 );
        REQUIRE( result.positionalArgumentAt(0) == QLatin1String("file1.txt") );
      }

      SECTION("myapp file1.txt")
      {
        arguments << QLatin1String("file1.txt");
        Parser parser(parserDefinition);
        REQUIRE( parser.parse(arguments) );
        REQUIRE( !parser.hasError() );
        result = parser.toParserResult();
        REQUIRE( result.getValues(destinationOption) == QStringList{QLatin1String("/usr/bin")} );
        REQUIRE( result.positionalArgumentCount() == 1 );
        REQUIRE( result.positionalArgumentAt(0) == QLatin1String("file1.txt") );
      }
    }
  }

  SECTION("App with sub-commands")
  {
    parserDefinition.addHelpOption();

    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.addHelpOption();
    copyCommand.addPositionalArgument( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
    copyCommand.addPositionalArgument( ValueType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
    parserDefinition.addSubCommand(copyCommand);

    SECTION("myapp")
    {
      Parser parser(parserDefinition);
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !parser.hasError() );
      result = parser.toParserResult();
      REQUIRE( result.positionalArgumentCount() == 0 );
    }

    SECTION("myapp copy")
    {
      arguments << QLatin1String("copy");
      Parser parser(parserDefinition);
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !parser.hasError() );
      result = parser.toParserResult();
      REQUIRE( result.positionalArgumentCount() == 0 );
      REQUIRE( result.hasSubCommand() );
      REQUIRE( result.subCommand().name() == QLatin1String("copy") );
      REQUIRE( result.subCommand().positionalArgumentCount() == 0 );
    }

    SECTION("myapp copy file1.txt /tmp")
    {
      arguments << QLatin1String("copy") << QLatin1String("file1.txt") << QLatin1String("/tmp");
      Parser parser(parserDefinition);
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !parser.hasError() );
      result = parser.toParserResult();
      REQUIRE( result.positionalArgumentCount() == 0 );
      REQUIRE( result.hasSubCommand() );
      REQUIRE( result.subCommand().positionalArgumentCount() == 2 );
      REQUIRE( result.subCommand().positionalArgumentAt(0) == QLatin1String("file1.txt") );
      REQUIRE( result.subCommand().positionalArgumentAt(1) == QLatin1String("/tmp") );
    }

    SECTION("myapp arg1 copy file1.txt /tmp arg2")
    {
      arguments << QLatin1String("arg1") << QLatin1String("copy") << QLatin1String("file1.txt") << QLatin1String("/tmp") << QLatin1String("arg2");
      Parser parser(parserDefinition);
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !parser.hasError() );
      result = parser.toParserResult();
      REQUIRE( result.positionalArgumentCount() == 1 );
      REQUIRE( result.positionalArgumentAt(0) == QLatin1String("arg1") );
      REQUIRE( result.hasSubCommand() );
      REQUIRE( result.subCommand().positionalArgumentCount() == 3 );
      REQUIRE( result.subCommand().positionalArgumentAt(0) == QLatin1String("file1.txt") );
      REQUIRE( result.subCommand().positionalArgumentAt(1) == QLatin1String("/tmp") );
      REQUIRE( result.subCommand().positionalArgumentAt(2) == QLatin1String("arg2") );
    }
  }
}

TEST_CASE("Parser_parse_error")
{
  ParserDefinition parserDefinition;
  Parser parser(parserDefinition);
  QStringList arguments{QLatin1String("myapp")};

  SECTION("myapp --unknown-option")
  {
    arguments << QLatin1String("--unknown-option");
    REQUIRE( !parser.parse(arguments) );
    REQUIRE( parser.hasError() );
    REQUIRE( !parser.errorText().isEmpty() );
  }
}

TEST_CASE("Parser_unknownOptions")
{
  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();

  Parser parser(parserDefinition);

  QStringList arguments;
  QStringList expectedNames;

  SECTION("app -h")
  {
    arguments = qStringListFromUtf8Strings({"app","-h"});

    REQUIRE( parser.parse(arguments) );
    REQUIRE( parser.getUnknownOptionNames().isEmpty() );
  }

  SECTION("app --unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"app","--unknown-option"});
    expectedNames = qStringListFromUtf8Strings({"unknown-option"});

    REQUIRE( !parser.parse(arguments) );
    REQUIRE( parser.getUnknownOptionNames() == expectedNames );
  }

  SECTION("app -h --unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"app","-h","--unknown-option"});
    expectedNames = qStringListFromUtf8Strings({"unknown-option"});

    REQUIRE( !parser.parse(arguments) );
    REQUIRE( parser.getUnknownOptionNames() == expectedNames );
  }

  SECTION("app -u --unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"app","-u","--unknown-option"});
    expectedNames = qStringListFromUtf8Strings({"u","unknown-option"});

    REQUIRE( !parser.parse(arguments) );
    REQUIRE( parser.getUnknownOptionNames() == expectedNames );
  }
}

TEST_CASE("Parser_parse_multiple_time")
{
  ParserResult result;
  ParserDefinition parserDefinition;
  parserDefinition.addOption( 'f', QLatin1String("force"), QLatin1String("Force option") );

  Parser parser(parserDefinition);

  const QStringList arguments = qStringListFromUtf8Strings({"app","-f","file.txt","/tmp"});

  REQUIRE( parser.parse(arguments) );
  result = parser.toParserResult();
  REQUIRE( result.isOptionShortNameSet('f') );
  REQUIRE( result.positionalArgumentCount() == 2 );

  REQUIRE( parser.parse(arguments) );
  result = parser.toParserResult();
  REQUIRE( result.isOptionShortNameSet('f') );
  REQUIRE( result.positionalArgumentCount() == 2 );
}
