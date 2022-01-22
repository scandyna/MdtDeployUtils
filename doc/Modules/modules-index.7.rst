
All Modules
===========

.. toctree::
   :maxdepth: 1
   :caption: Contents:

   MdtSharedLibrariesDepencyHelpers.rst
   MdtDeployApplication.rst
   MdtInstallCMakeModules.rst


Message logging
===============

CMake supports some levels for logging, described in the
`message() <https://cmake.org/cmake/help/latest/command/message.html>`_
command.

The log level can be set by passing the ``--log-level`` option to the
`cmake <https://cmake.org/cmake/help/latest/manual/cmake.1.html>`_ executable,
or setting the `CMAKE_MESSAGE_LOG_LEVEL <https://cmake.org/cmake/help/latest/variable/CMAKE_MESSAGE_LOG_LEVEL.html#variable:CMAKE_MESSAGE_LOG_LEVEL>`_
variable.

Some parts in `Mdt Deploy Utils` modules are not written in CMake,
so they cannot reuse the ``--log-level`` argument passed to the `cmake` executable.

Instead, set the `CMAKE_MESSAGE_LOG_LEVEL` variable:

.. code-block:: cmake

  cmake -DCMAKE_MESSAGE_LOG_LEVEL=DEBUG ..
