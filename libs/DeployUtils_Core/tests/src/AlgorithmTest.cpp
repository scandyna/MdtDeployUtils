/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiler binaries
 **
 ** Copyright (C) 2020-2020 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Mdt/DeployUtils/Algorithm.h"
#include <QLatin1String>
#include <QString>
#include <QFileInfo>
#include <vector>
#include <string>


QFileInfo fileInfoFromString(const std::string & file)
{
  return QFileInfo( QString::fromStdString(file) );
}

TEST_CASE("stringStartsWith")
{
  using Mdt::DeployUtils::stringStartsWith;

  std::string str;

  SECTION("str empty")
  {
    SECTION("s A")
    {
      REQUIRE( !stringStartsWith(str, "A") );
    }
  }

  SECTION("str A")
  {
    str = "A";

    SECTION("s A")
    {
      REQUIRE( stringStartsWith(str, "A") );
    }

    SECTION("s a")
    {
      REQUIRE( !stringStartsWith(str, "a") );
    }

    SECTION("s B")
    {
      REQUIRE( !stringStartsWith(str, "B") );
    }
  }

  SECTION("str AB")
  {
    str = "AB";

    SECTION("s A")
    {
      REQUIRE( stringStartsWith(str, "A") );
    }

    SECTION("s a")
    {
      REQUIRE( !stringStartsWith(str, "a") );
    }

    SECTION("s B")
    {
      REQUIRE( !stringStartsWith(str, "B") );
    }

    SECTION("s AA")
    {
      REQUIRE( !stringStartsWith(str, "AA") );
    }

    SECTION("s AB")
    {
      REQUIRE( stringStartsWith(str, "AB") );
    }

    SECTION("s ABC")
    {
      REQUIRE( !stringStartsWith(str, "ABC") );
    }
  }
}

TEST_CASE("joinToStdString")
{
  using Mdt::DeployUtils::joinToStdString;

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
  using Mdt::DeployUtils::appendToStdVector;

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
