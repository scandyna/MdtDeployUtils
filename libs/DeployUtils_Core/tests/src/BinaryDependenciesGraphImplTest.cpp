// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "TestUtils.h"
#include "TestIsExistingSharedLibrary.h"
#include "BinaryDependenciesTestCommon.h"
#include "Mdt/DeployUtils/BinaryDependenciesGraph.h"
#include <QHash>


#include "Mdt/DeployUtils/Impl/BinaryDependencies/FileComparison.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies/GraphFile.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies/DiscoveredDependenciesList.h"

#include <QLatin1String>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QFileInfoList>
#include <string>
#include <algorithm>
#include <optional>

#include "Mdt/DeployUtils/FileInfoUtils.h"
#include "Mdt/DeployUtils/RPath.h"
#include "Mdt/DeployUtils/AbstractSharedLibraryFinder.h"
#include "Mdt/DeployUtils/FindDependencyError.h"
#include "Mdt/DeployUtils/BinaryDependenciesResult.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QObject>
#include <QFileInfo>
#include <QFileInfoList>
#include <memory>
#include <cassert>

#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <QDebug>

using namespace Mdt::DeployUtils;

using Mdt::DeployUtils::Impl::BinaryDependencies::GraphFile;
using Mdt::DeployUtils::Impl::BinaryDependencies::DiscoveredDependencies;
using Mdt::DeployUtils::Impl::BinaryDependencies::DiscoveredDependenciesList;

class TestExecutableFileReader
{
 public:

  void openFile(const QFileInfo & fileInfo)
  {
    assert( fileInfoHasFileName(fileInfo) );

    mCurrentFileName = fileInfo.fileName();
  }

  void openFile(const QFileInfo & fileInfo, const Platform &)
  {
    openFile(fileInfo);
  }

  bool isOpen() const noexcept
  {
    return !mCurrentFileName.isEmpty();
  }

  void close()
  {
    mCurrentFileName.clear();
  }

//   Platform getFilePlatform()
//   {
//   }

  bool isExecutableOrSharedLibrary()
  {
    return true;
  }

  QStringList getNeededSharedLibraries()
  {
    return mFileMap.value(mCurrentFileName).neededSharedLibraries;
  }

  RPath getRunPath()
  {
    return mFileMap.value(mCurrentFileName).runPath;
  }

  void setNeededSharedLibraries(const std::string & dependendFileName, const std::vector<std::string> & librariesFileNames)
  {
    mFileMap[QString::fromStdString(dependendFileName)].neededSharedLibraries = qStringListFromUtf8Strings(librariesFileNames);
  }

  void setRunPath(const std::string & fileName, const RPath & paths)
  {
    mFileMap[QString::fromStdString(fileName)].runPath = paths;
  }

 private:

  struct File
  {
    QStringList neededSharedLibraries;
    RPath runPath;
  };

  QHash<QString, File> mFileMap;
  QString mCurrentFileName;
};

TEST_CASE("TestExecutableFileReader_OpenClose")
{
  TestExecutableFileReader reader;
  REQUIRE( !reader.isOpen() );

  reader.openFile( QString::fromLatin1("/tmp/app") );
  REQUIRE( reader.isOpen() );

  reader.close();
  REQUIRE( !reader.isOpen() );
}

