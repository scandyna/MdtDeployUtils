// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_LIST_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_LIST_H

#include "BinaryDependenciesResult.h"
#include "BinaryDependenciesResultLibrary.h"
#include "OperatingSystem.h"
#include "mdt_deployutilscore_export.h"
#include <QString>

#include <QStringList>

#include <optional>
#include <vector>

namespace Mdt{ namespace DeployUtils{

  /*! \brief List of BinaryDependenciesResult
   */
  class MDT_DEPLOYUTILSCORE_EXPORT BinaryDependenciesResultList
  {
    using ResultContainer = std::vector<BinaryDependenciesResult>;

   public:

    /*! \brief STL const iterator
     */
    using const_iterator = ResultContainer::const_iterator;

//     using value_type = ResultContainer::value_type;
    using const_reference = ResultContainer::const_reference;

    BinaryDependenciesResultList() = delete;

    /*! \brief Construct a empty list of results for given os
     *
     * \pre \a os must be valid
     */
    explicit
    BinaryDependenciesResultList(OperatingSystem os);

    // Copy disabled
    BinaryDependenciesResultList(const BinaryDependenciesResultList & other) = delete;
    BinaryDependenciesResultList & operator=(const BinaryDependenciesResultList & other) = delete;

    /*! \brief Move construct a list of results from \a other
     */
    BinaryDependenciesResultList(BinaryDependenciesResultList && other) = default;

    /*! \brief Move assign \a other to this list of results
     */
    BinaryDependenciesResultList & operator=(BinaryDependenciesResultList && other) noexcept = default;

    /*! \brief Get the OS of this result
     */
    OperatingSystem operatingSystem() const noexcept
    {
      return mOs;
    }

    /*! \brief Add a result to this list
     */
    void addResult(const BinaryDependenciesResult & result) noexcept;

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const noexcept
    {
      return mList.empty();
    }

    /*! \brief Check if all results in this list are solved
     *
     * \note A empty list is also considered as solved
     */
    bool isSolved() const noexcept
    {
      return mIsSolved;
    }

    /*! \brief Get count of results in this list
     */
    size_t resultCount() const noexcept
    {
      return mList.size();
    }

    /*! \brief Find a result for given target name
     *
     * \pre \a name must not be empty
     */
    std::optional<BinaryDependenciesResult>
    findResultForTargetName(const QString & name) const noexcept;

    /*! \brief Get an iterator to the beginning of the list of results
     */
    const_iterator cbegin() const noexcept
    {
      return mList.cbegin();
    }

    /*! \brief Get an iterator to the beginning of the list of results
     */
    const_iterator begin() const noexcept
    {
      return cbegin();
    }

    /*! \brief Get an iterator to the end of the list results
     */
    const_iterator cend() const noexcept
    {
      return mList.cend();
    }

    /*! \brief Get an iterator to the end of the list results
     */
    const_iterator end() const noexcept
    {
      return cend();
    }

   private:

    OperatingSystem mOs;
    bool mIsSolved = true;
    ResultContainer mList;
  };

  /*! \brief Get a list of absolute file paths for each library in given list of results
   *
   * Each path is unique in the returned list.
   *
   * \pre each result in \a resultList must be solved
   * (i.e. every library has its absolute file path set).
   */
  [[deprecated]]
  MDT_DEPLOYUTILSCORE_EXPORT
  QStringList getLibrariesAbsoluteFilePathList(const BinaryDependenciesResultList & resultList);

  /*! \internal Get a list of libraries to redistribute from given list of results
   *
   * Each library is unique in the returned list.
   *
   * \pre each result in \a resultList must be solved
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  std::vector<BinaryDependenciesResultLibrary>
  getLibrariesToRedistribute(const BinaryDependenciesResultList & resultList) noexcept;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_RESULT_LIST_H
