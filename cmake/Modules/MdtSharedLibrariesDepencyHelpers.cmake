# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtSharedLibrariesDepencyHelpers
# --------------------------------
#
# .. contents:: Summary
#    :local:
#
# Functions
# ^^^^^^^^^
#
# .. command:: mdt_get_shared_libraries_target_depends_on
#
# Add a string of variables to the end of the ``ENVIRONMENT`` property of a test::
#
#   mdt_get_shared_libraries_target_depends_on(test variables_string)
#
# Example:
#
# .. code-block:: cmake
#
#   mdt_get_shared_libraries_target_depends_on(SomeTest "var1=value1;var2=value2")
#
# The `VARIABLES_STRING` is not parsed, but appended as is to the ``ENVIRONMENT`` property of the test.
# See next sections to understand this choice.

