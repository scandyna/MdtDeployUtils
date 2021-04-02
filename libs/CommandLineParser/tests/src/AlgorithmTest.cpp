/****************************************************************************
 **
 ** Copyright (C) 2011-2021 Philippe Steinmann.
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
#include "Mdt/CommandLineParser/Algorithm.h"
#include <QString>
#include <QLatin1String>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>

struct MyInt
{
  int value = 0;
};

std::vector<MyInt> createIntList(const std::vector<int> & list)
{
  std::vector<MyInt> myIntList;

  const auto op = [](int value){
    return MyInt{value};
  };
  std::transform(list.cbegin(), list.cend(), std::back_inserter(myIntList), op);

  return myIntList;
}

TEST_CASE("forEachUntil")
{
  using Mdt::CommandLineParser::forEachUntil;
  using V = std::vector<int>;

  V result;

  const auto f = [&result](const int & i){
    result.push_back(i);
  };

  const auto p = [](const int & i){
    return i == 3;
  };

  SECTION("empty")
  {
    const V v;
    REQUIRE( forEachUntil( v.cbegin(), v.cend(), f, p ) == v.cend() );
    REQUIRE( result == V{} );
  }

  SECTION("1")
  {
    const V v{1};
    auto it = forEachUntil( v.cbegin(), v.cend(), f, p );
    REQUIRE( it == v.cend() );
    REQUIRE( result == V{1} );
  }

  SECTION("1,2,3")
  {
    const V v{1,2,3};
    auto it = forEachUntil( v.cbegin(), v.cend(), f, p );
    REQUIRE( it == (v.cbegin() + 2) );
    REQUIRE( result == V{1,2,3} );
  }

  SECTION("1,2,3,4")
  {
    const V v{1,2,3,4};
    auto it = forEachUntil( v.cbegin(), v.cend(), f, p );
    REQUIRE( it == (v.cbegin() + 2) );
    REQUIRE( result == V{1,2,3} );

    result.clear();
    ++it;
    it = forEachUntil( it, v.cend(), f, p );
    REQUIRE( it == v.cend() );
    REQUIRE( result == V{4} );
  }
}

TEST_CASE("qStringFromLatin1Char")
{
  using Mdt::CommandLineParser::qStringFromLatin1Char;

  REQUIRE( qStringFromLatin1Char('a') == QLatin1String("a") );
}

TEST_CASE("qStringFromSubString")
{
  using Mdt::CommandLineParser::qStringFromSubString;

  QString str;

  SECTION("empty")
  {
    REQUIRE( qStringFromSubString( str.cbegin(), str.cend() ).isEmpty() );
  }

  SECTION("[A]")
  {
    str = QLatin1String("A");
    REQUIRE( qStringFromSubString( str.cbegin(), str.cend() ) == QLatin1String("A") );
  }

  SECTION("[A]B")
  {
    str = QLatin1String("AB");
    REQUIRE( qStringFromSubString( str.cbegin(), str.cbegin()+1 ) == QLatin1String("A") );
  }

  SECTION("A[B]")
  {
    str = QLatin1String("AB");
    REQUIRE( qStringFromSubString( str.cbegin()+1, str.cend() ) == QLatin1String("B") );
  }

  SECTION("A[B]C")
  {
    str = QLatin1String("ABC");
    REQUIRE( qStringFromSubString( str.cbegin()+1, str.cbegin()+2 ) == QLatin1String("B") );
  }
}

TEST_CASE("joinToQString")
{
  using Mdt::CommandLineParser::joinToQString;

  std::vector<MyInt> myIntList;
  const auto extract = [](const MyInt & myInt){
    return QString::number(myInt.value);
  };

  SECTION("empty")
  {
    REQUIRE( joinToQString(myIntList, ';', extract).isEmpty() );
  }

  SECTION("1")
  {
    myIntList = createIntList({1});
    REQUIRE( joinToQString(myIntList, ';', extract) == QLatin1String("1") );
  }

  SECTION("1,2")
  {
    myIntList = createIntList({1,2});
    REQUIRE( joinToQString(myIntList, ';', extract) == QLatin1String("1;2") );
  }

  SECTION("1,2,3")
  {
    myIntList = createIntList({1,2,3});
    REQUIRE( joinToQString(myIntList, ';', extract) == QLatin1String("1;2;3") );
  }
}

TEST_CASE("joinToStdString")
{
  using Mdt::CommandLineParser::joinToStdString;

  SECTION("empty")
  {
    REQUIRE( joinToStdString({},';') == "" );
  }

  SECTION("A")
  {
    REQUIRE( joinToStdString({"A"},';') == "A" );
  }

  SECTION("A,B")
  {
    REQUIRE( joinToStdString({"A","B"},';') == "A;B" );
  }

  SECTION("A,B,C")
  {
    REQUIRE( joinToStdString({"A","B","C"},';') == "A;B;C" );
  }
}

TEST_CASE("appendToStdVector")
{
  using Mdt::CommandLineParser::appendToStdVector;

  std::vector<std::string> destination;
  std::vector<std::string> reference;

  SECTION("empty -> empty")
  {
    appendToStdVector({}, destination);
    reference = {};
    REQUIRE( destination == reference );
  }

  SECTION("A -> empty")
  {
    appendToStdVector({"A"}, destination);
    reference = {"A"};
    REQUIRE( destination == reference );
  }

  SECTION("empty -> A")
  {
    destination = {"A"};
    appendToStdVector({}, destination);
    reference = {"A"};
    REQUIRE( destination == reference );
  }

  SECTION("B -> A")
  {
    destination = {"A"};
    appendToStdVector({"B"}, destination);
    reference = {"A","B"};
    REQUIRE( destination == reference );
  }
}
