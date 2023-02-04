// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "TestUtils.h"
#include "TestFileUtils.h"
#include "TestIsExistingSharedLibrary.h"
#include "BinaryDependenciesTestCommon.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies/Graph.h"
#include "Mdt/DeployUtils/RPath.h"
#include "Mdt/DeployUtils/Platform.h"
#include "Mdt/DeployUtils/BinaryDependenciesResult.h"
#include <QHash>
#include <QLatin1String>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QFileInfoList>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <cassert>

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

  void setRunPath(const std::string & fileName, const std::vector<std::string> & paths)
  {
    RPath rpath;
    for(const auto & path : paths){
      rpath.appendPath( QString::fromStdString(path) );
    }
    mFileMap[QString::fromStdString(fileName)].runPath = rpath;
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

TEST_CASE("TestExecutableFileReader_RPath")
{
  TestExecutableFileReader reader;
  RPath rpath;

  /*
   * app: {/tmp}
   * libA: {/opt}
   */
  reader.setRunPath("app", {"/tmp"});
  reader.setRunPath("libA.so", {"/opt"});

  SECTION("read libB that does not have any rpath")
  {
    reader.openFile( QString::fromLatin1("/tmp/libB.so") );

    REQUIRE( reader.getRunPath().isEmpty() );
  }

  SECTION("read app rpath: {/tmp}")
  {
    reader.openFile( QString::fromLatin1("/tmp/app") );

    rpath = reader.getRunPath();

    REQUIRE( rpath.entriesCount() == 1 );
    REQUIRE( rpath.entryAt(0).path() == QLatin1String("/tmp") );
  }

  SECTION("read libA rpath: {/opt}")
  {
    reader.openFile( QString::fromLatin1("/tmp/libA.so") );

    rpath = reader.getRunPath();

    REQUIRE( rpath.entriesCount() == 1 );
    REQUIRE( rpath.entryAt(0).path() == QLatin1String("/opt") );
  }
}


TEST_CASE("addFile")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  Graph graph(platform);

  const auto app = GraphFile::fromQFileInfo( QString::fromLatin1("/tmp/app") );

  SECTION("app")
  {
    const VertexDescriptor v = graph.addFile(app);

    REQUIRE( graph.fileCount() == 1 );
    REQUIRE( graph.internalGraph()[v].fileName() == QLatin1String("app") );
  }

  SECTION("adding same file twice only adds it once")
  {
    VertexDescriptor v = graph.addFile(app);

    REQUIRE( graph.fileCount() == 1 );
    REQUIRE( graph.internalGraph()[v].fileName() == QLatin1String("app") );

    v = graph.addFile(app);

    REQUIRE( graph.fileCount() == 1 );
    REQUIRE( graph.internalGraph()[v].fileName() == QLatin1String("app") );
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
  auto isExistingSharedLibraryOp = std::make_shared<TestIsExistingSharedLibrary>();
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
  auto isExistingSharedLibraryOp = std::make_shared<TestIsExistingSharedLibrary>();
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  GraphBuildVisitorWorker visitorWorker(shLibFinder, platform, discoveredDependenciesList);
  TestExecutableFileReader reader;
  GraphBuildVisitor<TestExecutableFileReader> visitor( visitorWorker, reader, graph.internalGraph() );

  reader.setNeededSharedLibraries("app", {"libA.so","libQt5Core.so"});
  reader.setNeededSharedLibraries("libA.so", {"libB.so","libQt5Core.so"});

  shLibFinder.setSearchPathList({"/tmp"});
  isExistingSharedLibraryOp->setExistingSharedLibraries({
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
  auto isExistingSharedLibraryOp = std::make_shared<TestIsExistingSharedLibrary>();
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  TestExecutableFileReader reader;
  Graph graph(platform);

  shLibFinder.setSearchPathList({"/tmp"});

  isExistingSharedLibraryOp->setExistingSharedLibraries({
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
  SECTION("starting with multiple targets (app,Qt5Core)")
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

  /*
   * Same as above, but pass Qt5Core then app
   * Bug from 2022/12/18
   */
  SECTION("starting with multiple targets (Qt5Core,app)")
  {
    reader.setNeededSharedLibraries("app", {"libA.so","libQt5Core.so"});
    reader.setNeededSharedLibraries("libA.so", {"libB.so","libQt5Core.so"});

    QFileInfo qt5Core( QString::fromLatin1("/tmp/libQt5Core.so") );
    graph.addTargets({qt5Core,app});

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 4 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libB.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libQt5Core.so") ) );
  }

  /*
   *    (app1)     (app2)
   *     /  \        |
   * (LibA) (libB) (libC)
   */
  SECTION("2 apps with different dependencies")
  {
    QFileInfo app1( QString::fromLatin1("/tmp/app1") );
    reader.setNeededSharedLibraries("app1", {"libA.so","libB.so"});

    QFileInfo app2( QString::fromLatin1("/tmp/app2") );
    reader.setNeededSharedLibraries("app2", {"libC.so"});

    graph.addTarget(app1);
    graph.addTarget(app2);

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 5 );
    REQUIRE( graph.containsFileName( QLatin1String("app1") ) );
    REQUIRE( graph.containsFileName( QLatin1String("app2") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libB.so") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libC.so") ) );
  }
}

TEST_CASE("findTransitiveDependencies_RPath")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  auto isExistingSharedLibraryOp = std::make_shared<TestIsExistingSharedLibrary>();
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  TestExecutableFileReader reader;
  Graph graph(platform);

  isExistingSharedLibraryOp->setExistingSharedLibraries({
    "/tmp/libA.so"
  });

  QFileInfo app( QString::fromLatin1("/tmp/app") );

  /*
   * app
   *  |->libA
   */
  SECTION("app - depends on libA")
  {
    reader.setNeededSharedLibraries("app", {"libA.so"});
    reader.setRunPath("app", {"/tmp"});
    graph.addTarget(app);

    graph.findTransitiveDependencies(shLibFinder, reader);

    REQUIRE( graph.fileCount() == 2 );
    REQUIRE( graph.containsFileName( QLatin1String("app") ) );
    REQUIRE( graph.containsFileName( QLatin1String("libA.so") ) );

    const auto appVertex = graph.findVertex( QLatin1String("app") );
    REQUIRE( appVertex.has_value() );
    const GraphFile & appFile = graph.internalGraph()[*appVertex];
    REQUIRE( appFile.rPath().entriesCount() == 1 );
    REQUIRE( appFile.rPath().entryAt(0).path() == QLatin1String("/tmp") );

    const auto libAVertex = graph.findVertex( QLatin1String("libA.so") );
    REQUIRE( libAVertex.has_value() );
    const GraphFile & libAFile = graph.internalGraph()[*libAVertex];
    REQUIRE( libAFile.hasAbsolutePath() );
    REQUIRE( libAFile.fileInfo().absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }
}

bool resultContainsLibraryAbsolutePath(const BinaryDependenciesResult & result, const std::string & path)
{
  // Take care to check absolute path also on Windows
  const QString qPath = makeAbsolutePath(path);

  const auto pred = [&qPath](const BinaryDependenciesResultLibrary & library){
    if( !library.isFound() ){
      return false;
    }
    return library.absoluteFilePath() == qPath;
  };

  return std::find_if(result.cbegin(), result.cend(), pred) != result.cend();
}


TEST_CASE("addGraphFileToResult")
{
  const auto os = OperatingSystem::Linux;
  QFileInfo app( QString::fromLatin1("/tmp/app") );
  BinaryDependenciesResult result(app, os);

  SECTION("add a found library")
  {
    const auto file = GraphFile::fromQFileInfo( QString::fromLatin1("/tmp/libA.so") );
    REQUIRE( !file.isNotFound() );

    addGraphFileToResult(file, result, os);

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( resultContainsLibraryAbsolutePath(result, "/tmp/libA.so") );
  }

  SECTION("add a found library with rpath")
  {
    RPath rpath;
    rpath.appendPath( QLatin1String("/tmp") );

    auto file = GraphFile::fromQFileInfo( QString::fromLatin1("/tmp/libA.so") );
    file.setRPath(rpath);
    REQUIRE( !file.isNotFound() );

    addGraphFileToResult(file, result, os);

    REQUIRE( result.libraryCount() == 1 );
    const auto library = result.findLibraryByName( QLatin1String("libA.so") );
    REQUIRE( library.has_value() );
    REQUIRE( library->absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
    REQUIRE( library->rPath() == rpath );
  }

  SECTION("adding the target does nothing")
  {
    const auto file = GraphFile::fromQFileInfo(app);
    REQUIRE( !file.isNotFound() );

    addGraphFileToResult(file, result, os);

    REQUIRE( result.libraryCount() == 0 );
  }

  SECTION("add a not found library")
  {
    auto file = GraphFile::fromLibraryName( QLatin1String("libA.so") );
    file.markAsNotFound();
    REQUIRE( file.isNotFound() );

    addGraphFileToResult(file, result, os);

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( !result.libraryAt(0).isFound() );
    REQUIRE( result.libraryAt(0).libraryName() == QLatin1String("libA.so") );
  }

  SECTION("add a library to not redistribute")
  {
    auto file = GraphFile::fromLibraryName( QLatin1String("libA.so") );
    file.markAsNotToBeRedistributed();
    REQUIRE( file.shouldNotBeRedistributed() );

    addGraphFileToResult(file, result, os);

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( result.libraryAt(0).shouldNotBeRedistributed() );
    REQUIRE( result.libraryAt(0).libraryName() == QLatin1String("libA.so") );
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
void buildExampleGraphLinux(Graph & graph)
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  auto isExistingSharedLibraryOp = std::make_shared<TestIsExistingSharedLibrary>();
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  TestExecutableFileReader reader;

  shLibFinder.setSearchPathList({"/tmp"});

  isExistingSharedLibraryOp->setExistingSharedLibraries({
    "/tmp/libA.so",
    "/tmp/libB.so",
    "/tmp/libQt5Core.so"
  });

  reader.setNeededSharedLibraries("app", {"libA.so","libQt5Core.so"});
  reader.setNeededSharedLibraries("libA.so", {"libB.so","libQt5Core.so"});

  QFileInfo app( QString::fromLatin1("/tmp/app") );

  graph.addTarget(app);
  graph.findTransitiveDependencies(shLibFinder, reader);
}


TEST_CASE("getResult")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  Graph graph(platform);
  buildExampleGraphLinux(graph);
  REQUIRE( graph.fileCount() == 4 );

  QFileInfo app( QString::fromLatin1("/tmp/app") );

  /*
   * app
   *  |->libA
   *  |->libB
   *  |->Qt5Core
   */
  SECTION("result for app")
  {
    const BinaryDependenciesResult result = graph.getResult(app);

    REQUIRE( result.target().absoluteFilePath() == makeAbsolutePath("/tmp/app") );
    REQUIRE( result.libraryCount() == 3 );
    REQUIRE( resultContainsLibraryAbsolutePath(result, "/tmp/libA.so") );
    REQUIRE( resultContainsLibraryAbsolutePath(result, "/tmp/libB.so") );
    REQUIRE( resultContainsLibraryAbsolutePath(result, "/tmp/libQt5Core.so") );
  }
}

TEST_CASE("getResultList")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  Graph graph(platform);
  buildExampleGraphLinux(graph);
  REQUIRE( graph.fileCount() == 4 );

  QFileInfo app( QString::fromLatin1("/tmp/app") );
  QFileInfo libA( QString::fromLatin1("/tmp/libA.so") );

  /*
   * app
   *  |->libA
   *  |->libB
   *  |->Qt5Core
   *
   * libA
   *  |->Qt5Core
   *  |->libB
   */
  const BinaryDependenciesResultList resultList = graph.getResultList({app, libA});

  REQUIRE( resultList.resultCount() == 2 );

  const auto appResult = resultList.findResultForTargetName( app.fileName() );
  REQUIRE( appResult.has_value() );
  REQUIRE( appResult->libraryCount() == 3 );
  REQUIRE( appResult->containsLibraryName( QLatin1String("libA.so") ) );
  REQUIRE( appResult->containsLibraryName( QLatin1String("libB.so") ) );
  REQUIRE( appResult->containsLibraryName( QLatin1String("libQt5Core.so") ) );

  const auto libAResult = resultList.findResultForTargetName( libA.fileName() );
  REQUIRE( libAResult.has_value() );
  REQUIRE( libAResult->libraryCount() == 2 );
  REQUIRE( libAResult->containsLibraryName( QLatin1String("libQt5Core.so") ) );
  REQUIRE( libAResult->containsLibraryName( QLatin1String("libB.so") ) );
}

/*
 * This test aims to take libararies
 * that has not to be distributed
 * and not found libraries
 *
 * app
 *  |->Mylib
 *  |   |->Qt5Core
 *  |   |->libc
 *  |   |->NotFound
 *  |->Qt5Core
 *      |->libc
 *
 * libc must not be redistributed
 *
 *           (app)
 *           /   \
 *        (Mylib) |
 *        /  \  \ |
 * (NotFound) \ (Qt5Core)
 *             \  /
 *            (libc)
 */
TEST_CASE("LibrariesStateTest")
{
  const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
  auto isExistingSharedLibraryOp = std::make_shared<TestIsExistingSharedLibrary>();
  SharedLibraryFinderBDTest shLibFinder(isExistingSharedLibraryOp);
  TestExecutableFileReader reader;

  shLibFinder.setSearchPathList({"/tmp"});
  shLibFinder.setLibraryNamesToNotRedistribute({"libc.so"});

  isExistingSharedLibraryOp->setExistingSharedLibraries({
    "/tmp/libMylib.so",
    "/tmp/libc.so",
    "/tmp/libQt5Core.so"
  });

  reader.setNeededSharedLibraries("app", {"libMylib.so","libQt5Core.so"});
  reader.setNeededSharedLibraries("libMylib.so", {"libc.so","libQt5Core.so","libNotFound.so"});
  reader.setNeededSharedLibraries("libQt5Core.so", {"libc.so"});

  QFileInfo app( QString::fromLatin1("/tmp/app") );

  Graph graph(platform);
  graph.addTarget(app);
  graph.findTransitiveDependencies(shLibFinder, reader);

  /*
   * app
   *  |->Mylib
   *  |->libc
   *  |->Qt5Core
   *  |->NotFound
   */
  const BinaryDependenciesResult result = graph.getResult(app);

  REQUIRE( result.libraryCount() == 4 );

  const auto myLib = result.findLibraryByName( QLatin1String("libMylib.so") );
  REQUIRE( myLib.has_value() );
  REQUIRE( myLib->isFound() );
  REQUIRE( !myLib->shouldNotBeRedistributed() );

  const auto libc = result.findLibraryByName( QLatin1String("libc.so") );
  REQUIRE( libc.has_value() );
  REQUIRE( libc->shouldNotBeRedistributed() );

  const auto qt5Core = result.findLibraryByName( QLatin1String("libQt5Core.so") );
  REQUIRE( qt5Core.has_value() );
  REQUIRE( qt5Core->isFound() );
  REQUIRE( !qt5Core->shouldNotBeRedistributed() );

  const auto notFound = result.findLibraryByName( QLatin1String("libNotFound.so") );
  REQUIRE( notFound.has_value() );
  REQUIRE( !notFound->isFound() );
  REQUIRE( !notFound->shouldNotBeRedistributed() );

  REQUIRE( !result.isSolved() );
}
