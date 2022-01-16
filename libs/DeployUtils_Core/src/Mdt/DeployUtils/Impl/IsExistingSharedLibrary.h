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
#ifndef MDT_DEPLOY_UTILS_IMPL_IS_EXISTING_SHARED_LIBRARY_H
#define MDT_DEPLOY_UTILS_IMPL_IS_EXISTING_SHARED_LIBRARY_H

#include "AbstractIsExistingSharedLibrary.h"
#include "Mdt/DeployUtils/ExecutableFileReader.h"
#include "Mdt/DeployUtils/Platform.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{

  /*! \internal
   */
  class IsExistingSharedLibrary : public AbstractIsExistingSharedLibrary
  {
   public:

    /*! \brief Constructor
     */
    explicit IsExistingSharedLibrary(ExecutableFileReader & reader, const Platform & platform) noexcept
     : mReader(reader),
       mPlatform(platform)
    {
      assert( !mReader.isOpen() );
    }

   private:

    bool doIsExistingSharedLibrary(const QFileInfo & libraryFile) const override
    {
      assert( !mReader.isOpen() );

      if( !libraryFile.exists() ){
        return false;
      }

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

    ExecutableFileReader & mReader;
    const Platform mPlatform;
  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_IS_EXISTING_SHARED_LIBRARY_H