TEST_CASE("TestExecutableFileReader_NeededSharedLibraries")
{
  TestExecutableFileReader reader;

  /*
   * app
   *  |->libA
   *  |   |->Qt5Core
   *  |   |->libB
   *  |->Qt5Core
   */
  reader.setNeededSharedLibraries("app", {"libA.so","libQt5Core.so"});
  reader.setNeededSharedLibraries("libA.so", {"libB.so","libQt5Core.so"});

  SECTION("read libB that does not have any dependency")
  {
    reader.openFile( QString::fromLatin1("/tmp/libB.so") );

    REQUIRE( reader.getNeededSharedLibraries().isEmpty() );
  }

  SECTION("app depends on Qt5Core and libA")
  {
    reader.openFile( QString::fromLatin1("/tmp/app") );

    const QStringList dependencies = reader.getNeededSharedLibraries();
    REQUIRE( dependencies.size() == 2 );
    REQUIRE( dependencies.contains( QLatin1String("libQt5Core.so") ) );
    REQUIRE( dependencies.contains( QLatin1String("libA.so") ) );
  }

  SECTION("libA depends on Qt5Core and libB")
  {
    reader.openFile( QString::fromLatin1("/tmp/libA.so") );

    const QStringList dependencies = reader.getNeededSharedLibraries();
    REQUIRE( dependencies.size() == 2 );
    REQUIRE( dependencies.contains( QLatin1String("libQt5Core.so") ) );
    REQUIRE( dependencies.contains( QLatin1String("libB.so") ) );
  }
}

/** \todo
 *
 * - To find some file (Vertex), not BFS required, find by using vertex iterators
 *
 * - Cound we solve all transitive deps in implementation ? Would be very nice !
 *   -> findTransitiveDependencies( targets, someVisitor )
 */

/** \internal How to build the graph
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
 * graph.addFiles(targets);
 * \endcode
 *
 * \todo document that we have to handle things like colors
 *
 * 
 * \code
 * BinaryDependenciesGraphBuildVisitor visitor(...,...);
 * u = graph.addFile(target);
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
 * \todo document that complexity is not good, but tradeoff..
 *
 * Processing multiple targets should also work:
 * \code
 * graph.addXY(targets);
 * queue.enqueue(targets);
 * \endcode
 *
 */


using GraphAL = boost::adjacency_list<
  boost::hash_setS, // OutEdgeList: hash_setS (boost::unordered_set): choosed to avoid adding a edge more than once
  boost::vecS,      // VertexList: vecS (std::vector)
  boost::directedS, // Directed: directedS (Directed graph)
  GraphFile         // VertexProperties
>;

using VertexDescriptor = boost::graph_traits<GraphAL>::vertex_descriptor;
using EdgeDescriptor = boost::graph_traits<GraphAL>::edge_descriptor;

using VertexIterator = boost::graph_traits<GraphAL>::vertex_iterator;

/// \todo Extract vertex container from graph ? or not ?
//using VertexDescriptorList = boost::graph_traits<GraphAL>::v
using VertexDescriptorList = std::vector<VertexDescriptor>;


/*! \internal Worker for GraphBuildVisitor
 *
 * This worker provides translation and signal/slot support.
 */
class GraphBuildVisitorWorker : public QObject
{
  ///Q_OBJECT

 public:

  /*! \brief Constructor
   */
  explicit
  GraphBuildVisitorWorker(AbstractSharedLibraryFinder & sharedLibraryFinder,
                          const Platform & platform,
                          DiscoveredDependenciesList & discoveredDependenciesList,
                          QObject *parent = nullptr) noexcept
   : QObject(parent),
     mSharedLibraryFinder(sharedLibraryFinder),
     mPlatform(platform),
     mDiscoveredDependenciesList(discoveredDependenciesList)
  {
  }

  /*!\brief Read given file to extract dependencies and rpath if supported
   */
  template<typename Reader>
  void readFile(GraphFile & file, Reader & reader)
  {
    assert( !file.isReaden() );
    assert( file.hasAbsolutePath() );
    assert( !reader.isOpen() );

    qDebug() << "read: " << file.fileName() << " (path: " << file.fileInfo().absoluteFilePath() << ")";
    /// emitProcessingCurrentFileMessage(currentFile);

    reader.openFile(file.fileInfo(), mPlatform);
    if( !reader.isExecutableOrSharedLibrary() ){
      /**
      const QString message = tr("'%1' is not a executable or a shared library")
                              .arg( file.absoluteFilePath() );
      throw FindDependencyError(message);
      */
    }

    mDiscoveredDependenciesList.setDirectDependenciesFileNames( file, reader.getNeededSharedLibraries() );

    /// emitDirectDependenciesMessage(currentFile);

    file.setRPath( reader.getRunPath() );

    reader.close();
  }

