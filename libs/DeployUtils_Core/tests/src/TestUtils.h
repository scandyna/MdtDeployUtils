/****************************************************************************
 **
 ** Copyright (C) 2020-2023 Philippe Steinmann.
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
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "Mdt/DeployUtils/LibraryName.h"
#include "Mdt/DeployUtils/BuildType.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QTemporaryDir>
#include <vector>
#include <string>
#include <algorithm>


Mdt::DeployUtils::BuildType currentBuildType();

Mdt::DeployUtils::Platform getNonNativePlatform();

QStringList qStringListFromUtf8Strings(const std::vector<std::string> & args);

QString generateStringWithNChars(int n);

QStringList getTestPrefixPath(const char * const prefixPath);

template<typename Container, typename GetLibraryName>
bool containsLibrary(const Container & libraries, GetLibraryName getLibraryName, const QString & libraryName)
{
  const Mdt::DeployUtils::LibraryName searchedLibraryName(libraryName);

  const auto pred = [&searchedLibraryName, &getLibraryName](const auto & currentItem){
    const QFileInfo libraryFile( getLibraryName(currentItem) );
    const Mdt::DeployUtils::LibraryName currentLibraryName( libraryFile.fileName() );
    return currentLibraryName.nameWithoutDebugSuffix() == searchedLibraryName.nameWithoutDebugSuffix();
  };

  const auto it = std::find_if(libraries.cbegin(), libraries.cend(), pred);

  return it != libraries.cend();
}

bool containsLibrary(const QStringList & libraries, const QString & libraryName);

bool dirContainsLibrary(const QTemporaryDir & dir, const QString & libraryName);

bool containsQt5Core(const QStringList & libraries);

bool dirContainsQt5Core(const QTemporaryDir & dir);

bool containsTestSharedLibrary(const QStringList & libraries);

bool dirContainsTestSharedLibrary(const QTemporaryDir & dir);

#endif // #ifndef TEST_UTILS_H
