/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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
 ****************************************************************************/
#include <QApplication>
#include <QTimer>
#include <QDebug>

int main(int argc, char **argv)
{
  qDebug() << "Qt Widgets app starting ...";

  QApplication app(argc, argv);
  QTimer::singleShot(500, &app, &QApplication::quit);

  const int retval = app.exec();

  qDebug() << "Qt Widgets app, exec finished";

  return retval;
}