  /*! \brief Find the absolute path for given library name
   */
  void findLibraryAbsolutePath(GraphFile & file, const GraphFile & parentFile)
  {
    assert( !file.hasBeenSearched() );
    assert( fileInfoIsFileNameWithoutPath( file.fileInfo() ) );

    const QString libraryName = file.fileName();

    if( !mSharedLibraryFinder.libraryShouldBeDistributed(libraryName) ){
      file.markAsNotToBeRedistributed();
      return;
    }
    qDebug() << " find full path of " << libraryName << " using rpath from " << parentFile.fileName();

    const auto dependentFile = BinaryDependenciesFile::fromQFileInfo( parentFile.fileInfo() );
    try{
      const QFileInfo path = mSharedLibraryFinder.findLibraryAbsolutePath( libraryName, dependentFile );
      assert( fileInfoIsAbsolutePath(path) );
      file.setAbsoluteFilePath(path);
    }catch(const FindDependencyError &){
      file.markAsNotFound();
    }
  }

 private:

  AbstractSharedLibraryFinder & mSharedLibraryFinder;
  const Platform & mPlatform;
  DiscoveredDependenciesList & mDiscoveredDependenciesList;
};

/*! \internal Visitor used to build a binary dependencies graph
 */
template<typename Reader>
class GraphBuildVisitor : public boost::default_bfs_visitor
{
 public:

  /*! \brief Constructor
   *
   * The worker inherits QObject and requires the Q_OBJECT macro.
   * So, it can't be a class templated on reader.
   * As workaround, reference the reader here.
   *
   * We have to modify some vertex properties (set absolute file path, rpath, ...).
   * The BFS takes a const graph, so we can't use the graph passed as argument
   * to this visitor methods.
   * As workaround, we reference the graph as non const.
   */
  explicit
  GraphBuildVisitor(GraphBuildVisitorWorker & worker, Reader & reader, GraphAL & graph) noexcept
   : mWorker(worker),
     mReader(reader),
     mGraph(graph)
  {
  }

  /*! \brief Read the file associated with the new discovered vertex
   *
   * Will read the new discovered file to get its direct dependencies librarie names
   * and its rpath (if supported).
   *
   * If this was already done for the given file,
   * nothing is done (the graph can be processed multiple times during build).
   *
   * \pre The absolute path of given file must have been set
   */
  void discover_vertex(VertexDescriptor u, const GraphAL &)
  {
    GraphFile & file = mGraph[u];

//     qDebug() << "discover vertex " << file.fileName();

    if( file.isReaden() ){
      return;
    }
    assert( file.hasAbsolutePath() );

    mWorker.readFile(file, mReader);

    file.markAsReaden();
  }

  /*! \brief Find the full path for the file associated with given target vertex
   *
   * Will find the full path for given library name
   * (associated with the target vertex of given edge).
   *
   * If appropriate for the platform,
   * the rpath from the "parent" file will be used
   * ("parent" is the file associated with the source vertex of given edge).
   *
   * If the full path is already set,
   * nothing is done (the graph can be processed multiple times during build).
   *
   * \todo flag if find full path fails
   */
  void examine_edge(EdgeDescriptor e, const GraphAL & g)
  {
    const VertexDescriptor parent = boost::source(e, g);
    const VertexDescriptor current = boost::target(e, g);

//     const GraphFile & parentFile = mGraph[parent];
    GraphFile & currentFile = mGraph[current];

    if( currentFile.hasBeenSearched() ){
      return;
    }

    mWorker.findLibraryAbsolutePath(currentFile, mGraph[parent]);
  }

 private:

  GraphBuildVisitorWorker & mWorker;
  Reader & mReader;
  GraphAL & mGraph;
};



