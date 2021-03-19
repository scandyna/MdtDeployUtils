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
#include "Mdt/CommandLineParser/Impl/Parser.h"
#include "Mdt/CommandLineParser/CommandLine/Algorithm.h"
#include <QLatin1String>
#include <QStringList>
#include <string>
#include <vector>

#include <QDebug>

using namespace Mdt::CommandLineParser;
using namespace Mdt::CommandLineParser::Impl;
using namespace Mdt::CommandLineParser::CommandLine;
// using Mdt::CommandLineParser::CommandLine::CommandLine;
// using Mdt::CommandLineParser::CommandLine::getPositionalArgumentValue;

QStringList qStringListFromUtf8Strings(const std::vector<std::string> & args)
{
  QStringList arguments;

  for(const auto & arg : args){
    arguments.append( QString::fromStdString(arg) );
  }

  return arguments;
}


/**
 * should split:
 *  - parse until sub command is found (can be slow, only use if def has sub-command)
 *  - parse until --
 *  - parse the rest which does not have any option, sub command, etc...
 *
 * function to handle short options,
 * which could be combined (-abc == -a -b -c)
 * see also the rule for combination and option value
 * (if a expects a value, is -abc value valid ?,
 *  if c expects a value, is -abc value valid, etc..)
 *
 * \pre \a arg must be at least 2 chars (-h f..ex) just - not accpeted
 * parseShortOption(const QString & arg, iterator & current, ...);
 *  arg and current are redoundant
 *
 * Or better:
 * \pre \a \opt must begin with - and second must not be a dash ( meaybe see isValidShortOptionName() helper )
 * parseShortOption(char[2] opt, iterator & current, ...);
 *  maybe opt and current are redoundant
 */

/** \todo Should also handle --param key=value !
 *
 * \note QCommandLineParser threats it as a option value, see tests
 * All seems ok, just the parser must not be confused by:
 * --param key=value  -> param is option name, key=value is the value
 * --param=value
 *
 * Also test:
 *  --input - (- is the value of input)
 *
 * Also test:
 *  app copy   (copy is a sub-command name)
 *  app --copy copy
 *  app copy copy  (1 positional argument accepted in main command, so the first copy is apos arg, second is the sub-command name)
 *
 * Below is probably not interesting, except for result
 * 1) See if the syntax --param=value is something valid
 * 2) See impact on CommandLine
 * 3) See impact on ParserDefinitionOption
 * 4) See impact on ParserResultOption (should have a key attribute)
 * 5) See impact on Bash completion
 */

/*! \todo If a option expects a value, and it is not given:
 * A) If option def has no default value, should fail ?
 * B) If option def has default value: should accept and not add to the command line
 *    (case should then be handled in the parser result)
 */

TEST_CASE("findOptionByNameInCommand")
{
  ParserDefinitionCommand command;
  command.addHelpOption();

  SECTION("find help")
  {
    const auto it = findOptionByLongNameInCommand( QLatin1String("help"), command );
    REQUIRE( it != command.options().cend() );
    REQUIRE( it->name() == QLatin1String("help") );
  }

  SECTION("find h")
  {
    const auto it = findOptionByShortNameInCommand( 'h', command );
    REQUIRE( it != command.options().cend() );
    REQUIRE( it->name() == QLatin1String("help") );
  }

  SECTION("unknown option - by long name")
  {
    const auto it = findOptionByLongNameInCommand( QLatin1String("unknown"), command );
    REQUIRE( it == command.options().cend() );
  }

  SECTION("unknown option - by short name")
  {
    const auto it = findOptionByShortNameInCommand( 'u', command );
    REQUIRE( it == command.options().cend() );
  }
}

