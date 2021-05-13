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
#include "LibraryNameBenchmark.h"
#include "Mdt/DeployUtils/LibraryName.h"
#include <QCoreApplication>
#include <QLatin1String>

using namespace Mdt::DeployUtils;

void LibraryNameBenchmark::initTestCase()
{
}

void LibraryNameBenchmark::cleanupTestCase()
{
}

/*
 * Benchmarks
 */

void LibraryNameBenchmark::fromString()
{
  LibraryName libraryName;

  QBENCHMARK{
    libraryName = LibraryName( QLatin1String("libQt5Core.so.5.9.1") );
  }

  QCOMPARE( libraryName.prefix(), QLatin1String("lib") );
  QCOMPARE( libraryName.name(), QLatin1String("Qt5Core") );
  QVERIFY( libraryName.extension().isSo() );
  QCOMPARE( libraryName.version().toString(), QLatin1String("5.9.1") );
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  LibraryNameBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
