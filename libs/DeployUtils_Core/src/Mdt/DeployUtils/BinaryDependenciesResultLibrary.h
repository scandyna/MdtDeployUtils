// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_LIBRARY_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_LIBRARY_H

#include "mdt_deployutilscore_export.h"
#include <QFileInfo>
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \internal
   */
  class MDT_DEPLOYUTILSCORE_EXPORT BinaryDependenciesResultLibrary
  {
   public:

    BinaryDependenciesResultLibrary() = delete;

    /*! \brief Copy construct a result library from \a other
     */
    BinaryDependenciesResultLibrary(const BinaryDependenciesResultLibrary & other) = default;

    /*! \brief Copy assign \a other to this result library
     */
    BinaryDependenciesResultLibrary & operator=(const BinaryDependenciesResultLibrary & other) = default;

    /*! \brief Move construct a result library from \a other
     */
    BinaryDependenciesResultLibrary(BinaryDependenciesResultLibrary && other) = default;

    /*! \brief Move assign \a other to this result library
     */
    BinaryDependenciesResultLibrary & operator=(BinaryDependenciesResultLibrary && other) noexcept = default;

    /*! \brief Check if this library is found
     */
    bool isFound() const noexcept;

    /*! \brief Get the library name
     *
     * Returns the file name of the library,
     * with its extension.
     */
    QString libraryName() const noexcept
    {
      return mFile.fileName();
    }

    /*! \brief Construct a result library from \a fileInfo
     *
     * \pre \a fileInfo must have at least a file name
     */
    static
    BinaryDependenciesResultLibrary fromQFileInfo(const QFileInfo & fileInfo) noexcept;

   private:

    BinaryDependenciesResultLibrary(const QFileInfo & fileInfo)
     : mFile(fileInfo)
    {
    }

    QFileInfo mFile;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_LIBRARY_H
