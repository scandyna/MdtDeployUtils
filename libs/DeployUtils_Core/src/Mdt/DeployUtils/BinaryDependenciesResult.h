// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_H

#include "BinaryDependenciesResultLibrary.h"
#include "BinaryDependenciesFile.h"
#include "mdt_deployutilscore_export.h"
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <vector>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Collection of found binary dependencies
   */
  class MDT_DEPLOYUTILSCORE_EXPORT BinaryDependenciesResult
  {
    using LibraryContainer = std::vector<BinaryDependenciesResultLibrary>;

   public:

    /*! \brief STL const iterator
     */
    using const_iterator = LibraryContainer::const_iterator;

    BinaryDependenciesResult() = delete;

    /*! \brief Construct a result for given target
     *
     * \sa target()
     * \pre \a fileInfo must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     */
    BinaryDependenciesResult(const QFileInfo & target);

    /*! \brief Copy construct a result from \a other
     */
    BinaryDependenciesResult(const BinaryDependenciesResult & other) = default;

    /*! \brief Copy assign \a other to this result
     */
    BinaryDependenciesResult & operator=(const BinaryDependenciesResult & other) = default;

    /*! \brief Move construct a result from \a other
     */
    BinaryDependenciesResult(BinaryDependenciesResult && other) = default;

    /*! \brief Move assign \a other to this result
     */
    BinaryDependenciesResult & operator=(BinaryDependenciesResult && other) noexcept = default;

    /*! \brief Get the target
     *
     * The target is the binary
     * (the executable or shared library)
     * for which the dependencies were found.
     */
    const QFileInfo & target() const noexcept
    {
      return mTarget;
    }

    /*! \brief Check if this result is solved
     *
     * This result is solved if each library (dependency) it contains
     * is found.
     *
     * A result without any library (dependency)
     * is also solved.
     */
    bool isSolved() const noexcept
    {
      return mIsSolved;
    }

    /*! \brief Get the count of libraries this result contains
     */
    size_t libraryCount() const noexcept
    {
      return mEntries.size();
    }

    /*! \brief Check if this result contains a library by name
     *
     * \pre \a name must not be empty
     */
    bool containsLibraryName(const QString & name) const noexcept;

    /*! \brief Add given library to this result
     *
     * Given \a library can be found or not.
     *
     * If a library with the same name as given \a library already exists in this result,
     * given \a library will not be added.
     *
     * \pre \a library must not be null
     */
    void addLibrary(const BinaryDependenciesFile & library) noexcept;

    /*! \brief Get an iterator to the beginning of the list of libraries this result contains
     */
    const_iterator cbegin() const noexcept
    {
      return mEntries.cbegin();
    }

    /*! \brief Get an iterator to the beginning of the list of libraries this result contains
     */
    const_iterator begin() const noexcept
    {
      return cbegin();
    }

    /*! \brief Get an iterator to the end of the list of libraries this result contains
     */
    const_iterator cend() const noexcept
    {
      return mEntries.cend();
    }

    /*! \brief Get an iterator to the end of the list of libraries this result contains
     */
    const_iterator end() const noexcept
    {
      return cend();
    }

   private:

    bool mIsSolved = false;
    QFileInfo mTarget;
    LibraryContainer mEntries;
  };

  /*! \brief A list of BinaryDependenciesResult
   */
  using BinaryDependenciesResultList = std::vector<BinaryDependenciesResult>;

  /*! \brief Get a list of absolute file paths for each library in given list of results
   *
   * Each path is unique in the returned list.
   *
   * \pre each result in \a resultList must be solved
   * (i.e. every library has its absolute file path set).
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QStringList getLibrariesAbsoluteFilePathList(const BinaryDependenciesResultList & resultList);

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_H