/*! \internal
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

  /*! \brief Find a vertex by given file name
   *
   * \pre \a fileName must not be empty
   */
  std::optional<VertexDescriptor> findVertex(const QString & fileName) const noexcept
  {
    assert( !fileName.trimmed().isEmpty() );

    using Mdt::DeployUtils::Impl::BinaryDependencies::fileNamesAreEqual;

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

  /*! \internal Find transitive dependencies for files in this graph
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

  /*! \internal Reference the internal graph
   */
  GraphAL & internalGraph() noexcept
  {
    return mGraph;
  }

  /*! \brief Check if given files are equal
   *
   * Returns true if file \a a and \a b have the same file name.
   * It does not matter given files are absolute paths.
   *
   * \pre \a a and \a b must have their file name set
   * \sa fileInfoHasFileName()
   */
//   static
//   bool filesAreEqual(const QFileInfo & a, const QFileInfo & b) noexcept
//   {
//     assert( fileInfoHasFileName(a) );
//     assert( fileInfoHasFileName(b) );
//   }

 private:

  GraphAL mGraph;
  Platform mPlatform;
};

/*! \internal
 */
template<typename Reader>
BinaryDependenciesResult
findDependencies(const QFileInfo & target,
                 const std::shared_ptr<AbstractSharedLibraryFinder> & shLibFinder,
                 Reader & reader, const Platform & platform)
{
}

/*! \internal
 */
template<typename Reader>
BinaryDependenciesResultList
findDependencies(const QFileInfoList & targetList,
                 const std::shared_ptr<AbstractSharedLibraryFinder> & shLibFinder,
                 Reader & reader, const Platform & platform)
{
}


TEST_CASE("addTarget")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  Graph graph(platform);

  QFileInfo app( QString::fromLatin1("/tmp/app") );

  SECTION("app")
  {
    graph.addTarget(app);

    REQUIRE( graph.fileCount() == 1 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
  }

  SECTION("adding same target twice only adds it once")
  {
    graph.addTarget(app);
    graph.addTarget(app);

    REQUIRE( graph.fileCount() == 1 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
  }
}

TEST_CASE("addTargets")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  Graph graph(platform);

  QFileInfoList targets{QString::fromLatin1("/tmp/app"),QString::fromLatin1("/tmp/libA.so")};

  graph.addTargets(targets);

  REQUIRE( graph.fileCount() == 2 );
  REQUIRE( graph.containsFileName( QLatin1String("app") ) );
  REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
}

TEST_CASE("addDependencies")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  Graph graph(platform);
  DiscoveredDependenciesList discoveredDependenciesList( platform.operatingSystem() );
  QStringList dependencies;

  GraphFile app = GraphFile::fromQFileInfo( QString::fromLatin1("/tmp/app") );

  SECTION("app depends on libA.so")
  {
    dependencies = qStringListFromUtf8Strings({"libA.so"});
    discoveredDependenciesList.setDirectDependenciesFileNames(app, dependencies);

    graph.addFile(app);
    graph.addDependencies(discoveredDependenciesList);

    REQUIRE( graph.fileCount() == 2 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
  }
}

