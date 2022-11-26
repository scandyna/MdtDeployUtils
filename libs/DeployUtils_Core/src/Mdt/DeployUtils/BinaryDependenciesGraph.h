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

#include "mdt_deployutilscore_export.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief Binary dependencies graph
   *
   * Given we have to solve \a app :
   * \code
   * Target to solve: app
   *
   * app
   *  |->libA
   *  |   |->Qt5Core
   *  |   |->libB
   *  |->Qt5Core
   * \endcode
   *
   * The dependency graph will look like that:
   * \code
   *      (app)tgt
   *      /   \
   *  (libA)   |
   *     |  \  |
   * (libB) (Qt5Core)
   * \endcode
   *
   * In above graph, no full path is represented.
   * In reality, it will be required while solving,
   * because each binary must be read to get its direct dependencies.
   *
   * The result will be the given target
   * with all its transitive dependencies:
   * \code
   * app
   *  |->libA
   *  |->libB
   *  |->Qt5Core
   * \endcode
   *
   * Attributes, like "is it solved" or the full path to each dependency
   * is not represented here, but is required.
   *
   * Now we want to solve multiple targets at the same time.
   * Most of them have common dependencies,
   * so we try to avoid to solve them again and again.
   * But, we also need to know the transitive dependencies for each given target.
   * \code
   * Targets to solve: app,libA
   *
   * app
   *  |->libA
   *  |   |->Qt5Core
   *  |   |->libB
   *  |->Qt5Core
   *
   * libA
   *  |->Qt5Core
   *  |->libB
   * \endcode
   *
   * The graph is very similar to the previous one.
   * The only difference is that \a app AND \a libA
   * are marked as a given target:
   * \code
   *      (app)tgt
   *      /    \
   *  (libA)tgt |
   *     |  \  /
   * (libB) (Qt5Core)
   * \endcode
   *
   * The result will be each given target
   * with their transitive dependencies:
   * \code
   * app
   *  |->libA
   *  |->libB
   *  |->Qt5Core
   *
   * libA
   *  |->Qt5Core
   *  |->libB
   * \endcode
   *
   * Solving example OLD
   * 
   * Start with app:
   * \code
   *  auto app = BinaryDependenciesFile::fromQFileInfo("/path/to/app");
   *
   * // Read the file and set the direct dependencies file names and rpath
   * app.setDependenciesFileNames( mReader.getNeededSharedLibraries() );
   * app.setRPath( mReader.getRunPath() );
   *
   * mGraph.addNodeXY(app);
   * \endcode
   *
   * At this state, the graph looks like this:
   * \code
   *        ( app )P
   *       /   |   \
   * (libA) (libB) (Qt5Core)
   * \endcode
   *
   * app has its full path and its dependencies,
   * it is now considered as processed.
   *
   * libA, libB and Qt5Core are NOT considered as processed.
   *
   * \note we use processed, because we could have unsolved results at the end.
   *
   * 
   * \todo for the parent problem, see add_edge() !
   * 
   * \code
   * XXX = findUnprocessedBinaires();
   * // Will return libA, libB and Qt5Core
   *
   * for(library : XXX){
   *   x.setAbsoluteFilePath(  );
   * }
   * \endcode
   *
   * \code
   * auto app = BinaryDependenciesFile::fromQFileInfo("/path/to/app");
   *
   * mGraph.addNodeXY(app);
   *
   * x = findUnprocessedBinaires();
   * // x contains app
   * 
   * for(bin : x){
   *   setFullPathToBinIfRequired(bin);
   *   setDirectDependenciesAndRPath(bin);
   *   directDeps = findDirectDepenencies(bin);
   *   mGraph.addNodesXYZ(directDeps);
   * }
   * \endcode
   *
   * Solving example
   *
   * Start with app:
   * \code
   * auto app = BinaryDependenciesFile::fromQFileInfo("/path/to/app");
   *
   * mGraph.addFile(app);
   * \endcode
   *
   * The graph contains only \a app ,
   * which has its full path:
   * \code
   * (app)
   * \endcode
   *
   * Now we can get the unprocessed binaries,
   * that will return a list that contains \a app :
   * \code
   * x = findUnprocessedBinaries();
   * // x contains app
   * auto fileList = findUnprocessedFiles();
   * // fileList contains app
   *
   * for(auto & file : fileList){
   *   setDirectDependenciesAndRPath(file);
   *   auto directDependencies = findDirectDepenencies(file);
   *   mGraph.addDirectDependencies(directDependencies, file);
   * }
   * \endcode
   *
   * We have to read the file that represents \a app
   * to get its direct dependencies library names,
   * and also its rpath to help find them (on system that supports rpath).
   *
   * findDirectDepenencies() returns a list of BinaryDependenciesFile.
   * Each file in this list contains the full path to it (it is found),
   * or only the library name (it is NOT found).
   *
   * Those files are added to the graph as direct dependencies of \a app :
   * \code
   *      (app)P
   *      /   \
   * (libA)  (Qt5Core)
   * \endcode
   *
   * Now \a app is marked as processed.
   *
   * We can repeat the process:
   * \code
   * x = findUnprocessedBinaires();
   * // x contains libA, Qt5Core
   * 
   * for(bin : x){
   *   setDirectDependenciesAndRPath(bin);
   *   auto directDependencies = findDirectDepenencies(bin);
   *   mGraph.addNodesXYZ(directDependencies, bin);
   * }
   * \endcode
   *
   * After that, \a libA and \a Qt5Core are marked as processed.
   * Notice that \a libA also has a direct dependency to \a Qt5Core .
   * Also, while processing \a libA , a new dependency, \a libB , have been discovered.
   * \code
   *      (app)P
   *      /    \
   *  (libA)P   |
   *     |  \  /
   * (libB) (Qt5Core)P
   * \endcode
   *
   * Repeat the process again:
   * \code
   * x = findUnprocessedBinaires();
   * // x contains libB
   * 
   * for(bin : x){
   *   setDirectDependenciesAndRPath(bin);
   *   auto directDependencies  = findDirectDepenencies(bin);
   *   mGraph.addNodesXYZ(directDependencies, bin);
   * }
   * \endcode
   *
   * libB has no direct dependencies,
   * so no new binary is added to the graph.
   * The only change: libB is marked as processed:
   * \code
   *      (app)P
   *      /    \
   *   (libA)P  |
   *     |  \  /
   * (libB)P (Qt5Core)P
   * \endcode
   *
   * Now findUnprocessedBinaires() will return a empty list:
   * \code
   * x = findUnprocessedBinaires();
   * // x is empty
   * \endcode
   *
   * We found all transitive dependencies of \a app .
   *
   * The complete code looks like:
   * \code
   * auto app = BinaryDependenciesFile::fromQFileInfo("/path/to/app");
   *
   * mGraph.addNodeXY(app);
   *
   * do{
   *   // MUST return a copy because of iterator invalidation !
   *   x = findUnprocessedBinaires();
   *
   *   for(bin : x){
   *     setDirectDependenciesAndRPath(bin);
   *     auto directDependencies  = findDirectDepenencies(bin);
   *     mGraph.addNodesXYZ(directDependencies, bin);
   *   }
   * }while( !x.isEmpty() );
   * \endcode
   * 
   * For each unprocessed files, we have to:
   * - find its absolute path
   * - read it and set its direct dependencies and rpath
   * \todo if find absolute path fails, set some flag
   *
   * \code
   * const auto f = [](){
   * };
   *
   * mGraph.solveUnprocessedBinaries();
   * \endcode
   * 
   * OR: BFS !! or not
   * 
   * 
   * Solving example II
   *
   * \code
   * auto app = BinaryDependenciesFile::fromQFileInfo("/path/to/app");
   *
   * mGraph.addNodeXY(app);
   * \endcode
   *
   * \code
   * (app)
   * \endcode
   *
   * \code
   * const auto f = [](){
   * };
   *
   * mGraph.solveUnprocessedBinaries();
   * \endcode
   *
   * f will be called with app.
   * app already has its full path, so we have to:
   * - read it, set its direct dependencies and rpath
   * - find the full path for each of its dependency
   *
   * \code
   *        ( app )P
   *       /   |   \
   * (libA) (libB) (Qt5Core)
   * \endcode
   *
   * 
   * 
   * app has its full path, so it is found.
   * libA, libB and Qt5Core only have their file names, no path.
   *
   * findNotFoundBinaries() 
   * app has its full path set and its direct dependencies file names.
   * \todo define a state for that
   *
   * Now we need to find the absolute path
   * for libA, libB and Qt5Core.
   *
   * \note is marking as target required ?
   * While extracting a result,
   * a arbitrary target should be given
   *
   * \todo circular dependencies
   *
   * \todo this graph is a graph, it does not read files etc..
   * User must pass things ?
   * 
   * For impl, see https://github.com/mmccoo/nerd_mmccoo/tree/master/boost_properties
   */
  class MDT_DEPLOYUTILSCORE_EXPORT BinaryDependenciesGraph
  {
   public:

    /*! \brief Add given target to this graph
     *
     * If given target already exists in this graph,
     * it will be marked as target.
     *
     * Otherwise, ... 
     */
    void addTarget() noexcept;

    /*! \brief Find a nodeXY in this graph
     *
     * Returns a iterator if found,
     * otherwise ??
     * 
     * \todo find is a impl detail
     */
    
    
    /*! \brief Add given nodeXY to this graph
     *
     * Will add given nodeXY to this graph.
     * Each direct dependency will also be added
     * as a dependency of the nodeXY...
     *
     * \todo document above example by adding step by step
     * 
     * \note One "node" is unique.
     * Ex: LibA will always have the same lib name and direct dependencies !
     *
     * As example, add \a libA with its direct dependencies:
     * \code
     * libA
     *  |->Qt5Core
     *  |->libB
     * \endcode
     *
     * Each library will be added (as vertices).
     * The dependencies of libA to libB and Qt5Core
     * will also be added (edges):
     * \code
     *     (libA)
     *     /    \
     * (libB) (Qt5Core)
     * \endcode
     *
     * Now add \a app with its direct dependencies:
     * \code
     * app
     *  |->libA
     *  |->Qt5Core
     * \endcode
     *
     * \a app does not exist, so it will be added (new vertex).
     * libA already exists in the graph.
     * A dependency will be added from app to libA (new edge).
     * The same happens for Qt5Core.
     * The graph now looks like this:
     * \code
     *      (app)
     *      /   \
     *  (libA)   |
     *     |  \ /
     * (libB) (Qt5Core)
     * \endcode
     *
     * \pre given nodeXY must have its full path set ?
     * -> NO, flag..
     */
    void addNodeXY() noexcept;

   private:

    
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_GRAPH_H
