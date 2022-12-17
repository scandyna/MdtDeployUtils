// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_H
#define MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_H

#include "GraphDef.h"
#include "GraphBuildVisitor.h"
#include "FileComparison.h"
#include "DiscoveredDependenciesList.h"
#include "GraphResultVisitor.h"
#include "Mdt/DeployUtils/Platform.h"
#include "Mdt/DeployUtils/AbstractSharedLibraryFinder.h"
#include "Mdt/DeployUtils/BinaryDependenciesResult.h"
#include "Mdt/DeployUtils/BinaryDependenciesResultList.h"
#include "Mdt/DeployUtils/FileInfoUtils.h"
#include <QString>
#include <QFileInfo>
#include <QFileInfoList>
#include <boost/graph/breadth_first_search.hpp>
#include <optional>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

  /*! \internal
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
   *      (app)
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
   * is not represented here, but is required in the result.
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
   * The graph will be exactly the same as above.
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
   * # How to build the graph
   *
   * We begin with some target,
   * that is a binary (executable or shared library).
   * That target is the full path to a binary on the file system.
   * \code
   * (app)
   * \endcode
   *
   * We have to read it to extract its direct dependencies,
   * which are library names.
   * We also have to read its rpath that helps finding those direct dependencies (for platforms that supports it)
   * \note Are rpath always only for direct dependencies ?
   * Now we have to add the direct dependencies of given target to the graph:
   * \code
   *     (app)
   *     /   \
   * (libA) (Qt5Core)
   * \endcode
   *
   * Now, for \a libA and \a Qt5Core , we have to:
   * - Find the full path, using rpath from \a app
   * - Read them to extract direct dependencies library names + rpath
   * - Add them to the graph
   *
   * We should not modify the graph during a BFS,
   * because that will invalidate internals, like iterators.
   * \sa https://boost-users.boost.narkive.com/Tt0rF8Ao/bgl-add-vertex-during-breadth-first-search
   *
   * So, process the graph (BFS) and return a list of discovered dependencies.
   *
   * Then, process the graph again.
   * Because \a libA and \a Qt5Core can require the rpath from \a app ,
   * we should process starting from \a app .
   *
   * \code
   * DiscoveredDependenciesList discoveredDependenciesList(os);
   * // Reference the discovered dependencies, because the visitor must be cheep to copy
   * GraphBuildVisitor visitor(discoveredDependenciesList,...,...);
   * graph.addFile(target);
   *
   * do{
   *   discoveredDependenciesList.clear();
   *   // process() calls BFS - No graph modifications here
   *   graph.findDependencies(visitor);
   *   // Now add the new discovered dependencies to the graph
   *   graph.addDependencies(discoveredDependenciesList);
   * }while( !discoveredDependenciesList.isEmpty() );
   * \endcode
   *
   * Processing multiple targets should also work:
   * \code
   * graph.addTargets(targets);
   * \endcode
   *
   *
   * ## Tradeoffs
   *
   * This solution works.
   * The complexity is not so good.
   * - we have to find vertices by file names over and over.
   * - we traverse the entier graph over and over
   *
   * But, keep in mind that this graph should not be huge
   * (a application with more than 1'000 dependencies seems not so common).
   * The cost of reading file probably dominates over the cost of traversing
   * the graph and finding vertices.
   * Also, avoiding reading files over and over is handled in the graph.
   *
   * ## Maybe a other manner to do ?
   *
   * First: before changing anything, a benchmark should be created.
   *
   * This is a sandbox that I explored.
   * I did never try to implement this pseudo-code:
   * \code
   * BinaryDependenciesGraphBuildVisitor visitor(...,...);
   * u = graph.addTarget(target);
   * queue.enqueue(u);
   *
   * while( !queue.isEmpty() ){
   *   u = queue.dequeue();
   *   // process() calls BFS - No graph modifications here
   *   directDependencies = graph.processXY(u, visitor);
   *   // Now add the new direct dependencies to the graph
   *   graph.addDependencies(u, directDependencies);
   *   queue.enqueue(directDependencies);
   * }
   * \endcode
   *
   * \todo Maybe inherit QObject for tr() + signal/slot
   */
  class Graph
  {
   public:

    /*! \brief Constructor
     *
     * \note The graph requires a operating system to compare file names.
     * Because some methods, like findTransitiveDependencies()
     * requires a platform, we require it here.
     * This helps to avoid inconsistencies.
     *
     * \pre \a platform must be valid
     */
    explicit
    Graph(const Platform & platform) noexcept
    : mPlatform(platform)
    {
      assert( !mPlatform.isNull() );
    }

    /// \todo disable copy

    /*! \brief Find a vertex by given file name
     *
     * \pre \a fileName must not be empty
     */
    std::optional<VertexDescriptor> findVertex(const QString & fileName) const noexcept
    {
      assert( !fileName.trimmed().isEmpty() );

      const OperatingSystem os = mPlatform.operatingSystem();
      const GraphAL & graph = mGraph;

      const auto pred = [&fileName, os, &graph](VertexDescriptor v){
        return fileNamesAreEqual(graph[v].fileName(), fileName, os);
      };

      const auto itRange = boost::vertices(graph);
      const auto it = std::find_if(itRange.first, itRange.second, pred);

      if(it == itRange.second){
        return {};
      }

      return *it;
    }

    /*! \brief Get the count of files in this graph
     */
    size_t fileCount() const noexcept
    {
      return boost::num_vertices(mGraph);
    }

    /*! \brief Check if this graph contains given file name
     *
     * \pre \a fileName must not be empty
     */
    bool containsFileName(const QString & fileName) const noexcept
    {
      assert( !fileName.trimmed().isEmpty() );

      const auto it = findVertex(fileName);

      return it.has_value();
    }

    /*! \brief Add given target to this graph
     *
     * If given file already exists in this graph,
     * nothing is done.
     *
     * \pre \a file must be an absolute path to a file
     * \sa fileInfoIsAbsolutePath()
     */
    void addTarget(const QFileInfo & file) noexcept
    {
      assert( fileInfoIsAbsolutePath(file) );

      addFile( GraphFile::fromQFileInfo(file) );
    }

    /*! \brief Add given list of targets to this graph
     *
     * \sa addTarget()
     */
    void addTargets(const QFileInfoList & targets) noexcept
    {
      for(const QFileInfo & file : targets){
        assert( fileInfoIsAbsolutePath(file) );
        addTarget(file);
      }
    }

    /*! \internal Add given file to this graph
     *
     * If given file already exists in this graph,
     * nothing is done.
     *
     * \pre \a file must have its file name set
     */
    VertexDescriptor addFile(const GraphFile & file) noexcept
    {
      assert( file.hasFileName() );

      const auto candidateVertex = findVertex( file.fileName() );

      if( candidateVertex.has_value() ){
        return *candidateVertex;
      }

      return boost::add_vertex(file, mGraph);
    }

    /*! \brief Add given dependencies to this graph
     *
     * Each file name in \a dependencies will be added
     * if it does not exist.
     * A dependency from the dependent file to the dependency
     * will be added if it does not exist.
     *
     * \pre The dependent file in \a dependencies
     * must exist in this graph
     * \pre Each dependency file name in \a dependencies
     * must not be empty
     */
    void addDependencies(const DiscoveredDependencies & dependencies) noexcept
    {
      assert( containsFileName( dependencies.dependentFileName() ) );

      const auto parentVertex = findVertex( dependencies.dependentFileName() );
      assert( parentVertex.has_value() );

      for( const QString & libraryName : dependencies.dependenciesFileNames() ){
        VertexDescriptor libraryVertex = addFile( GraphFile::fromLibraryName(libraryName) );
        boost::add_edge(*parentVertex, libraryVertex, mGraph);
      }
    }

    /*! \brief Add given dependencies to this graph
     *
     * \pre For each set in \a dependencies ,
     * the dependent file must exist in this graph
     * \sa addDependencies(const DiscoveredDependencies &)
     */
    void addDependencies(const DiscoveredDependenciesList & dependencies) noexcept
    {
      for(const auto & item : dependencies){
        addDependencies(item);
      }
    }

    /*! \internal Find dependencies for files in this graph
     *
     * Will travel the graph (using BFS)
     * and uses given visitor to find direct dependencies and absolute file paths.
     *
     * \warning Given visitor is not allowed to change this graphs structure
     * (i.e. it must not add/remove vertices/edges).
     *
     * The newly discovered dependencies are added to the visitor,
     * which references them.
     *
     * \sa GraphBuildVisitor
     *
     * \pre This graph must have at least one file
     * \sa addTarget()
     * \sa addTargets()
     */
    template<typename Visitor>
    void findDependencies(Visitor & visitor)
    {
      assert( fileCount() > 0 );

      const VertexDescriptor u = boost::vertex(0, mGraph);

      boost::breadth_first_search( mGraph, u, boost::visitor(visitor) );
    }

    /*! \brief Find transitive dependencies for files in this graph
     *
     * \pre This graph must have at least one file
     * \sa addTarget()
     * \sa addTargets()
     */
    template<typename Reader>
    void findTransitiveDependencies(AbstractSharedLibraryFinder & shLibFinder, Reader & reader)
    {
      assert( fileCount() > 0 );

      DiscoveredDependenciesList discoveredDependenciesList( mPlatform.operatingSystem() );
      GraphBuildVisitorWorker visitorWorker(shLibFinder, mPlatform, discoveredDependenciesList);
      GraphBuildVisitor<Reader> visitor(visitorWorker, reader, mGraph);

      do{
        discoveredDependenciesList.clear();
        findDependencies(visitor);
        addDependencies(discoveredDependenciesList);
      }while( !discoveredDependenciesList.isEmpty() );
    }

    /*! \brief Get a result for given target
     *
     * \pre \a target must be an absolute path to a file
     * \sa fileInfoIsAbsolutePath()
     * \pre \a target must exist in this graph
     */
    BinaryDependenciesResult getResult(const QFileInfo & target) const noexcept
    {
      assert( fileInfoIsAbsolutePath(target) );

      BinaryDependenciesResult result( target, mPlatform.operatingSystem() );

      GraphResultVisitor visitor( result, mPlatform.operatingSystem() );
      auto bfsVisitor = boost::make_bfs_visitor(visitor);

      const auto u = findVertex( target.fileName() );
      assert( u.has_value() );

      boost::breadth_first_search( mGraph, *u, boost::visitor(bfsVisitor) );

      return result;
    }

    /*! \brief Get a list of results for given targets
     *
     * \pre each target in \a targets must be an absolute path to a file
     * \sa fileInfoIsAbsolutePath()
     * \sa getResult()
     */
    BinaryDependenciesResultList getResultList(const QFileInfoList & targets) const noexcept
    {
      BinaryDependenciesResultList resultList( mPlatform.operatingSystem() );

      for(const QFileInfo & target : targets){
        assert( fileInfoIsAbsolutePath(target) );
        resultList.addResult( getResult(target) );
      }

      return resultList;
    }

    /*! \internal Reference the internal graph
     */
    GraphAL & internalGraph() noexcept
    {
      return mGraph;
    }

   private:

    GraphAL mGraph;
    Platform mPlatform;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_H