TEST_CASE("findDependencies")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  const OperatingSystem os = platform.operatingSystem();
  Graph graph(platform);
  DiscoveredDependenciesList discoveredDependenciesList(os);
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  GraphBuildVisitorWorker visitorWorker(shLibFinder, platform, discoveredDependenciesList);
  TestExecutableFileReader reader;
  GraphBuildVisitor<TestExecutableFileReader> visitor( visitorWorker, reader, graph.internalGraph() );
  QStringList expectedDependenciesFileNames;

  const QFileInfo app( QString::fromLatin1("/tmp/app") );

  /*
   * (app)
   */
  SECTION("app without any dependency")
  {
    graph.addTarget(app);

    graph.findDependencies(visitor);

    REQUIRE( graph.fileCount() == 1 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( discoveredDependenciesList.isEmpty() );
  }

  /*
   * app
   *  |->libA.so
   */
  SECTION("app depends on libA")
  {
    graph.addTarget(app);

    reader.setNeededSharedLibraries("app", {"libA.so"});

    graph.findDependencies(visitor);

    // The graph is unchanged
    REQUIRE( graph.fileCount() == 1 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );

    REQUIRE( discoveredDependenciesList.size() == 1 );
    REQUIRE( discoveredDependenciesList.at(0).dependentFileName() == QLatin1String("app") );
    expectedDependenciesFileNames = qStringListFromUtf8Strings({"libA.so"});
    REQUIRE( discoveredDependenciesList.at(0).dependenciesFileNames() == expectedDependenciesFileNames );

    // Run again, graph does not change and no new dependency is discovered
    discoveredDependenciesList.clear();
    graph.findDependencies(visitor);
    REQUIRE( graph.fileCount() == 1 );
    REQUIRE( discoveredDependenciesList.isEmpty() );
  }
}

/*
 * app
 *  |->libA
 *  |   |->Qt5Core
 *  |   |->libB
 *  |->Qt5Core
 *
 *
 *      (app)
 *      /   \
 *  (libA)   |
 *     |  \  |
 * (libB) (Qt5Core)
 */
TEST_CASE("buildGraph_docExample")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  const OperatingSystem os = platform.operatingSystem();
  Graph graph(platform);
  DiscoveredDependenciesList discoveredDependenciesList(os);
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  GraphBuildVisitorWorker visitorWorker(shLibFinder, platform, discoveredDependenciesList);
  TestExecutableFileReader reader;
  GraphBuildVisitor<TestExecutableFileReader> visitor( visitorWorker, reader, graph.internalGraph() );

  reader.setNeededSharedLibraries("app", {"libA.so","libQt5Core.so"});
  reader.setNeededSharedLibraries("libA.so", {"libB.so","libQt5Core.so"});

  shLibFinder.setSearchPathList({"/tmp"});
  isExistingSharedLibraryOp.setExistingSharedLibraries({
    "/tmp/libA.so",
    "/tmp/libB.so",
    "/tmp/libQt5Core.so"
  });

  /*
   * Add the target
   *
   * (app)
   */
  const QFileInfo app( QString::fromLatin1("/tmp/app") );
  graph.addTarget(app);

  /*
   * Discover app dependencies
   *
   * app
   *  |->libA
   *  |->Qt5Core
   *
   * (app)
   */
  discoveredDependenciesList.clear();
//   reader.setNeededSharedLibraries({"libA.so","libQt5Core.so"});
  graph.findDependencies(visitor);

  REQUIRE( discoveredDependenciesList.size() == 1 );
  REQUIRE( discoveredDependenciesList.at(0).dependentFileName() == QLatin1String("app") );
  REQUIRE( discoveredDependenciesList.at(0).dependenciesFileNames().size() == 2 );
  REQUIRE( discoveredDependenciesList.at(0).dependenciesFileNames().contains( QLatin1String("libA.so") ) );
  REQUIRE( discoveredDependenciesList.at(0).dependenciesFileNames().contains( QLatin1String("libQt5Core.so") ) );

  /*
   * Add app dependencies to the graph
   *
   *     (app)
   *     /   \
   * (libA) (Qt5Core)
   */
  graph.addDependencies(discoveredDependenciesList);

  /*
   * Find the absolute path for libA and Qt5Core
   *
   * Discover dependencies for libA and Qt5Core
   *
   * libA
   *  |->Qt5Core
   *  |->libB
   *
   * Qt5Core
   */
  discoveredDependenciesList.clear();
  /// \todo the reader will not be usable for more than 1 dependent file !
