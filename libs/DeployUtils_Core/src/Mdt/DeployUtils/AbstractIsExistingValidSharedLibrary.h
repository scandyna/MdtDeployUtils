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
#ifndef MDT_DEPLOY_UTILS_ABSTRACT_IS_EXISTING_VALID_SHARED_LIBRARY_H
#define MDT_DEPLOY_UTILS_ABSTRACT_IS_EXISTING_VALID_SHARED_LIBRARY_H

#include "mdt_deployutilscore_export.h"
#include <QFileInfo>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \internal
   *
   * We have to export symbols here, despite it is header only.
   * See https://stackoverflow.com/questions/57294792/c-ubsan-produces-false-positives-with-derived-objects
   */
  class MDT_DEPLOYUTILSCORE_EXPORT AbstractIsExistingValidSharedLibrary
  {
   public:

    virtual ~AbstractIsExistingValidSharedLibrary() noexcept = default;

    /*! \brief Check if \a libraryFile is a existing and valid shared library
     *
     * \pre \a libraryFile must be a absolute file path
     */
    bool isExistingValidSharedLibrary(const QFileInfo & libraryFile) const
    {
      assert( !libraryFile.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
      assert( libraryFile.isAbsolute() );

      return doIsExistingValidSharedLibrary(libraryFile);
    }

   private:

    virtual bool doIsExistingValidSharedLibrary(const QFileInfo & libraryFile) const = 0;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ABSTRACT_IS_EXISTING_VALID_SHARED_LIBRARY_H
