/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2023 Philippe Steinmann.
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
#include "FileInfoUtils.h"
#include <cassert>


namespace Mdt{ namespace DeployUtils{

SharedLibraryFinderCommon::SharedLibraryFinderCommon(const std::shared_ptr<const AbstractIsExistingValidSharedLibrary> & isExistingValidShLibOp,
                                                     std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory,
                                                     QObject *parent) noexcept
 : AbstractSharedLibraryFinder(isExistingValidShLibOp, parent),
   mQtDistributionDirectory(qtDistributionDirectory)
{
  assert(mQtDistributionDirectory.get() != nullptr);
}

bool SharedLibraryFinderCommon::validateSpecificSharedLibrary(const QFileInfo & libraryFile)
{
  assert( fileInfoIsAbsolutePath(libraryFile) );
  assert(mQtDistributionDirectory.get() != nullptr);

  if( QtSharedLibraryFile::isQtSharedLibrary(libraryFile) ){
    emit debugMessage(
      tr("  checking if %1 comes from a Qt distribution")
      .arg( libraryFile.absoluteFilePath() )
    );
    if( mQtDistributionDirectory->isNull() ){
      mQtDistributionDirectory->setupFromQtSharedLibrary( libraryFile, operatingSystem() );
      if( mQtDistributionDirectory->isValidExisting() ){
        emit verboseMessage(
          tr(" found Qt distribution: %1")
          .arg( mQtDistributionDirectory->rootAbsolutePath() )
        );
      }else{
        emit debugMessage(
          tr("  %1 is not a Qt distribution")
          .arg( mQtDistributionDirectory->rootAbsolutePath() )
        );
        mQtDistributionDirectory->clear();
        assert( mQtDistributionDirectory->isNull() );
        return false;
      }
    }
    assert( mQtDistributionDirectory->isValidExisting() );
    if( !mQtDistributionDirectory->containsSharedLibrary(libraryFile) ){
      return false;
    }
  }

  return true;
}

}} // namespace Mdt{ namespace DeployUtils{