//   reader.setNeededSharedLibraries({"libB.so","libQt5Core.so"});
  graph.findDependencies(visitor);

  REQUIRE( discoveredDependenciesList.size() == 1 );
  REQUIRE( discoveredDependenciesList.at(0).dependentFileName() == QLatin1String("libA.so") );
  REQUIRE( discoveredDependenciesList.at(0).dependenciesFileNames().size() == 2 );
  REQUIRE( discoveredDependenciesList.at(0).dependenciesFileNames().contains( QLatin1String("libQt5Core.so") ) );
  REQUIRE( discoveredDependenciesList.at(0).dependenciesFileNames().contains( QLatin1String("libB.so") ) );

  /*
   * Add libA dependencies to the graph
   *
   *      (app)
   *      /   \
   *  (libA)   |
   *     |  \  |
   * (libB) (Qt5Core)
   */
  graph.addDependencies(discoveredDependenciesList);

  /*
   * Find the absolute path for libB
   *
   * Discover dependencies for libB
   *
   * libB
   */
  discoveredDependenciesList.clear();
  graph.findDependencies(visitor);
//   reader.setNeededSharedLibraries({});
  graph.findDependencies(visitor);

  REQUIRE( discoveredDependenciesList.isEmpty() );

  REQUIRE( graph.fileCount() == 4 );
  REQUIRE( graph.containsFileName( QLatin1String("app") ) );
  REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
  REQUIRE( graph.containsFileName( QLatin1String("libB.so") ) );
  REQUIRE( graph.containsFileName( QLatin1String("libQt5Core.so") ) );
}

TEST_CASE("findTransitiveDependencies")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  TestExecutableFileReader reader;
  Graph graph(platform);

  shLibFinder.setSearchPathList({"/tmp"});

  isExistingSharedLibraryOp.setExistingSharedLibraries({
    "/tmp/libA.so",
    "/tmp/libB.so",
    "/tmp/libC.so",
    "/tmp/libQt5Core.so"
  });

  QFileInfo app( QString::fromLatin1("/tmp/app") );

  /*
   * app
   *
   * (app)
   */
  SECTION("app - no dependencies")
  {
    graph.addTarget(app);

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 1 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
  }

  /*
   * app
   *  |->libA
   */
  SECTION("app - depends on libA")
  {
    reader.setNeededSharedLibraries("app", {"libA.so"});
    graph.addTarget(app);

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 2 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
  }

  /*
   * app
   *  |->libA
   *      |->libB
   *
   * (app)
   *   |
   * (libA)
   *   |
   * (libB)
   */
  SECTION("app -> libA -> libB")
  {
    reader.setNeededSharedLibraries("app", {"libA.so"});
    reader.setNeededSharedLibraries("libA.so", {"libB.so"});

    graph.addTarget(app);

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 3 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libB.so") ) );
  }

  /*
   * app
   *  |->libA
   *      |->libB
   *          |->libA
   *
   * (app)
   *   |
   * (libA)
   *   |
   * (libB)
   *
   * NOTE: happens in real world (some Windows dll's as example)
   */
  SECTION("app -> libA <-> libB (cycle)")
  {
    reader.setNeededSharedLibraries("app", {"libA.so"});
    reader.setNeededSharedLibraries("libA.so", {"libB.so"});
    reader.setNeededSharedLibraries("libB.so", {"libA.so"});

    graph.addTarget(app);

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 3 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libB.so") ) );
  }

  /*
   * app
   *  |->libA
   *  |->libB
   *  |->libC
   *
   *         (app)
   *       /   |   \
   * (libA) (libB) (libC)
   */
  SECTION("app depends on libA libB and libC")
  {
    reader.setNeededSharedLibraries("app", {"libA.so","libB.so","libC.so"});

    graph.addTarget(app);

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 4 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libB.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libC.so") ) );
  }

  /*
   * app
   *  |->libA
   *  |   |->Qt5Core
   *  |   |->libB
   *  |->Qt5Core
   *
   *
   *      (app)
   *      /   \
   *  (libA)   |
   *     |  \  |
   * (libB) (Qt5Core)
   */
  SECTION("doc example")
  {
    reader.setNeededSharedLibraries("app", {"libA.so","libQt5Core.so"});
    reader.setNeededSharedLibraries("libA.so", {"libB.so","libQt5Core.so"});

    graph.addTarget(app);

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 4 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libB.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libQt5Core.so") ) );
  }

  /*
   * Doc example as above but start with 2 targets
   */
  SECTION("starting with multiple targets")
  {
    reader.setNeededSharedLibraries("app", {"libA.so","libQt5Core.so"});
    reader.setNeededSharedLibraries("libA.so", {"libB.so","libQt5Core.so"});

    QFileInfo qt5Core( QString::fromLatin1("/tmp/libQt5Core.so") );
    graph.addTargets({app, qt5Core});

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 4 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libB.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libQt5Core.so") ) );
  }
}

