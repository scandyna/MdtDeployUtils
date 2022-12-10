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
#include "Mdt/DeployUtils/Impl/BinaryDependencies/Graph.h"
#include "Mdt/DeployUtils/RPath.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QHash>
#include <QLatin1String>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QFileInfoList>
#include <string>
#include <vector>
#include <cassert>

#include "Mdt/DeployUtils/BinaryDependenciesResult.h"

#include <QDebug>

using namespace Mdt::DeployUtils;
using namespace Mdt::DeployUtils::Impl::BinaryDependencies;


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
