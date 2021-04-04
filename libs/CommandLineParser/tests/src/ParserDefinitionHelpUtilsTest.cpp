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
#include "Mdt/CommandLineParser/Impl/ParserDefinitionHelpUtils.h"
#include "Mdt/CommandLineParser/ParserDefinitionOption.h"
#include "Mdt/CommandLineParser/ParserDefinitionPositionalArgument.h"
#include <QString>
#include <QLatin1String>
#include <vector>

using namespace Mdt::CommandLineParser;


TEST_CASE("isBreackableChar")
{
  using Impl::isBreackableChar;

  SECTION("space")
  {
    REQUIRE( isBreackableChar( QLatin1Char(' ') ) );
  }

  SECTION("newline")
  {
    REQUIRE( isBreackableChar( QLatin1Char('\n') ) );
  }

  SECTION("a")
  {
    REQUIRE( !isBreackableChar( QLatin1Char('a') ) );
  }

  SECTION("A")
  {
    REQUIRE( !isBreackableChar( QLatin1Char('A') ) );
  }

  SECTION("1")
  {
    REQUIRE( !isBreackableChar( QLatin1Char('1') ) );
  }
}

TEST_CASE("findBreakPoint")
{
  using Impl::findBreakPoint;

  QString text;

  SECTION("empty,2")
  {
    REQUIRE( findBreakPoint(text.cbegin(), text.cend(), 2) == text.cend() );
  }

  SECTION("A,2")
  {
    text = QLatin1String("A");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend(), 2) == text.cend() );
  }

  SECTION("A,5")
  {
    text = QLatin1String("A");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 5) == text.cend() );
  }

  SECTION("AB,2")
  {
    text = QLatin1String("AB");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 2) == text.cend() );
  }

  SECTION("AB C,2")
  {
    text = QLatin1String("AB C");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 2) == text.cbegin()+2 );
  }

  SECTION("ABC,2")
  {
    text = QLatin1String("ABC");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 2) == text.cbegin()+1 );
  }

  SECTION("AB,5")
  {
    text = QLatin1String("AB");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 5) == text.cend() );
  }
}

