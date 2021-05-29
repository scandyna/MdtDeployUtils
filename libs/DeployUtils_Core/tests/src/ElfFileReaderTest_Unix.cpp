/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/ElfFileReader.h"
#include "Mdt/DeployUtils/Impl/Elf/ElfFileReader.h"
#include <QString>
#include <QLatin1String>

#include <QDebug>

using namespace Mdt::DeployUtils;

TEST_CASE("Sandbox")
{
  qDebug() << "TEST_SHARED_LIBRARY_FILE_PATH: " << TEST_SHARED_LIBRARY_FILE_PATH;

  ElfFileReader::sandbox( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
}

TEST_CASE("isElfFile")
{
  REQUIRE( ElfFileReader::isElfFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) ) );
}

/** \todo Check that at least QtCore and DeployUtils are found,
 * or at least that returned list is not empty
 */