TEST_CASE("addShortOptionsToCommandLine")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force") );

  ParserDefinitionOption overwriteBehaviorOption( 'o', QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );

  ParserDefinitionCommand command;
  command.addHelpOption();
  command.addOption(forceOption);
  command.addOption(overwriteBehaviorOption);

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("-h")
  {
    arguments = qStringListFromUtf8Strings({"-h"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o keep")
  {
    arguments = qStringListFromUtf8Strings({"-o","keep"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
    REQUIRE( current == (arguments.cbegin()+1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o")
  {
    arguments = qStringListFromUtf8Strings({"-o"});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-fh")
  {
    arguments = qStringListFromUtf8Strings({"-fh"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','h'} );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo keep")
  {
    arguments = qStringListFromUtf8Strings({"-fo","keep"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
    REQUIRE( current == (arguments.cbegin()+1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo")
  {
    arguments = qStringListFromUtf8Strings({"-fo"});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-of keep")
  {
    arguments = qStringListFromUtf8Strings({"-of","keep"});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-of")
  {
    arguments = qStringListFromUtf8Strings({"-of"});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-o=keep")
  {
    arguments = qStringListFromUtf8Strings({"-o=keep"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o=")
  {
    arguments = qStringListFromUtf8Strings({"-o="});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-o=1")
  {
    arguments = qStringListFromUtf8Strings({"-o=1"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("1") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo=keep")
  {
    arguments = qStringListFromUtf8Strings({"-fo=keep"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isShortOptionListWithLastHavingValue( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o=key=value")
  {
    arguments = qStringListFromUtf8Strings({"-o=key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("key=value") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o key=value")
  {
    arguments = qStringListFromUtf8Strings({"-o","key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("key=value") );
    REQUIRE( current == (arguments.cbegin()+1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo=key=value")
  {
    arguments = qStringListFromUtf8Strings({"-fo=key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isShortOptionListWithLastHavingValue( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("key=value") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo key=value")
  {
    arguments = qStringListFromUtf8Strings({"-fo","key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("key=value") );
    REQUIRE( current == (arguments.cbegin()+1) );
    REQUIRE( !error.hasError() );
  }
}

TEST_CASE("addLongOptionToCommandLine")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force") );

  ParserDefinitionOption overwriteBehaviorOption( 'o', QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );

  ParserDefinitionCommand command;
  command.addHelpOption();
  command.addOption(forceOption);
  command.addOption(overwriteBehaviorOption);

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("--help")
  {
    arguments = qStringListFromUtf8Strings({"--help"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("help") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("--unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"--unknown-option"});
    auto current = arguments.cbegin();
    REQUIRE( !addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("--overwrite-behavior keep")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior","keep"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
    REQUIRE( current == (arguments.cbegin() + 1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("--overwrite-behavior")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior"});
    auto current = arguments.cbegin();
    REQUIRE( !addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("--overwrite-behavior=keep")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior=keep"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("--overwrite-behavior=")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior="});
    auto current = arguments.cbegin();
    REQUIRE( !addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("--overwrite-behavior=key=value")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior=key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("key=value") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("--overwrite-behavior key=value")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior","key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("key=value") );
    REQUIRE( current == (arguments.cbegin() + 1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("--p")
  {
    arguments = qStringListFromUtf8Strings({"--p"});
    auto current = arguments.cbegin();
    REQUIRE( !addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }
}

TEST_CASE("addOptionOrDashToCommandLine")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionCommand command;
  command.addHelpOption();

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("app -h")
  {
    arguments = qStringListFromUtf8Strings({"-h"});
    auto current = arguments.cbegin();
    REQUIRE( addOptionOrDashToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("app --help")
  {
    arguments = qStringListFromUtf8Strings({"--help"});
    auto current = arguments.cbegin();
    REQUIRE( addOptionOrDashToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("help") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("app -")
  {
    arguments = qStringListFromUtf8Strings({"-"});
    auto current = arguments.cbegin();
    REQUIRE( addOptionOrDashToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isSingleDash( commandLine.argumentAt(1) ) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

//   SECTION("app --")
//   {
//     arguments = qStringListFromUtf8Strings({"--"});
//     auto current = arguments.cbegin();
//     REQUIRE( addOptionOrDashToCommandLine(current, arguments.cend(), command, commandLine, error) );
//     REQUIRE( commandLine.argumentCount() == 2 );
//     REQUIRE( isDoubleDash( commandLine.argumentAt(1) ) );
//     REQUIRE( current == arguments.cbegin() );
//     REQUIRE( !error.hasError() );
//   }
}

TEST_CASE("parseAsPositionalArgument")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("-h file.txt - -- --option")
  {
    arguments = qStringListFromUtf8Strings({"-h","file.txt","-","--","--option"});
    parseAsPositionalArgument( arguments.cbegin(), arguments.cend(), commandLine );
    REQUIRE( commandLine.argumentCount() == 6 );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(1) ) == QLatin1String("-h") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(2) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(2) ) == QLatin1String("file.txt") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(3) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(3) ) == QLatin1String("-") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(4) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(4) ) == QLatin1String("--") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(5) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(5) ) == QLatin1String("--option") );
  }
}

TEST_CASE("parse")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force") );

  ParserDefinitionOption overwriteBehaviorOption( 'o', QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption(forceOption);
  parserDefinition.addOption(overwriteBehaviorOption);

  SECTION("Empty")
  {
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.isEmpty() );
  }

  SECTION("app")
  {
    arguments = qStringListFromUtf8Strings({"app"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
  }

  SECTION("app file.txt")
  {
    arguments = qStringListFromUtf8Strings({"app","file.txt"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(1) ) == QLatin1String("file.txt") );
  }

  SECTION("app -h")
  {
    arguments = qStringListFromUtf8Strings({"app","-h"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
  }

  SECTION("app --help")
  {
    arguments = qStringListFromUtf8Strings({"app","--help"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("help") );
  }

  SECTION("app --unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"app","--unknown-option"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( error.unknownOptionNames == qStringListFromUtf8Strings({"unknown-option"}) );
  }

  SECTION("app -u (unknown-option)")
  {
    arguments = qStringListFromUtf8Strings({"app","-u"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( error.unknownOptionNames == qStringListFromUtf8Strings({"u"}) );
  }

  SECTION("app -")
  {
    arguments = qStringListFromUtf8Strings({"app","-"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isSingleDash(  commandLine.argumentAt(1)  ) );
  }

  SECTION("app --")
  {
    arguments = qStringListFromUtf8Strings({"app","--"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isDoubleDash(  commandLine.argumentAt(1)  ) );
  }

  SECTION("app -fh")
  {
    arguments = qStringListFromUtf8Strings({"app","-fh"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','h'} );
  }

  SECTION("app -o keep")
  {
    arguments = qStringListFromUtf8Strings({"app","-o","keep"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOptionExpectingValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
  }

  SECTION("app -o=keep")
  {
    arguments = qStringListFromUtf8Strings({"app","-o=keep"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
  }

  SECTION("app --overwrite-behavior keep")
  {
    arguments = qStringListFromUtf8Strings({"app","--overwrite-behavior","keep"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOptionExpectingValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
  }

  SECTION("app --overwrite-behavior=keep")
  {
    arguments = qStringListFromUtf8Strings({"app","--overwrite-behavior=keep"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
  }

  SECTION("app -- --unknown-option -h (Must be added as a positional argument)")
  {
    arguments = qStringListFromUtf8Strings({"app","--","--unknown-option","-h"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 4 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isDoubleDash( commandLine.argumentAt(1) ) );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(2) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(2) ) == QLatin1String("--unknown-option") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(3) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(3) ) == QLatin1String("-h") );
    REQUIRE( !error.hasError() );
  }
}

TEST_CASE("parse_AppWithSubCommand")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force") );

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.addOption(forceOption);

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addSubCommand(copyCommand);

  SECTION("Empty")
  {
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.isEmpty() );
    REQUIRE( error.hasError() );
  }

  SECTION("app")
  {
    arguments = qStringListFromUtf8Strings({"app"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( !error.hasError() );
  }

  SECTION("app -h")
  {
    arguments = qStringListFromUtf8Strings({"app","-h"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
    REQUIRE( !error.hasError() );
  }

  SECTION("app -f")
  {
    arguments = qStringListFromUtf8Strings({"app","-f"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( error.hasError() );
  }

  SECTION("app copy -f")
  {
    arguments = qStringListFromUtf8Strings({"app","copy","-f"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isSubCommandNameArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getSubCommandName( commandLine.argumentAt(1) ) == QLatin1String("copy") );
    REQUIRE( isOption( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(2) ) == QLatin1String("f") );
    REQUIRE( !error.hasError() );
  }

  SECTION("app copy -h")
  {
    arguments = qStringListFromUtf8Strings({"app","copy","-h"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( error.hasError() );
  }

  SECTION("app copy copy")
  {
    arguments = qStringListFromUtf8Strings({"app","copy","copy"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isSubCommandNameArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getSubCommandName( commandLine.argumentAt(1) ) == QLatin1String("copy") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(2) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(2) ) == QLatin1String("copy") );
    REQUIRE( !error.hasError() );
  }
}