TEST_CASE("breakText")
{
  using Impl::breakText;

  int leftPadLength = 5;
  int maxLength = 10;
  QString text;
  QString expectedText;
  QString result;

  SECTION("empty")
  {
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("A,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("A");
    expectedText = text;
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("AB,5,2")
  {
    leftPadLength = 5;
    maxLength = 2;
    text = QLatin1String("AB");
    expectedText = text;
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("AB C,5,2")
  {
    leftPadLength = 5;
    maxLength = 2;
    text = QLatin1String("AB C");
    expectedText = QLatin1String("AB\n     C");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABC,5,2")
  {
    leftPadLength = 5;
    maxLength = 2;
    text = QLatin1String("ABC");
    expectedText = QLatin1String("A-\n     BC");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("A B C D E F G H,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("A B C D E F G H");
    expectedText = QLatin1String("A B C D E\n     F G H");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABC\nDE,lp5,ml3")
  {
    leftPadLength = 5;
    maxLength = 3;
    text = QLatin1String("ABC\nDE");
    expectedText = QLatin1String("ABC\n     DE");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABCD EFG HIJK,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("ABCD EFG HIJK");
    expectedText = QLatin1String("ABCD EFG\n     HIJK");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABCD EFG HIJK,5,7")
  {
    leftPadLength = 5;
    maxLength = 7;
    text = QLatin1String("ABCD EFG HIJK");
    expectedText = QLatin1String("ABCD\n     EFG\n     HIJK");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABCDEFGHIJK,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("ABCDEFGHIJK");
    expectedText = QLatin1String("ABCDEFGHI-\n     JK");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABC DEFG HIJ KLMN")
  {
    leftPadLength = 5;
    maxLength = 3;
    text = QLatin1String("ABC DEFG HIJ KLMN");
    expectedText = QLatin1String("ABC\n     DE-\n     FG\n     HIJ\n     KL-\n     MN");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABCDEFG HIJ KLMNO,lp5,ml3")
  {
    leftPadLength = 5;
    maxLength = 3;
    text = QLatin1String("ABCDEFG HIJ KLMNO");
    expectedText = QLatin1String("AB-\n     CD-\n     EFG\n     HIJ\n     KL-\n     MNO");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABC DEFGHIJ KLM,lp5,ml3")
  {
    leftPadLength = 5;
    maxLength = 3;
    text = QLatin1String("ABC DEFGHIJ KLM");
    expectedText = QLatin1String("ABC\n     DE-\n     FG-\n     HIJ\n     KLM");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }
}

TEST_CASE("wrapText")
{
  using Impl::wrapText;

  QString names;
  QString description;
  QString expectedText;
  int longestNamesStringLengt;
  int maxLength;
  QString result;

  SECTION("-h, --help,longest:10,ml:50")
  {
    names = QLatin1String("-h, --help");
    description = QLatin1String("Display this help");
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  -h, --help  Display this help");
    result = wrapText(names, longestNamesStringLengt, description, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("-h, --help,longest:15,ml:50")
  {
    names = QLatin1String("-h, --help");
    description = QLatin1String("Display this help");
    longestNamesStringLengt = 15;
    maxLength = 50;

    expectedText = QLatin1String("  -h, --help       Display this help");
    result = wrapText(names, longestNamesStringLengt, description, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("-h, --help,longest:10,ml:25")
  {
    names = QLatin1String("-h, --help");
    description = QLatin1String("Display this help");
    longestNamesStringLengt = 10;
    maxLength = 25;

    expectedText = QLatin1String("  -h, --help  Display\n"
                                 "              this help");
    result = wrapText(names, longestNamesStringLengt, description, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("-h, --help,no description,longest:10,ml:50")
  {
    names = QLatin1String("-h, --help");
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  -h, --help");
    result = wrapText(names, longestNamesStringLengt, description, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("copy,description with new-line")
  {
    names = QLatin1String("copy");
    description = QLatin1String("Copy a file:\narguments are source and destination");
    longestNamesStringLengt = names.length();
    maxLength = 100;

    expectedText = QLatin1String("  copy  Copy a file:\n"
                                 "        arguments are source and destination");
    result = wrapText(names, longestNamesStringLengt, description, maxLength);
    REQUIRE( result == expectedText );
  }
}

TEST_CASE("wrapText_option")
{
  using Impl::wrapText;

  QString expectedText;
  int longestNamesStringLengt;
  int maxLength;
  QString result;

  SECTION("--help,longest:10,ml:50")
  {
    ParserDefinitionOption helpOption( QLatin1String("help"), QLatin1String("Display this help") );
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  --help      Display this help");
    result = wrapText(helpOption, longestNamesStringLengt, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("-h, --help,longest:10,ml:50")
  {
    ParserDefinitionOption helpOption( 'h', QLatin1String("help"), QLatin1String("Display this help") );
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  -h, --help  Display this help");
    result = wrapText(helpOption, longestNamesStringLengt, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("-d, --destination <diretory>,longest:29,ml:50")
  {
    ParserDefinitionOption destinationOption( 'd', QLatin1String("destination"), QLatin1String("Destination directory") );
    destinationOption.setValueName( QLatin1String("directory") );
    longestNamesStringLengt = 29;
    maxLength = 80;

    expectedText = QLatin1String("  -d, --destination <directory>  Destination directory");
    result = wrapText(destinationOption, longestNamesStringLengt, maxLength);
    REQUIRE( result == expectedText );
  }
}

TEST_CASE("wrapText_argument")
{
  using Impl::wrapText;

  QString expectedText;
  int longestNamesStringLengt;
  int maxLength;
  QString result;

  SECTION("source,longest:10,ml:50")
  {
    ParserDefinitionPositionalArgument sourceArgument( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  source      Source file");
    result = wrapText(sourceArgument, longestNamesStringLengt, maxLength);
    REQUIRE( result == expectedText );
  }
}

TEST_CASE("optionNamesStringLength")
{
  using Impl::optionNamesStringLength;

  SECTION("--help")
  {
    ParserDefinitionOption option( QLatin1String("help"), QLatin1String("Display this help") );
    REQUIRE( optionNamesStringLength(option) == 6 );
  }

  SECTION("-h, --help")
  {
    ParserDefinitionOption option( 'h', QLatin1String("help"), QLatin1String("Display this help") );
    REQUIRE( optionNamesStringLength(option) == 10 );
  }

  SECTION("--destination <directory>")
  {
    ParserDefinitionOption option( QLatin1String("destination"), QLatin1String("Destination directory") );
    option.setValueName( QLatin1String("directory") );
    REQUIRE( optionNamesStringLength(option) == 25 );
  }

  SECTION("-d, --destination <directory>")
  {
    ParserDefinitionOption option( 'd', QLatin1String("destination"), QLatin1String("Destination directory") );
    option.setValueName( QLatin1String("directory") );
    REQUIRE( optionNamesStringLength(option) == 29 );
  }
}

TEST_CASE("findLongestOptionNamesStringLength")
{
  using Impl::findLongestOptionNamesStringLength;

  std::vector<ParserDefinitionOption> options;

  SECTION("--help")
  {
    options.emplace_back( QLatin1String("help"), QLatin1String("Display this help") );
    REQUIRE( findLongestOptionNamesStringLength(options) == 6 );
  }

  SECTION("-h, --help")
  {
    options.emplace_back( 'h', QLatin1String("help"), QLatin1String("Display this help") );
    REQUIRE( findLongestOptionNamesStringLength(options) == 10 );
  }

  SECTION("--help,--verbose")
  {
    options.emplace_back( QLatin1String("help"), QLatin1String("Display this help") );
    options.emplace_back( QLatin1String("verbose"), QLatin1String("Verbose") );
    REQUIRE( findLongestOptionNamesStringLength(options) == 9 );
  }
}

TEST_CASE("findLongestArgumentNamesStringLength")
{
  using Impl::findLongestArgumentNamesStringLength;

  std::vector<ParserDefinitionPositionalArgument> arguments;

  SECTION("source")
  {
    arguments.emplace_back( ValueType::File, QLatin1String("source"), QLatin1String("Source description") );
    REQUIRE( findLongestArgumentNamesStringLength(arguments) == 6 );
  }

  SECTION("source,destination")
  {
    arguments.emplace_back( ValueType::File, QLatin1String("source"), QLatin1String("Source description") );
    arguments.emplace_back( ValueType::Directory, QLatin1String("destination"), QLatin1String("Destination description") );
    REQUIRE( findLongestArgumentNamesStringLength(arguments) == 11 );
  }
}

TEST_CASE("argumentToUsageString")
{
  using Impl::argumentToUsageString;

  SECTION("No syntax")
  {
    ParserDefinitionPositionalArgument argument( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
    REQUIRE( argumentToUsageString(argument) == QLatin1String("source") );
  }

  SECTION("With syntax")
  {
    ParserDefinitionPositionalArgument argument( ValueType::File, QLatin1String("source"), QLatin1String("Source file"), QLatin1String("[file]") );
    REQUIRE( argumentToUsageString(argument) == QLatin1String("[file]") );
  }
}

TEST_CASE("argumentListToUsageString")
{
  using Impl::argumentListToUsageString;

  std::vector<ParserDefinitionPositionalArgument> arguments;
  arguments.emplace_back( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
  arguments.emplace_back( ValueType::Directory, QLatin1String("destination"), QLatin1String("Destination diretory") );

  REQUIRE( argumentListToUsageString(arguments) == QLatin1String("source destination") );
}
