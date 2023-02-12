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
#ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_COMMON_H
#define MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_COMMON_H

#include "AbstractSharedLibraryFinder.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  class QtDistributionDirectory;

  /*! \brief Common base to implement a shared library finder
   */
  class MDT_DEPLOYUTILSCORE_EXPORT SharedLibraryFinderCommon : public AbstractSharedLibraryFinder
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * If given \a qtDistributionDirectory is not already initialzed,
     * it will be the first time a Qt library have been found.
     *
     * \pre \a reader must be a valid pointer
     * \pre \a qtDistributionDirectory must be a valid pointer
     */
    explicit SharedLibraryFinderCommon(const std::shared_ptr<const AbstractIsExistingValidSharedLibrary> & isExistingValidShLibOp,
                                       std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory,
                                       QObject *parent = nullptr) noexcept;

   private:

    /*! \brief Check if given library is valid reagarding library specific criteria
     *
     * \pre \a libraryFile must be a absolute file path
     */
    bool validateSpecificSharedLibrary(const QFileInfo & libraryFile) override;

    std::shared_ptr<QtDistributionDirectory> mQtDistributionDirectory;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_COMMON_H
