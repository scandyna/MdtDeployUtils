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
#ifndef MDT_DEPLOY_UTILS_QT_MODULE_LIST_H
#define MDT_DEPLOY_UTILS_QT_MODULE_LIST_H

#include "QtModule.h"
#include "QtSharedLibraryFile.h"
#include "mdt_deployutilscore_export.h"
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief A list of Qt modules
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtModuleList
  {
    using Container = std::vector<QtModule>;

   public:

    /*! \brief STL value type
     */
    using value_type = Container::value_type;

    /*! \brief STL size type
     */
    using size_type = Container::size_type;

    /*! \brief STL const iterator
     */
    using const_iterator = Container::const_iterator;

    /*! \brief Construct a empty list
     */
    QtModuleList() noexcept = default;

    /*! \brief Copy construct a list from \a other
     */
    QtModuleList(const QtModuleList & other) = default;

    /*! \brief Copy assign \a other to this list
     */
    QtModuleList & operator=(const QtModuleList & other) = default;

    /*! \brief Move construct a list from \a other
     */
    QtModuleList(QtModuleList && other) noexcept = default;

    /*! \brief Move assign \a other to this list
     */
    QtModuleList & operator=(QtModuleList && other) noexcept = default;

    /*! \brief Add a module tos this list
     *
     * if \a module allready exists, it will not be added again
     */
    void addModule(QtModule module) noexcept;

    /*! \brief Check if given module exists in this list
     */
    bool contains(QtModule module) const noexcept;

    /*! \brief Get the module at \a index
     *
     * \pre \a index must be in range ( \a index < count() )
     */
    QtModule at(size_type index) const noexcept
    {
      assert( index < count() );

      return mList[index];
    }

    /*! \brief Get the count of modules in this list
     */
    size_type count() const noexcept
    {
      return mList.size();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const noexcept
    {
      return mList.empty();
    }

    /*! \brief STL iterator to the first element
     */
    const_iterator cbegin() const noexcept
    {
      return mList.cbegin();
    }

    /*! \brief STL iterator past the last element
     */
    const_iterator cend() const noexcept
    {
      return mList.cend();
    }

    /*! \brief STL iterator to the first element
     */
    const_iterator begin() const noexcept
    {
      return cbegin();
    }

    /*! \brief STL iterator past the last element
     */
    const_iterator end() const noexcept
    {
      return cend();
    }

    /*! \brief Get a list of Qt modules from given list of Qt shared libraries
     */
    static
    QtModuleList fromQtSharedLibraries(const QtSharedLibraryFileList & qtLibraries);

   private:

    std::vector<QtModule> mList;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_MODULE_LIST_H
