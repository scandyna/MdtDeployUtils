// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_DISCOVERED_DEPENDENCIES_LIST_H
#define MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_DISCOVERED_DEPENDENCIES_LIST_H

#include "GraphFile.h"
#include "FileComparison.h"

// #include "Mdt/DeployUtils/FileInfoUtils.h"

#include "Mdt/DeployUtils/OperatingSystem.h"
#include <QString>
#include <QStringList>
#include <vector>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

  /*! \internal Helper class to store discovered dependencies while building a binary dependencies graph
   */
  class DiscoveredDependencies
  {
   public:

    /*! \brief Constructor
     */
    explicit
    DiscoveredDependencies(const GraphFile & file, const QStringList & dependencies) noexcept
     : mDependentFileName( file.fileName() ),
       mDependenciesFileNames(dependencies)
    {
      assert( file.hasFileName() );
      assert( !dependencies.isEmpty() );
    }

    /*! \brief Get the dependent file name
     */
    const QString & dependentFileName() const noexcept
    {
      return mDependentFileName;
    }

    /*! \brief Get the list of dependencies of this file
     */
    const QStringList & dependenciesFileNames() const noexcept
    {
      return mDependenciesFileNames;
    }

   private:

    QString mDependentFileName;
    QStringList mDependenciesFileNames;
  };

  /*! \internal Helper class to store a list discovered dependencies while building a binary dependencies graph
   */
  class DiscoveredDependenciesList
  {
   public:

    /*! \brief STL iterator
     */
    using const_iterator = std::vector<DiscoveredDependencies>::const_iterator;

    /*! \brief Construct a list
     *
     * \pre \a os must be valid
     */
    explicit
    DiscoveredDependenciesList(OperatingSystem os) noexcept
     : mOs(os)
    {
      assert( mOs != OperatingSystem::Unknown );
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const noexcept
    {
      return mList.empty();
    }

    /*! \brief Get the count of elements in this list
     */
    size_t size() const noexcept
    {
      return mList.size();
    }

    /*! \brief Get the element at \a index
     *
     * \pre \a index must be in valid range
     */
    const DiscoveredDependencies & at(size_t index) const noexcept
    {
      assert( index < size() );

      return mList[index];
    }

    /*! \brief Get a iterator to the begin of this list
     */
    const_iterator cbegin() const noexcept
    {
      return mList.cbegin();
    }

    /*! \brief Get a iterator past the end of this list
     */
    const_iterator cend() const noexcept
    {
      return mList.cend();
    }

    /*! \brief Get a iterator to the begin of this list
     */
    const_iterator begin() const noexcept
    {
      return cbegin();
    }

    /*! \brief Get a iterator past the end of this list
     */
    const_iterator end() const noexcept
    {
      return cend();
    }

    /*! \brief Check if this list contains given dependent file
     *
     * \pre \a file must have its file name
     */
    bool containsDependentFile(const GraphFile & file) const noexcept
    {
      assert( file.hasFileName() );

      const QString fileName = file.fileName();
      const OperatingSystem os = mOs;
      const auto pred = [&fileName, os](const DiscoveredDependencies & dependencies){
        return fileNamesAreEqual(dependencies.dependentFileName(), fileName, os);
      };

      const auto it = std::find_if(cbegin(), cend(), pred);

      return it != cend();
    }

    /*! \brief Set the direct dependencies for given file
     *
     * Design choice:
     * Here we pass a file as the one depending on given dependencies.
     * This requires later to find the vertex associated with this file
     * by searching by file name.
     * But notice that this search has to be done anyway for given dependencies.
     * They are just file names extracted from a executable (wich can be a shared library).
     * So, it should be check if this overheat is significant before brining a vertex here !
     *
     * \note If given file already exists in this list,
     * nothing is done.
     * It is assumed that the same file will have the same dependencies.
     *
     * \pre \a file must have its file name
     * \pre each file name in \a dependencies must not be empty
     * \note It is allowed that \a dependencies is a empty list,
     * in which case nothing is added
     */
    void setDirectDependenciesFileNames(const GraphFile & file, const QStringList & dependencies) noexcept
    {
      assert( file.hasFileName() );

      if( containsDependentFile(file) ){
        return;
      }
      if( dependencies.isEmpty() ){
        return;
      }

      mList.emplace_back(file, dependencies);
    }

    /*! \brief clear this list
     */
    void clear() noexcept
    {
      mList.clear();
    }

   private:

    OperatingSystem mOs;
    std::vector<DiscoveredDependencies> mList;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_DISCOVERED_DEPENDENCIES_LIST_H
