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
#include "IsExistingValidSharedLibrary.h"
#include "QtSharedLibraryFile.h"
#include "QtSharedLibrary.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

bool IsExistingValidSharedLibrary::doIsExistingValidSharedLibrary(const QFileInfo & libraryFile) const
{
  if( !libraryFile.exists() ){
    return false;
  }
  if( !isSharedLibraryForExpectedPlatform(libraryFile) ){
    return false;
  }

  if( QtSharedLibraryFile::isQtSharedLibrary(libraryFile) ){
    if( !QtSharedLibrary::sharedLibraryIsInQtDistribution(libraryFile) ){
      return false;
    }
  }

  return true;
}

bool IsExistingValidSharedLibrary::isSharedLibraryForExpectedPlatform(const QFileInfo & libraryFile) const
{
  assert( !mReader.isOpen() );

  try{
    mReader.openFile(libraryFile, mPlatform);

    const bool isSharedLibrary = mReader.isExecutableOrSharedLibrary();

    const Platform libraryPlatform = mReader.getFilePlatform();
    const bool isCorrectProcessorISA = ( libraryPlatform.processorISA() == mPlatform.processorISA() );

    mReader.close();

    return isSharedLibrary && isCorrectProcessorISA;
  }catch(...){
    mReader.close();
    throw;
  }
}

}} // namespace Mdt{ namespace DeployUtils{
