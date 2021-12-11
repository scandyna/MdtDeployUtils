/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_RPATH_H
#define MDT_DEPLOY_UTILS_RPATH_H

#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QStringList>
#include <QChar>
#include <QLatin1Char>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a single path in a RPath
   */
  class MDT_DEPLOYUTILSCORE_EXPORT RPathEntry
  {
   public:

    RPathEntry() = delete;

    /*! \brief Construct a RPath entry from \a path
     *
     * \pre \a path must not be empty
     */
    RPathEntry(const QString & path) noexcept
     : mPath( path.trimmed() )
    {
      assert( !mPath.isEmpty() );
    }

    /*! \brief Copy construct a path from \a other
     */
    RPathEntry(const RPathEntry & other) = default;

    /*! \brief Copy assign \a other to this path
     */
    RPathEntry & operator=(const RPathEntry & other) = default;

    /*! \brief Move construct a path from \a other
     */
    RPathEntry(RPathEntry && other) noexcept = default;

    /*! \brief Move assign \a other to this path
     */
    RPathEntry & operator=(RPathEntry && other) noexcept = default;

    /*! \brief Check if the path of this entry is relative
     *
     * If true, the path will be relative to the location
     * of the binary file.
     * The path in the file will start with $ORIGIN
     * or \@loader_path , depending on the platform.
     */
    bool isRelative() const noexcept
    {
      assert( !mPath.isEmpty() );

      return !mPath.startsWith( QLatin1Char('/') );
    }

    /*! \brief Get the path of this entry
     */
    const QString & path() const noexcept
    {
      return mPath;
    }

   private:

    QString mPath;
  };

  /*! \brief Represents a rpath (run-time search path)
   *
   * A rapth is a list of paths encoded in a executable file or a shared library.
   *
   * \sa RPathElf
   * \sa RPathMachO
   * \sa https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling
   * \sa https://man7.org/linux/man-pages/man8/ld.so.8.html
   * \sa https://developer.apple.com/library/archive/documentation/DeveloperTools/Conceptual/DynamicLibraries/100-Articles/RunpathDependentLibraries.html
   */
  class MDT_DEPLOYUTILSCORE_EXPORT RPath
  {
   public:

    /*! \brief STL const iterator
     */
    using const_iterator = std::vector<RPathEntry>::const_iterator;

    /*! \brief Contruct a empty rpath
     */
    explicit RPath() noexcept = default;

    /*! \brief Copy construct a rpath from \a other
     */
    RPath(const RPath & other) = default;

    /*! \brief Copy assign \a other to this rpath
     */
    RPath & operator=(const RPath & other) = default;

    /*! \brief Move construct a rpath from \a other
     */
    RPath(RPath && other) noexcept = default;

    /*! \brief Move assign \a other to this rpath
     */
    RPath & operator=(RPath && other) noexcept = default;

    /*! \brief Add a entry to the end of this rpath
     */
    void appendEntry(const RPathEntry & entry) noexcept
    {
      mRPath.push_back(entry);
    }

    /*! \brief Add a path to the end of this rpath
     *
     * \pre \a path must not be empty
     */
    void appendPath(const QString & path) noexcept
    {
      assert( !path.trimmed().isEmpty() );

      mRPath.emplace_back(path);
    }

    /*! \brief Get the count of entries in this rpath
     */
    size_t entriesCount() const noexcept
    {
      return mRPath.size();
    }

    /*! \brief Check if this rpath is empty
     */
    bool isEmpty() const noexcept
    {
      return mRPath.empty();
    }

    /*! \brief Get the entry at \a index
     *
     * \pre \a index must be in valid range ( \a index < entriesCount() )
     */
    const RPathEntry & entryAt(size_t index) const noexcept
    {
      assert( index < entriesCount() );

      return mRPath[index];
    }

    /*! \brief Clear this rpath
     */
    void clear() noexcept
    {
      mRPath.clear();
    }

    /*! \brief Get a iterator to the beginning of this rpath
     */
    const_iterator cbegin() const noexcept
    {
      return mRPath.cbegin();
    }

    /*! \brief Get a iterator past the end of this rpath
     */
    const_iterator cend() const noexcept
    {
      return mRPath.cend();
    }

    /*! \brief Get a iterator to the beginning of this rpath
     */
    const_iterator begin() const noexcept
    {
      return cbegin();
    }

    /*! \brief Get a iterator past the end of this rpath
     */
    const_iterator end() const noexcept
    {
      return cend();
    }

   private:

    std::vector<RPathEntry> mRPath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_RPATH_H
