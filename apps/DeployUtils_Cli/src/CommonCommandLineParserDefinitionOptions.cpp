/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ***********************************************************************************************/
#include "CommonCommandLineParserDefinitionOptions.h"
#include <QLatin1String>

using namespace Mdt::CommandLineParser;

ParserDefinitionOption CommonCommandLineParserDefinitionOptions::makeCompilerLocationOption() noexcept
{
  const QString compilerLocationOptionDescription = tr(
    "Location of the compiler.\n"
    "This option is used to locate compiler libraries to distribute, like, for example, LIBSTDC++-6.DLL for MinGW or MSVCP140.DLL for MSVC.\n"
    "Possible values are from-env=<var-name>, vc-install-dir=<path-to-dir>, compiler-path=<path>\n"
    "from-env=<var-name>: will use given environment variable. Note: the var-name cannot be PATH. Example:\n"
    " from-env=VcInstallDir\n"
    "vc-install-dir=<path-to-dir>: specify the path to MSVC install directory. Example:\n"
    " vc-install-dir=\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\VC\"\n"
    "compiler-path=<path>: full path to the compiler. This can be used with CMAKE_CXX_COMPILER"
  );
  ParserDefinitionOption compilerLocationOption( QLatin1String("compiler-location"), compilerLocationOptionDescription );
  compilerLocationOption.setValueName( QLatin1String("location") );

  return compilerLocationOption;
}

ParserDefinitionOption CommonCommandLineParserDefinitionOptions::makeSearchPrefixPathListOption() noexcept
{
  /// \todo fix doc
  const QString description = tr(
    "Semicolon separated list of path where to find the shared libraries.\n"
    "Each path represents the installation prefix of a library, for example: /opt/Qt5/5.14.2/gcc_64.\n"
    "This is mainly useful on platforms that don't have RPATH support, such as Windows.\n"
    "On platforms that supports RPATH, some shared libraries cannot be found on some cases, "
    "so this option should also be provided.\n"
    "For more informations about why, see https://scandyna.gitlab.io/mdt-cmake-modules/Modules/MdtRuntimeEnvironment.html."
  );
  ParserDefinitionOption option( QLatin1String("search-prefix-path-list"), description );
  option.setValueName( QLatin1String("path-list") );

  return option;
}
