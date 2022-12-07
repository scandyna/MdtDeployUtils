/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2022 Philippe Steinmann.
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
#include "SharedLibraryFinderCommon.h"
#include "QtDistributionDirectory.h"
#include "QtSharedLibraryFile.h"
#include "FindDependencyError.h"
#include "AbstractIsExistingValidSharedLibrary.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

SharedLibraryFinderCommon::SharedLibraryFinderCommon(const AbstractIsExistingValidSharedLibrary & isExistingValidShLibOp,
                                                     std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory,
                                                     QObject *parent) noexcept
 : AbstractSharedLibraryFinder(isExistingValidShLibOp, parent),
   mQtDistributionDirectory(qtDistributionDirectory)
{
  assert(mQtDistributionDirectory.get() != nullptr);
}

bool SharedLibraryFinderCommon::isValidSpecificSharedLibrary(const QFileInfo & libraryFile) const
{
  assert( !libraryFile.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
  assert( libraryFile.isAbsolute() );
  assert(mQtDistributionDirectory.get() != nullptr);

  if( ( !mQtDistributionDirectory->isNull() ) &&  QtSharedLibraryFile::isQtSharedLibrary(libraryFile) ){
    assert( mQtDistributionDirectory->isValidExisting() );
    if( !mQtDistributionDirectory->containsSharedLibrary(libraryFile) ){
      return false;
    }
  }

  return true;
}

void SharedLibraryFinderCommon::performLibrarySpecificAction(const QFileInfo & library)
{
  assert(mQtDistributionDirectory.get() != nullptr);

  if( mQtDistributionDirectory->isNull() ){
    if( QtSharedLibraryFile::isQtSharedLibrary( library ) ){
      mQtDistributionDirectory->setupFromQtSharedLibrary( library, operatingSystem() );
      if( !mQtDistributionDirectory->isValidExisting() ){
        const QString msg = tr("found a Qt distribution located at '%1' , but seems not to be valid")
                            .arg( mQtDistributionDirectory->rootAbsolutePath() );
        throw FindDependencyError(msg);
      }
    }
  }
}

}} // namespace Mdt{ namespace DeployUtils{
