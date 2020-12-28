
# Impl header files

Should be in a Impl sub-directory
(should not be suffixed _Impl)
Should not be installed

# Script

See: https://argbash.io/
And also: https://argbash.readthedocs.io/en/stable/usage.html#bash-completion

Question:
Which positional argument I am currently processing ?
The first, the second, ... ?
positional_argument_index_of_current_word()

# Args: list of words, --help file -o true directory
extract_positional_arguments()

Note: with --file some-file (option value) this is not so easy..

Should explore re-using the parser..
see apt-get <-- This calls a function..
Maybe git ?

# NOTE: path to mayapp is $1 whan called from completion ($0 is bash)
local myapp-executable="$1"
COMPREPLY=$(compgen -W $(myapp-executable --complete-command-line-bash CURRENT_WORD_CURSOR THE_WORD_ARRAY) )


-->> What about to capture TAB ??
seems that std is <enter>
See: https://stackoverflow.com/questions/13971298/how-do-i-accept-tab-as-input-from-stdin-to-autofill-text/13978270

Other idea: https://github.com/rsalinas/xqtcmdline/blob/master/xqtparser.cpp

# tools executable

See how to link to Qt statically

Try also link statically to Mdt libs (conan option shared for those libs)

Document BUILD.md for conan options to link to static libs, like:
 - MdtApplication

# Licences

Command-line tools should be GPL

C++ library should be LGPL

CMake modules are BSD 3-Clause

# Conan file

settings should not allways depend on compiler etc..

See how to create a conanfile for a tool !
 --> See https://docs.conan.io/en/latest/devtools.html


# Problems common to all tools/APIs

Should be able to handle target architectures
that are different from the host one
(crosscompiling)

# Command line tools

Provide stand-alone executables

Document README how to install them

Also, specify that they are not required to be installed
when using CMake API + conan

## Output results

Try a simple array on stdout
(errors are then put to stderr)

See limits on platforms/shells

See the doc of QProcess

Add option for array output format:
 - Space separated quoted strings:
   "/path/to/libA.so" "/path/to some/libB.so"
 - `;` separated strings:
   "/path/to/libA.so;/path/to some/libB.so"

Note: only those 2 formats, do not generalise sepearator option

--output-format  (no short option, -f is ambigous, force)

Maybe, option to provide a file:
--output-file "file"

## Auto-completion

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
