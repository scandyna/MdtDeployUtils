/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
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
#include "Catch2QString.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/CMakeStyleMessageLogger.h"

using namespace Mdt::DeployUtils;

TEST_CASE("Init")
{
  REQUIRE( !MessageLogger::isInitialized() );

  MessageLogger messageLogger;
  REQUIRE( MessageLogger::isInitialized() );
}

TEST_CASE("Cleanup")
{
  {
    MessageLogger messageLogger;
    REQUIRE( MessageLogger::isInitialized() );
  }
  REQUIRE( !MessageLogger::isInitialized() );
}

TEST_CASE("set_CMakeStyleMessageLogger_backend")
{
  MessageLogger messageLogger;

  auto *backend = MessageLogger::setBackend<CMakeStyleMessageLogger>();
  REQUIRE( backend != nullptr );
}