TEST_CASE("sandbox")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  Graph graph(platform);
  QFileInfo app( QString::fromLatin1("/tmp/app") );
}

TEST_CASE("buildGraphAL_sandbox")
{
  /*
   * app
   *  |->libA
   *  |   |->Qt5Core
   *  |   |->libB
   *  |->Qt5Core
   */
  GraphAL graph;

  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  const auto platform = Platform::nativePlatform();
  DiscoveredDependenciesList discoveredDependenciesList( platform.operatingSystem() );
  GraphBuildVisitorWorker visitorWorker(shLibFinder, platform, discoveredDependenciesList);
  TestExecutableFileReader reader;
  GraphBuildVisitor<TestExecutableFileReader> visitor(visitorWorker, reader, graph);

  reader.setNeededSharedLibraries("app", {"libA.so","libQt5Core.so"});
  reader.setNeededSharedLibraries("libA.so", {"libB.so","libQt5Core.so"});

  shLibFinder.setSearchPathList({"/opt/lib"});
  isExistingSharedLibraryOp.setExistingSharedLibraries({"/opt/lib/libA", "/opt/lib/libB","/opt/lib/Qt5Core"});

  /*
   * Add app to the graph + its vertex to the queue
   *
   * (app)
   */
  VertexDescriptor u;
  VertexDescriptor app;
  QFileInfo appFi = QString::fromLatin1("/opt/app");
  app = boost::add_vertex( GraphFile::fromQFileInfo(appFi), graph );

  /*
   * dequeue, process app
   *
   * app
   *  |->libA
   *  |->Qt5Core
   *
   */
  u = app;
//   reader.setNeededSharedLibraries({"libA","Qt5Core"});
  boost::breadth_first_search( graph, u, boost::visitor(visitor) );
  /// \todo Check visitor contains libA and Qt5Core

  /*
   * Add libA and Qt5Core to the graph,
   * and their respective vertices to the queue
   *
   *     (app)
   *     /   \
   * (libA) (Qt5Core)
   */
  VertexDescriptor libA, Qt5Core;
  libA = boost::add_vertex( GraphFile::fromLibraryName( QString::fromLatin1("libA") ), graph );
  boost::add_edge(u, libA, graph);
  Qt5Core = boost::add_vertex( GraphFile::fromLibraryName( QString::fromLatin1("Qt5Core") ), graph );
  boost::add_edge(u, Qt5Core, graph);

  /*
   * dequeue, process libA
   *
   * libA
   *  |->libB
   *  |->Qt5Core
   */
  u = libA;
//   reader.setNeededSharedLibraries({"libB","Qt5Core"});
  boost::breadth_first_search( graph, app, boost::visitor(visitor) );
  /// \todo Check libA has its absolute path
  /// \todo Check visitor contains libB and Qt5Core

  /*
   * Add libB and Qt5Core to the graph,
   * and their respective vertices to the queue
   *
   * NOTE: the final graph implementation,
   * and queue implementation does not add what already exists
   */

}

