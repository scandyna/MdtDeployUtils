// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "TestUtils.h"
#include <QLatin1Char>
#include <QLatin1String>
#include <QDir>
#include <cassert>

Mdt::DeployUtils::BuildType currentBuildType()
{
  using Mdt::DeployUtils::BuildType;

#ifdef NDEBUG
  return BuildType::Release;
#else
  return BuildType::Debug;
#endif
}

Mdt::DeployUtils::Platform getNonNativePlatform()
{
  using namespace Mdt::DeployUtils;

  Platform nativePlatform = Platform::nativePlatform();

  if( nativePlatform.executableFileFormat() == ExecutableFileFormat::Elf ){
    return Platform( OperatingSystem::Windows, ExecutableFileFormat::Pe, nativePlatform.compiler(), nativePlatform.processorISA() );
  }
  return Platform( OperatingSystem::Linux, ExecutableFileFormat::Elf, nativePlatform.compiler(), nativePlatform.processorISA() );
}

QStringList qStringListFromUtf8Strings(const std::vector<std::string> & args)
{
  QStringList arguments;

  for(const auto & arg : args){
    arguments.append( QString::fromStdString(arg) );
  }

  return arguments;
}

QString generateStringWithNChars(int n)
{
  assert( n > 0 );

  QString str;

  for(int i=0; i<n; ++i){
    str += QString::number(i%10);
  }
  assert( str.length() == n );

  return str;
}

QStringList getTestPrefixPath(const char * const prefixPath)
{
  return QString::fromLocal8Bit(prefixPath).split( QLatin1Char(',') );
}

bool containsLibrary(const QStringList & libraries, const QString & libraryName)
{
//   const Mdt::DeployUtils::LibraryName searchedLibraryName(libraryName);
//
//   for(const QString & library : libraries){
//     const QFileInfo libraryFile(library);
//     const Mdt::DeployUtils::LibraryName currentLibraryName( libraryFile.fileName() );
//     if( currentLibraryName.nameWithoutDebugSuffix() == searchedLibraryName.nameWithoutDebugSuffix() ){
//       return true;
//     }
//   }

  const auto getLibraryName = [](const QString & library){
    return library;
  };

  return containsLibrary(libraries, getLibraryName, libraryName);
}

bool dirContainsLibrary(const QTemporaryDir & dir, const QString & libraryName)
{
  assert( dir.isValid() );

  return containsLibrary( QDir( dir.path() ).entryList(QDir::Files), libraryName );
}

bool containsQt5Core(const QStringList & libraries)
{
  return containsLibrary(libraries, QLatin1String("Qt5Core"));
}

bool dirContainsQt5Core(const QTemporaryDir & dir)
{
  return dirContainsLibrary( dir, QLatin1String("Qt5Core") );
}

bool containsTestSharedLibrary(const QStringList & libraries)
{
  return containsLibrary( libraries, QLatin1String("testSharedLibrary") );
}

bool dirContainsTestSharedLibrary(const QTemporaryDir & dir)
{
  return dirContainsLibrary( dir, QLatin1String("testSharedLibrary") );
}
