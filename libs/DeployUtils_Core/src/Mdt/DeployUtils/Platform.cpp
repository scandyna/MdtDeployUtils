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
#include "Platform.h"
#include <QtGlobal>

namespace Mdt{ namespace DeployUtils{

// Platform::Platform(OperatingSystem os, Compiler compiler, Processor processor) noexcept
//  : mOperatingSystem(os),
//    mCompiler(compiler),
//    mProcessor(processor)
// {
// }

OperatingSystem Platform::nativeOperatingSystem() noexcept
{
#ifdef Q_OS_LINUX
  return OperatingSystem::Linux;
#elif defined Q_OS_WIN
  return OperatingSystem::Windows;
#else
 #error "Current OS is not supported"
#endif // OS
}

ExecutableFileFormat Platform::nativeExecutableFileFormat() noexcept
{
#ifdef Q_OS_LINUX
  return ExecutableFileFormat::Elf;
#elif defined Q_OS_WIN
  return ExecutableFileFormat::Pe;
#else
 #error "Current OS is not supported"
#endif // OS
}

Compiler Platform::nativeCompiler() noexcept
{
#ifdef Q_CC_GNU
  return Compiler::Gcc;
#elif defined Q_CC_CLANG
  return Compiler::Clang;
#elif defined Q_CC_MSVC
  return Compiler::Msvc;
#else
 #error "Current compiler is not supported"
#endif // Compiler
}

ProcessorISA Platform::nativeProcessorISA() noexcept
{
#ifdef Q_PROCESSOR_X86_32
  return ProcessorISA::X86_32;
#elif defined Q_PROCESSOR_X86_64
  return ProcessorISA::X86_64;
#else
 #error "Current processor is not supported"
#endif // Compiler
}


}} // namespace Mdt{ namespace DeployUtils{
