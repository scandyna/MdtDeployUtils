/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2011-2021 Philippe Steinmann.
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
#include "Mdt/DeployUtils/Platform.h"

using namespace Mdt::DeployUtils;

TEST_CASE("simpleSetGetTest")
{
  Platform pf(OperatingSystem::Windows, ExecutableFileFormat::Pe, Compiler::Gcc, ProcessorISA::X86_32);
  REQUIRE( pf.operatingSystem() == OperatingSystem::Windows );
  REQUIRE( pf.executableFileFormat() == ExecutableFileFormat::Pe );
  REQUIRE( pf.compiler() == Compiler::Gcc );
  REQUIRE( pf.processorISA() == ProcessorISA::X86_32 );
}

TEST_CASE("isNull")
{
  SECTION("default constructed")
  {
    Platform pf;
    REQUIRE( pf.isNull() );
  }

  SECTION("Valid")
  {
    Platform pf(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
    REQUIRE( !pf.isNull() );
  }
}

TEST_CASE("nativePlatformTest")
{
  Platform pf = Platform::nativePlatform();

  // Check that the correct OS was choosen
#ifdef Q_OS_LINUX
  REQUIRE(pf.operatingSystem() == OperatingSystem::Linux);
  REQUIRE(pf.executableFileFormat() == ExecutableFileFormat::Elf);
#elif defined Q_OS_WIN
  REQUIRE(pf.operatingSystem() == OperatingSystem::Windows);
  REQUIRE(pf.executableFileFormat() == ExecutableFileFormat::Pe);
#else
 #error "Current OS is not supported"
#endif

  // Check that correct compiler was choosen
#ifdef Q_CC_GNU
  REQUIRE(pf.compiler() == Compiler::Gcc);
#elif defined Q_CC_CLANG
  REQUIRE(pf.compiler() == Compiler::Clang);
#elif defined Q_CC_MSVC
  REQUIRE(pf.compiler() == Compiler::Msvc);
#else
 #error "Current compiler is not supported"
#endif

  // Check that correct processor was choosen
#ifdef Q_PROCESSOR_X86_32
  REQUIRE(pf.processorISA() == ProcessorISA::X86_32);
#elif defined Q_PROCESSOR_X86_64
  REQUIRE(pf.processorISA() == ProcessorISA::X86_64);
#else
 #error "Current processor is not supported"
#endif
}

TEST_CASE("comparison")
{
  SECTION("all matches")
  {
    Platform a(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
    Platform b = a;
    REQUIRE( a == b );
    REQUIRE( !(a != b) );
  }

  SECTION("OS different")
  {
    Platform a(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
    Platform b(OperatingSystem::Windows, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
    REQUIRE( !(a == b) );
    REQUIRE( a != b );
  }

  SECTION("executable file format differs")
  {
    Platform a(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
    Platform b(OperatingSystem::Linux, ExecutableFileFormat::Pe, Compiler::Gcc, ProcessorISA::X86_64);
    REQUIRE( a != b );
  }

  SECTION("processor ISA differs")
  {
    Platform a(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
    Platform b(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_32);
    REQUIRE( a != b );
  }

  SECTION("compiler differs (see remarks in the class documentation)")
  {
    Platform a(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
    Platform b(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Clang, ProcessorISA::X86_64);
    REQUIRE( a == b );
  }
}
