#!/bin/bash

# echo "COMP_WORDS: ${COMP_WORDS}"
# echo "COMP_CWORD: $COMP_CWORD"
# 
# cur="${COMP_WORDS[COMP_CWORD]}"
# prev="${COMP_WORDS[COMP_CWORD-1]}"
# 
# echo $cur
# echo $prev

_mdtdeployutils_completions()
{
  local currentPositionalArgument="$1"
  local options
  local commands

  case $currentPositionalArgument in

    "command")
      options="-h --help"
      commands="get-shared-libraries-target-depends-on copy-shared-libraries-target-depends-on"
      echo "reply based on: $options $commands"
      ;;

    "copy-source")
      echo "Source file"
      ls
      ;;

  esac

}

_mdtdeployutils_completions "$@"
