/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#ifndef TEST_IS_EXISTING_SHARED_LIBRARY_H
#define TEST_IS_EXISTING_SHARED_LIBRARY_H

#include "Mdt/DeployUtils/AbstractIsExistingValidSharedLibrary.h"
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <vector>
#include <string>
#include <cassert>

class TestIsExistingSharedLibrary : public Mdt::DeployUtils::AbstractIsExistingValidSharedLibrary
{
 public:

  void setExistingSharedLibraries(const std::vector<std::string> & libraries)
  {
    for(const auto & library : libraries){
      const QFileInfo libraryFile( QString::fromStdString(library) );
      assert( libraryFile.isAbsolute() );
      mExistingSharedLibraries.append( libraryFile.absoluteFilePath() );
    }
  }

 private:

  bool doIsExistingValidSharedLibrary(const QFileInfo & libraryFile) const override
  {
    return mExistingSharedLibraries.contains( libraryFile.absoluteFilePath() );
  }

  QStringList mExistingSharedLibraries;
};

#endif // #ifndef TEST_IS_EXISTING_SHARED_LIBRARY_H
