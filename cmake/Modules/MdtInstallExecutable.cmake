# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtInstallExecutable
# --------------------
#
# .. contents:: Summary
#    :local:
#
# Functions
# ^^^^^^^^^
#
# .. command:: mdt_install_executable
#
# Install a executable::
#
#   mdt_install_executable(
#     TARGET <target>
#     RUNTIME_DESTINATION <dir>
#     
#     [COPY_DEPENDENT_SHARED_LIBRARIES [TRUE|FALSE]]
#     
#     [REMOVE_RPATH [TRUE|FALSE]]
#   )



# TODO : if subdir not 1 level, document to set RPATH manually
