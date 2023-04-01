/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2023 Philippe Steinmann.
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
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "TestUtils.h"
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/CopySharedLibrariesTargetDependsOn.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/ConsoleMessageLogger.h"
#include "Mdt/DeployUtils/RPath.h"
#include <Mdt/ExecutableFile/ExecutableFileReader.h>
#include <Mdt/ExecutableFile/ExecutableFileWriter.h>
#include <QTemporaryDir>

using namespace Mdt::DeployUtils;
using Mdt::ExecutableFile::ExecutableFileReader;
using Mdt::ExecutableFile::ExecutableFileWriter;


#ifdef Q_OS_WIN
  constexpr bool checkRPath = false;
#else
  constexpr bool checkRPath = true;
#endif

bool installExecutable(const QString & sourceExecutablePath, const QString & destinationExecutablePath)
{
  if( !copyFile(sourceExecutablePath, destinationExecutablePath) ){
    return false;
  }

  RPath rpath;
  rpath.appendPath( QLatin1String(".") );

  ExecutableFileWriter writer;
  writer.openFile(destinationExecutablePath);
  writer.setRunPath(rpath);
  writer.close();

  return true;
}

RPath getTestSharedLibraryRunPath(const QTemporaryDir & dir)
{
  ExecutableFileReader reader;
  reader.openFile( makePath(dir, "libtestSharedLibrary.so") );

  return reader.getRunPath();
}


/*
 * It was a atempt to run the test executable once copied.
 * On some builds this not worked because of a assertion in QLibraryInfo
 * see https://bugreports.qt.io/browse/QTBUG-86380
 */
TEST_CASE("CopySharedLibrariesTargetDependsOn")
{
  QTemporaryDir destinationDir;
  destinationDir.setAutoRemove(true);
  REQUIRE( destinationDir.isValid() );

  CopySharedLibrariesTargetDependsOnRequest request;

  request.targetFilePath = QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH);
#ifdef COMPILER_IS_MSVC
  request.compilerLocation.setType(CompilerLocationType::CompilerPath);
  request.compilerLocation.setValue( QString::fromLocal8Bit(CXX_COMPILER_PATH) );
#endif // #ifdef COMPILER_IS_MSVC
  request.searchPrefixPathList = getTestPrefixPath(PREFIX_PATH);
  request.destinationDirectoryPath = destinationDir.path();
  request.overwriteBehavior = OverwriteBehavior::Overwrite;

  MessageLogger messageLogger;
  MessageLogger::setBackend<ConsoleMessageLogger>();

  CopySharedLibrariesTargetDependsOn useCase;

  QObject::connect(&useCase, &CopySharedLibrariesTargetDependsOn::statusMessage, MessageLogger::info);
  QObject::connect(&useCase, &CopySharedLibrariesTargetDependsOn::verboseMessage, MessageLogger::info);
  QObject::connect(&useCase, &CopySharedLibrariesTargetDependsOn::debugMessage, MessageLogger::info);

  /*
   * Install the executable with its rpath allaways set to
   * the relative location where it is installed ($ORIGIN on Linux).
   * This way, we can check the use case.
   */
  const QString installedExecutable = makePath(destinationDir, "installedExecutable");
  REQUIRE( installExecutable(request.targetFilePath, installedExecutable) );

  SECTION("libraries have rpath set to the relative directory where installed - executable should run")
  {
    request.removeRpath = false;
    useCase.execute(request);

    REQUIRE( dirContainsTestSharedLibrary(destinationDir) );
    REQUIRE( dirContainsQt5Core(destinationDir) );

    if(checkRPath){
      REQUIRE( getTestSharedLibraryRunPath(destinationDir).entryAt(0).path() == QLatin1String(".") );
    }
    //REQUIRE( runExecutable(installedExecutable, {QLatin1String("25")}) );
  }

  SECTION("libraries have no rpath - executable should not run")
  {
    request.removeRpath = true;
    useCase.execute(request);

    REQUIRE( dirContainsTestSharedLibrary(destinationDir) );
    REQUIRE( dirContainsQt5Core(destinationDir) );

    if(checkRPath){
      REQUIRE( getTestSharedLibraryRunPath(destinationDir).isEmpty() );
    }
//     REQUIRE( !runExecutable(installedExecutable, {QLatin1String("25")}) );
  }
}

/*
TEST_CASE("sandbox_Windows")
{
  QTemporaryDir destinationDir;
  REQUIRE( destinationDir.isValid() );

  CopySharedLibrariesTargetDependsOnRequest request;

  request.destinationDirectoryPath = destinationDir.path();
  request.overwriteBehavior = OverwriteBehavior::Overwrite;
  request.removeRpath = false;

  /// \todo just for sandbox
  MessageLogger messageLogger;
  MessageLogger::setBackend<CMakeStyleMessageLogger>();

  CopySharedLibrariesTargetDependsOn useCase;

  QObject::connect(&useCase, &CopySharedLibrariesTargetDependsOn::message, MessageLogger::info);
  QObject::connect(&useCase, &CopySharedLibrariesTargetDependsOn::verboseMessage, MessageLogger::info);


  SECTION("Qt5Core")
  {
    request.targetFilePath = QString::fromLocal8Bit("/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Core.dll");
    useCase.execute(request);
  }

  SECTION("Qt5Cored")
  {
    request.targetFilePath = QString::fromLocal8Bit("/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Cored.dll");
    useCase.execute(request);
  }
}
*/