TEST_CASE("sandboxAL")
{
  /*
   * app
   *  |->libA
   *  |   |->Qt5Core
   *  |   |->libB
   *  |->Qt5Core
   */
  VertexDescriptor app, libA, Qt5Core;
  GraphAL graph;

  app = boost::add_vertex( GraphFile::fromLibraryName( QString::fromLatin1("app") ), graph );
  libA = boost::add_vertex( GraphFile::fromLibraryName( QString::fromLatin1("libA") ), graph );
  Qt5Core = boost::add_vertex( GraphFile::fromLibraryName( QString::fromLatin1("Qt5Core") ), graph );

  qDebug() << "a type     : " << typeid(app).name() << " , hash: " << typeid(app).hash_code();
  qDebug() << "size_t type: " << typeid(size_t).name() << " , hash: " << typeid(size_t).hash_code();

  
  qDebug() << "graph[0] type: " << typeid(graph[0]).name();
  qDebug() << "graph[app] type: " << typeid(graph[app]).name();
  
  qDebug() << "graph[app]: " << graph[app].fileName();
  
//   graph[c] = QLatin1String("c");
//   qDebug() << "graph[c]: " << graph[c];
//   qDebug() << "graph[c] size: " << graph[c].size();
  
  EdgeDescriptor edge;
  bool inserted = false;

  boost::tie(edge, inserted) = boost::add_edge(app, libA, graph);
  qDebug() << "inserted app->libA: " << inserted;

  boost::tie(edge, inserted) = boost::add_edge(app, libA, graph);
  qDebug() << "inserted app->libA: " << inserted;

  boost::tie(edge, inserted) = boost::add_edge(app, Qt5Core, graph);
  qDebug() << "inserted app->Qt5Core: " << inserted;

  // Cycle
//   boost::tie(edge, inserted) = boost::add_edge(c, a, graph);
//   qDebug() << "inserted c->a: " << inserted;

// //   boost::bfs_visitor<MyBFSVisitor> visitor;

  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  const auto platform = Platform::nativePlatform();
  DiscoveredDependenciesList discoveredDependenciesList( platform.operatingSystem() );
  GraphBuildVisitorWorker visitorWorker(shLibFinder, platform, discoveredDependenciesList);
  TestExecutableFileReader reader;
  GraphBuildVisitor<TestExecutableFileReader> visitor(visitorWorker, reader, graph);


  reader.setNeededSharedLibraries("app", {"libA","Qt5Core"});
  boost::breadth_first_search( graph, app, boost::visitor(visitor) );
}


BinaryDependenciesFile binaryDependenciesFileFromPath(const std::string & path)
{
  return BinaryDependenciesFile::fromQFileInfo( QString::fromStdString(path));
}

bool fileListContainsFileName(const BinaryDependenciesFileList & files, const std::string & name)
{
  const QString expectedName = QString::fromStdString(name);
  const auto pred = [&expectedName](const BinaryDependenciesFile & file){
    return file.fileName() == expectedName;
  };

  const auto it = std::find_if(files.cbegin(), files.cend(), pred);

  return it != files.cend();
}


TEST_CASE("addFile_findUnprocessedFiles")
{
  BinaryDependenciesGraph graph;
  BinaryDependenciesFileList unprocessedFiles;

  SECTION("empty graph")
  {
    unprocessedFiles = graph.findUnprocessedFiles();

    REQUIRE( unprocessedFiles.empty() );
  }

  SECTION("app")
  {
    const auto app = binaryDependenciesFileFromPath("/path/to/app");

    unprocessedFiles = graph.findUnprocessedFiles();

    REQUIRE( unprocessedFiles.size() == 1 );
    REQUIRE( fileListContainsFileName(unprocessedFiles, "app") );
  }
}

TEST_CASE("solvingExample")
{
  BinaryDependenciesGraph graph;
//   BinaryDependenciesResult result;

  SECTION("app (no dependencies)")
  {
    REQUIRE(false);
  }

  SECTION("test not implemented")
  {
    REQUIRE(false);
  }
}

TEST_CASE("solvingExample_multipleTargets")
{
  SECTION("test not implemented")
  {
    REQUIRE(false);
  }
}
