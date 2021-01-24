# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtSharedLibrariesDepencyHelpers
# --------------------------------
#
# .. contents:: Summary
#    :local:
#
# Introduction
# ^^^^^^^^^^^^
#
# Install libs to opt/xy
#
# Install libs to /usr/lib/${arch}
#
# Explore:
# - If source and destination library is the same, do not touch
#
#
# Also, on Linux, if executable has not RPATH, what to do:
#  Not support ?
#  PREFIX path ?
#  NOTE: no RPATH does NOT mean no dependecies informations
#
# Examples::
#
#   # Source
#   ~/opt/qt/Qt5/5.14.2/gcc_64/lib/libQt5Core.so
#
#   # Destination
#   ~/opt/myApp/lib/libQt5Core.so
#
#
#   # Source
#   /usr/lib/x86_64-linux-gnu/libQt5Core.so
#
#   # Destination
#   ~/opt/myApp/lib/libQt5Core.so
#
#
#   # Source
#   /usr/lib/x86_64-linux-gnu/libQt5Core.so
#
#   # Destination
#   /usr/lib/x86_64-linux-gnu/libQt5Core.so
#
#
# The main goal to copy shared libraries a executable depends on
# is application standalone deployment.
#
# NOTE: install 2 (or more) apps with same dependencies
#
# Copy shared libraries a target depends on
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# .. command:: mdt_copy_shared_libraries_target_depends_on
#
# Copy the shared libraries ``target`` depends on to the destination directory::
#
#   mdt_copy_shared_libraries_target_depends_on(
#     TARGET <target>
#     DESTINATION <dir>
#     [OVERWRITE_BEHAVIOR [KEEP|OVERWRITE|FAIL]]
#     [REMOVE_RPATH [TRUE|FALSE]]
#   )
#
# The shared libraries ``target`` depends on are copied to the location specified by ``DESTINATION``.
#
# If the source and destination locations for a shared library are the same,
# it will not be copied, and its rpath informations will not be changed at all,
# and this regardless of ``REMOVE_RPATH`` and ``OVERWRITE_BEHAVIOR``.
# (The only descent case this happens is a UNIX system wide installation,
# for a target that depends on shared libraries allready installed system wide).
#
# If a shared library allready exists at the destination location,
# but its not the same as the source (source and destination locations are different),
# the behavior is defined by ``OVERWRITE_BEHAVIOR``.
# If ``OVERWRITE_BEHAVIOR`` is ``KEEP``, the destination library will not be changed at all.
# If ``OVERWRITE_BEHAVIOR`` is ``OVERWRITE``, the destination library will replaced.
# If ``OVERWRITE_BEHAVIOR`` is ``FAIL``, a fatal error is thrown.
# By default, the ``OVERWRITE_BEHAVIOR`` is ``FAIL``.
#
# By default, on platform that supports rpath,
# the rpath informations is set to ``$ORIGIN`` for each shared library that has been  copied.
# If ``REMOVE_RPATH`` is ``TRUE``, the rpath informations are removed for each shared library that has been copied.
#
# Example:
#
# .. code-block:: cmake
#
#   add_executable(myApp myApp.cpp)
#   target_link_libraries(myApp PRIVATE Qt5::Core)
#   target_link_libraries(myApp PRIVATE Mdt0::PlainText)
#
#   mdt_copy_shared_libraries_target_depends_on(
#     TARGET myApp
#     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/myApp/lib"
#     OVERWRITE_BEHAVIOR OVERWRITE
#   )
#
# The shared libraries the ``myApp`` executable depends on are copied to ``"${CMAKE_CURRENT_BINARY_DIR}/myApp/lib"``.
#
# On platform that supports rpath,
# the rpath informations is set to ``$ORIGIN`` for each shared library that has been copied.
#
#
# Install shared libraries a target depends on
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# .. command:: mdt_install_shared_libraries_target_depends_on
#
# Specify rules to install the shared libraries ``target`` depends on::
#
#   mdt_install_shared_libraries_target_depends_on(
#     TARGET <target>
#     RUNTIME_DESTINATION <dir>
#     LIBRARY_DESTINATION <dir>
#     [INSTALL_IS_UNIX_SYSTEM_WIDE [TRUE|FALSE]]
#   )
#
# The shared libraries ``target`` depends on are installed to ``CMAKE_INSTALL_PREFIX``,
# into a appropriate subdirectory.
#
# On UNIX, the subdirectory is the one specified by ``LIBRARY_DESTINATION``.
# On Windows, the subdirectory is the one specified by ``RUNTIME_DESTINATION``.
#
# By default, the rpath informations is set to ``$ORIGIN`` for each shared library on platform that supports it.
# If ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ``TRUE``, the rpath informations are removed for each shared library on platform that supports it.
#
# If the source and destination locations for a shared library are the same,
# it will not be copied, and its rpath informations will not be changed at all.
# This can happen on UNIX systems when ``target`` depends on shared libraries installed system wide,
# and ``CMAKE_INSTALL_PREFIX`` refers to a system wide installation (i.e. ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ``TRUE``).
#
# If ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ``TRUE``,
# and a shared library ``target`` depends on allready exists at the destination location,
# but it is not the same (source and destination locations are different), a fatal error is thrown.
# The reason is to prevent erasing system wide installed libraries with a other version,
# which could corrupt the whole system.
# On the other hand, erasing libraries while creating a standalone application
# will happen often, and is not a problem.
#
# Example:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   add_executable(myApp myApp.cpp)
#   target_link_libraries(myApp PRIVATE Qt5::Core)
#   target_link_libraries(myApp PRIVATE Mdt0::PlainText)
#
#   mdt_install_shared_libraries_target_depends_on(
#     TARGET myApp
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#   )
#
# To create a standalone application on Linux, the ``CMAKE_INSTALL_PREFIX`` could be ``~/opt/myapp``.
# In that case, each shared library will be installed to ``~/opt/myapp/lib/``.
# The rpath informations will also be set to ``$ORIGIN`` for each installed shared library.
# If a shared library comes from a system wide location, for example ``/usr/lib/x86_64-linux-gnu/libQt5Core.so``,
# it will also be installed, and its rpath informations will be set to ``$ORIGIN``.
#
# On Linux, the application could also be installed system wide.
# In that case, the ``CMAKE_INSTALL_PREFIX`` will be ``/usr``.
# In that case, each shared library will be installed to ``/usr/lib/``,
# or ``/usr/lib/x86_64-linux-gnu/`` on a Gcc based multi-arch Debian.
# The rpath informations will also be removed for each installed shared library.
# If a shared library comes from a system wide location, for example ``/usr/lib/x86_64-linux-gnu/libQt5Core.so``,
# in which case its source and destination location are the same,
# it will (of course) not be installed, and its rpath informations will not be changed at all.
#
# On Windows, the shared libraries the ``myApp`` executable depends on are installed to ``${CMAKE_INSTALL_PREFIX}/bin``.
#
