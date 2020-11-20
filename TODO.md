
# Command line tools

Provide stand-alone executables

Document README how to install them

Also, specify that they are not required to be installed
when using CMake API + conan

# CMake API

Function that finds required tools, like ldd:
mdt_find_deploy_utils_tools()

mdt_find_ldd()

mdt_find_deploy_utils_executables()


Option to not install libraries:
 - Comming from some paths
 - That have no RPATH set  <-- this is strange

mdt_get_shared_libraries_target_depends_on()

mdt_install_shared_libraries_target_depends_on()

find/install Qt plugins

mdt_install_qt5_platform_plugins()

find/install Mdt plugins

mdt_install_executable()

mdt_deploy_application()
