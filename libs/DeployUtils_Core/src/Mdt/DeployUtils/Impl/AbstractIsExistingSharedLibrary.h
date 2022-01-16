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
#ifndef MDT_DEPLOY_UTILS_IMPL_ABSTRACT_IS_EXISTING_SHARED_LIBRARY_H
#define MDT_DEPLOY_UTILS_IMPL_ABSTRACT_IS_EXISTING_SHARED_LIBRARY_H

#include "mdt_deployutilscore_export.h"
#include <QFileInfo>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{

  /*! \internal
   *
   * We have to export symbols here, despite it is header only.
   * See https://stackoverflow.com/questions/57294792/c-ubsan-produces-false-positives-with-derived-objects
   */
  class MDT_DEPLOYUTILSCORE_EXPORT AbstractIsExistingSharedLibrary
  {
   public:

    virtual ~AbstractIsExistingSharedLibrary() noexcept = default;

    /*! \brief Check if \a libraryFile is a existing shared library
     *
     * \pre \a libraryFile must be a absolute file path
     */
    bool isExistingSharedLibrary(const QFileInfo & libraryFile) const
    {
      assert( !libraryFile.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
      assert( libraryFile.isAbsolute() );

      return doIsExistingSharedLibrary(libraryFile);
    }

   private:

    virtual bool doIsExistingSharedLibrary(const QFileInfo & libraryFile) const = 0;
  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ABSTRACT_IS_EXISTING_SHARED_LIBRARY_H
