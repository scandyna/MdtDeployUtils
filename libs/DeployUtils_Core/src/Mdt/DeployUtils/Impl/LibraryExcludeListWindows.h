/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_EXCLUDE_LIST_WINDOWS_H
#define MDT_DEPLOY_UTILS_IMPL_LIBRARY_EXCLUDE_LIST_WINDOWS_H

#include <QStringList>
#include <QLatin1String>

/*! \internal List of library to not deploy
 *
 * A good starting point can be found on Wikipedia:
 * https://en.wikipedia.org/wiki/Microsoft_Windows_library_files
 */
static
const QStringList libraryExcludelistWindows = {
  QLatin1String("HAL.DLL"),
  QLatin1String("NTDLL.DLL"),
  QLatin1String("KERNEL32.DLL"),
  QLatin1String("GDI32.DLL"),
  QLatin1String("USER32.DLL"),
  QLatin1String("COMCTL32.DLL"),
  QLatin1String("WS2_32.DLL"),
  QLatin1String("ADVAPI32.DLL"),
  QLatin1String("NETAPI32.DLL"),
  QLatin1String("SHSCRAP.DLL"),
  QLatin1String("WINMM.DLL"),
  QLatin1String("MSVCRT.DLL"),
  QLatin1String("mpr.DLL"),
  QLatin1String("ole32.DLL"),
  QLatin1String("shell32.DLL"),
  QLatin1String("version.DLL"),
  QLatin1String("crypt32.DLL"),
  QLatin1String("dnsapi.DLL"),
  QLatin1String("iphlpapi.DLL"),
  QLatin1String("opengl32.DLL"),
  QLatin1String("UxTheme.DLL"),
  QLatin1String("dwmapi.DLL"),
  QLatin1String("imm32.DLL"),
  QLatin1String("oleaut32.DLL"),
  QLatin1String("Secur32.DLL"),
  QLatin1String("odbc32.DLL"),
  QLatin1String("shfolder.DLL"),
  QLatin1String("wsock32.DLL")
};

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_EXCLUDE_LIST_WINDOWS_H
