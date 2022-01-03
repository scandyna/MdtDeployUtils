/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2022 Philippe Steinmann.
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
#ifndef PATH_TEST_H
#define PATH_TEST_H

#include <QObject>
#include <QtTest/QTest>

/*
 * NOTE: new tests for PathList
 *       will now be written in PathListTest.cpp
 */
class PathTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void pathListStringListTest();
  void pathListAppendTest();
  void pathListAppendTest_data();
  void pathListAppendPathListTest();
  void pathListAppendPathListTest_data();
  void pathListPrependTest();
  void pathListPrependTest_data();
  void pathListPrependPathListTest();
  void pathListPrependPathListTest_data();

  void searchPathListTest();
};

#endif // #ifndef PATH_TEST_H
