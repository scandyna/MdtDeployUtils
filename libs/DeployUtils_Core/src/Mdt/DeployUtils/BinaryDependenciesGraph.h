// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_GRAPH_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_GRAPH_H

#include "BinaryDependenciesFile.h"
#include "BinaryDependenciesResult.h"
#include "mdt_deployutilscore_export.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief Binary dependencies graph
   *
   *
   * ## Solving example - This is IMPL detail
   *
   *
   * ## Solving multiple targets at the same time
   *
   * The targets to solve are \a app and \a libA :
   * \code
   * const auto targets = ...
   * // targets contains app and libA
   *
   * graph.addFiles(targets);
   * \endcode
   *
   * The graph contains \a app and \a libA,
   * which have their full path:
   * \code
   * (app) (libA)
   * \endcode
   *
   * \code
   * auto fileList = graph.findUnprocessedFiles();
   * // fileList contains app and libA
   *
   * for(auto & file : fileList){
   *   setDirectDependenciesAndRPath(file);
   *   const auto directDependencies = findDirectDepenencies(file);
   *   graph.addDirectDependencies(file, directDependencies);
   * }
   * \endcode
   *
   * If we unroll the loop:
   * \code
   * setDirectDependenciesAndRPath(app);
   * const auto directDependencies = findDirectDepenencies(app);
   * // directDependencies contains libA and Qt5Core
   * graph.addDirectDependencies(app, directDependencies);
   * \endcode
   *
   * This is the graph after \a app have been processed:
   * \code
   *      (app)P
   *      /   \
   * (libA)  (Qt5Core)
   * \endcode
   *
   * Then \a libA is processed:
   * \code
   * setDirectDependenciesAndRPath(libA);
   * const auto directDependencies = findDirectDepenencies(libA);
   * // directDependencies contains libB and Qt5Core
   * graph.addDirectDependencies(libA, directDependencies);
   * \endcode
   *
   * This is the graph after \a libA have been processed:
   * \code
   *      (app)P
   *      /    \
   *  (libA)P   |
   *     |  \  /
   * (libB) (Qt5Core)
   * \endcode
   *
   * While the starting point is slightly different,
   * the ongoing process is the same as above.
   *
   * \todo circular dependencies
   *
   * ## Getting results
   *
   * Given a completely processed graph:
   * \code
   *      (app)P
   *      /    \
   *   (libA)P  |
   *     |  \  /
   * (libB)P (Qt5Core)P
   * \endcode
   *
   * We can get all transitive dependencies for \a app :
   * \code
   * const BinaryDependenciesResult result = graph.getResult(app);
   * \endcode
   *
   * \a result will look like:
   * \code
   * app
   *  |->libA
   *  |->libB
   *  |->Qt5Core
   * \endcode
   *
   * \sa getResult()
   *
   * ## Design choices
   *
   * This graph does not read files to solve dependencies,
   * it is only a graph to represent them.
   * This avoids to much coupling and make it more easy to test.
   */
  class MDT_DEPLOYUTILSCORE_EXPORT BinaryDependenciesGraph
  {
   public:

    /// \todo For impl, see https://github.com/mmccoo/nerd_mmccoo/tree/master/boost_properties

    /*! \brief Add given file to this graph
     *
     * If given file already exists in this graph,
     * nothing is done.
     *
     * The comparison is done using filesAreEqualByName().
     *
     * Given file can have its full path (it is found),
     * or only its file name (it is NOT found).
     *
     * \pre Given file must not be null
     * \pre Given file must not have any direct dependency
     */
    void addFile(const BinaryDependenciesFile & file) noexcept;

    /*! \brief Add a list of direct dependencies
     *
     * Will add a dependency from \a file to each file in \a dependencies .
     *
     * Each dependency will also be added if it does not already exist.
     * \sa addFile()
     *
     * \note It is not checked if given dependencies are coherent
     * to the direct dependencies file names in \a file .
     * It is the caller responsability to keep this clean.
     *
     * \pre \a file must already exist in this graph
     * \pre each dependency in \a dependencies must meet the preconditions of addFile()
     */
    void addDirectDependencies(const BinaryDependenciesFile & file, const BinaryDependenciesFileList & dependencies) noexcept;

    /*! \brief Get a list of the unprocessed files in this graph
     *
     * Returns a copy of the files in this graph.
     * This allows to mutate this graph while processing returned list.
     */
    BinaryDependenciesFileList findUnprocessedFiles() const noexcept;

    /*! \brief Get a result for given file
     *
     * \pre This graph must be entirely processed
     */
    BinaryDependenciesResult getResult(const BinaryDependenciesFile & file) const noexcept;

    /*! \brief Check if given files are equal by file name
     */
    static
    bool filesAreEqualByName() noexcept;

   private:

    /*! \brief Check if this graph is entirely processed
     *
     * \note This method will traverse the graph
     * \sa findUnprocessedFiles()
     */
    bool checkIsEntirelyProcessed() const noexcept;

  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_GRAPH_H
