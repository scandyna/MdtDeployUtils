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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies/GraphFile.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;
using namespace Mdt::DeployUtils::Impl::BinaryDependencies;

TEST_CASE("default constructed")
{
  GraphFile file;

  REQUIRE( file.isNull() );
}

TEST_CASE("fromLibraryName")
{
  auto file = GraphFile::fromLibraryName( QLatin1String("arbitraryFile.so") );

  REQUIRE( file.fileName() == QLatin1String("arbitraryFile.so") );
//   REQUIRE( file.absoluteDirectoryPath().isEmpty() );
  REQUIRE( !file.isNull() );
  REQUIRE( !file.hasAbsolutePath() );
}

TEST_CASE("fromQFileInfo")
{
  QFileInfo fi( QLatin1String("/path/to/some/arbitraryFile") );

  auto file = GraphFile::fromQFileInfo(fi);

  REQUIRE( file.fileName() == QLatin1String("arbitraryFile") );
//   REQUIRE( file.absoluteDirectoryPath() == makeAbsolutePath("/path/to/some") );
  REQUIRE( !file.isNull() );
  REQUIRE( file.hasAbsolutePath() );
}

TEST_CASE("AbsoluteFilePath")
{
  GraphFile file;
  REQUIRE( !file.hasAbsolutePath() );

  file.setAbsoluteFilePath( QString::fromLatin1("/opt/app") );

  REQUIRE( file.fileInfo().absoluteFilePath() == makeAbsolutePath("/opt/app") );
  REQUIRE( file.hasAbsolutePath() );
}

TEST_CASE("hasFileName")
{
  GraphFile file;

  SECTION("default constructed")
  {
    REQUIRE( !file.hasFileName() );
  }

  SECTION("library name")
  {
    file = GraphFile::fromLibraryName( QLatin1String("libA.so") );

    REQUIRE( file.hasFileName() );
  }
}

TEST_CASE("RPath")
{
  GraphFile file;

  RPath rpath;
  rpath.appendPath( QLatin1String("/opt/lib") );

  file.setRPath(rpath);

  REQUIRE( file.rPath().entriesCount() == 1 );
}

TEST_CASE("NotFound")
{
  GraphFile file;

  SECTION("a default constructed file is NOT not found")
  {
    REQUIRE( !file.isNotFound() );
  }

  SECTION("a absolute file path is found")
  {
    file.setAbsoluteFilePath( QString::fromLatin1("/opt/app") );

    REQUIRE( !file.isNotFound() );
  }

  SECTION("a file explicitly marked as not found is not found")
  {
    file.markAsNotFound();

    REQUIRE( file.isNotFound() );
  }
}

TEST_CASE("shouldNotBeRedistributed")
{
  GraphFile file;

  SECTION("default constructed, we don t know")
  {
    REQUIRE( !file.shouldNotBeRedistributed() );
  }

  SECTION("when explicitly marked, it should not be redistributed")
  {
    file.markAsNotToBeRedistributed();

    REQUIRE( file.shouldNotBeRedistributed() );
  }
}

TEST_CASE("hasBeenSearched")
{
  GraphFile file;

  SECTION("default constructed has not been searched")
  {
    REQUIRE( !file.hasBeenSearched() );
  }

  SECTION("file with absolute path has been searched")
  {
    file.setAbsoluteFilePath( QString::fromLatin1("/opt/app") );

    REQUIRE( file.hasBeenSearched() );
  }

  SECTION("file marked as not to be redistributed has been searched")
  {
    file.markAsNotToBeRedistributed();

    REQUIRE( file.hasBeenSearched() );
  }

  SECTION("file marked as not found has been searched")
  {
    file.markAsNotFound();

    REQUIRE( file.hasBeenSearched() );
  }
}

TEST_CASE("hasToBeRead")
{
  GraphFile file;

  SECTION("a default constructed file has not to be read (now)")
  {
    REQUIRE( !file.hasToBeRead() );
  }

  SECTION("a file that has its absolute path has to be read")
  {
    file.setAbsoluteFilePath( QString::fromLatin1("/opt/app") );

    REQUIRE( file.hasToBeRead() );
  }

  SECTION("a file that has been read has NOT to be read")
  {
    file.setAbsoluteFilePath( QString::fromLatin1("/opt/app") );
    file.markAsReaden();

    REQUIRE( !file.hasToBeRead() );
  }

  SECTION("a file that is not found has NOT to be read")
  {
    file.markAsNotFound();

    REQUIRE( !file.hasToBeRead() );
  }

  SECTION("a file marked as not to be redistributes has not to be read")
  {
    file.markAsNotToBeRedistributed();

    REQUIRE( !file.hasToBeRead() );
  }
}

TEST_CASE("isReaden")
{
  GraphFile file;

  SECTION("a default constructed file is not readen")
  {
    REQUIRE( !file.isReaden() );
  }

  SECTION("a file that has its absolute path")
  {
    file.setAbsoluteFilePath( QString::fromLatin1("/opt/app") );

    SECTION("if it has not been read it is NOT readen")
    {
      REQUIRE( !file.isReaden() );
    }

    SECTION("if it has been read it is readen")
    {
      file.markAsReaden();

      REQUIRE( file.isReaden() );
    }
  }

//   SECTION("a file marked as not found is processed")
//   {
//     file.markAsNotFound();
// 
//     REQUIRE( file.isProcessed() );
//   }
}
