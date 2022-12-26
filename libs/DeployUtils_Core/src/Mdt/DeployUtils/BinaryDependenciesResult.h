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
#include "OperatingSystem.h"
#include "RPath.h"
#include "mdt_deployutilscore_export.h"
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <vector>
#include <optional>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Collection of found binary dependencies
   *
   * \todo
   * One result should concern multiple targets. < NO
   *
   * \todo update doc
   *
   * \note
   * The solver can accept multiple targets at the same time.
   * It then try to avoid solving the same binary multiple times.
   * The litle drawback of this solution is that it will not
   * give the dependencies for each target in a reliable way.
   * As example, \a app depends on \a libQt5Core.so .
   * Both also depend on \a libstdc++.so :
   * \code
   * BinaryDependencies solver;
   * QFileInfoList binaryFilePathList{"/opt/bin/app","/opt/qt/lib/libQt5Core.so"}
   *
   * auto result = solver.findDependencies(binaryFilePathList, ...);
   *
   * // First target, app, will be solved:
   * // - app will be solved and depends on libQt5Core.so and libstdc++.so
   * // - libQt5Core.so will be solved (transitive solving) and depends on libstdc++.so
   * // - result: app depends on libQt5Core.so and libstdc++.so (flat list of all transitive dependencies)
   * // - app and libQt5Core.so will be marked as solved
   *
   * // Second target, libQt5Core.so, will be solved:
   * // - libQt5Core.so is already solved
   * // - result: empty dependency list (which is wrong)
   * \endcode
   *
   * It should be possible to change the implementation
   * so that it reports reliable results for each given target,
   * but this is not a priority for the moment
   * (is it worth it ?).
   *
   *
   * \todo What about checking if a plugin is solved or not ?
   * Maybe a error message for given target ?
   * Associate a error with a particular target..
   */
  class MDT_DEPLOYUTILSCORE_EXPORT BinaryDependenciesResult
  {
    using LibraryContainer = std::vector<BinaryDependenciesResultLibrary>;

   public:

    /*! \brief STL const iterator
     */
    using const_iterator = LibraryContainer::const_iterator;

    using const_reference = LibraryContainer::const_reference;

    BinaryDependenciesResult() = delete;

    /*! \brief Construct a result for given target
     *
     * \sa target()
     * \pre \a fileInfo must have its absolute file path set
     * \sa fileInfoIsAbsolutePath()
     * \pre \a os must be valid
     */
    explicit
    BinaryDependenciesResult(const QFileInfo & target, OperatingSystem os);

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

    /*! \brief Get the OS of this result
     */
    OperatingSystem operatingSystem() const noexcept
    {
      return mOs;
    }

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

    /*! \brief Get the library at \a index
     *
     * \pre \a index must be in valid range
     */
    const BinaryDependenciesResultLibrary & libraryAt(size_t index) const noexcept
    {
      assert( index < libraryCount() );

      return mEntries[index];
    }

    /*! \brief Check if this result contains a library by name
     *
     * \pre \a name must not be empty
     */
    bool containsLibraryName(const QString & name) const noexcept;

    /*! \brief Find a library for given name
     *
     * \pre \a name must not be empty
     */
    std::optional<BinaryDependenciesResultLibrary>
    findLibraryByName(const QString & name) const noexcept;

    /*! \brief Add given found library to this result
     *
     * If a library with the same name as given \a library already exists in this result,
     * given \a library will not be added.
     *
     * \pre \a library must be a absolute path
     */
    void addFoundLibrary(const QFileInfo & library, const RPath & rpath) noexcept;

    /*! \brief Add given not found library to this result
     *
     * If a library with the same name as given \a library already exists in this result,
     * given \a library will not be added.
     *
     * \pre \a library must have its file name
     */
    void addNotFoundLibrary(const QFileInfo & library) noexcept;

    /*! \brief Add given library to not redistribute to this result
     *
     * If a library with the same name as given \a library already exists in this result,
     * given \a library will not be added.
     *
     * \pre \a library must have its file name
     */
    void addLibraryToNotRedistribute(const QFileInfo & library) noexcept;

    /*! \brief Add given library to this result
     *
     * Given \a library can be found or not.
     *
     * If a library with the same name as given \a library already exists in this result,
     * given \a library will not be added.
     *
     * \pre \a library must not be null
     */
    [[deprecated]]
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

    const_iterator findIteratorByLibraryName(const QString & name) const noexcept;

    bool mIsSolved = false;
    OperatingSystem mOs;
    QFileInfo mTarget;
    LibraryContainer mEntries;
  };

  /*! \internal Get a list of libraries to redistribute from given result
   *
   * Each library is unique in the returned list.
   *
   * \pre \a result must be solved
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  std::vector<BinaryDependenciesResultLibrary>
  getLibrariesToRedistribute(const BinaryDependenciesResult & result) noexcept;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_H
