#/usr/bin/env bash
# complete -W "-h --help get-shared-libraries-target-depends-on copy-shared-libraries-target-depends-on" mdtdeployutils

_mdtdeployutils_completions()
{
#   COMPREPLY+=("get-shared-libraries-target-depends-on")
#   COMPREPLY+=("copy-shared-libraries-target-depends-on")
#   COMPREPLY+=("-h")
#   COMPREPLY+=("--help")

#   echo "#COMP_WORDS[@] ${#COMP_WORDS[@]}"
#   echo "COMP_WORDS[1]: ${COMP_WORDS[1]}"
#   echo "COMP_WORDS[2]: ${COMP_WORDS[2]}"

#   COMPREPLY=(
#     $(compgen -W "get-shared-libraries-target-depends-on copy-shared-libraries-target-depends-on -h --help" "${COMP_WORDS[1]}")
#   )

  cur="${COMP_WORDS[COMP_CWORD]}"
  prev="${COMP_WORDS[COMP_CWORD-1]}"

#   echo "prev: $prev"
#   echo "cur: $cur"
#   echo "COMP_CWORD: $COMP_CWORD"
#   echo "#COMP_WORDS[@]: ${#COMP_WORDS[@]}"
#   echo "COMP_POINT: ${COMP_POINT}"
#   echo "#COMP_LINE: ${#COMP_LINE}"
  
#   # Arguments available for the main command
#   if [ "${#COMP_WORDS[@]}" == "2" ]
#   then
#     COMPREPLY=($(compgen -W "get-shared-libraries-target-depends-on copy-shared-libraries-target-depends-on -h --help" -- "${COMP_WORDS[1]}"))
#     return
#   fi

  # Arguments available for the get-shared-libraries-target-depends-on command
  if [ "${#COMP_WORDS[@]}" == "3" ] && [ "${COMP_WORDS[1]}" == "get-shared-libraries-target-depends-on" ]
  then
    COMPREPLY=($(compgen -W "-h --help" -- "${COMP_WORDS[2]}"))
    return
  fi

  # Arguments available for the copy-shared-libraries-target-depends-on command
#   if [ "${#COMP_WORDS[@]}" == "3" ] && [ "${COMP_WORDS[1]}" == "copy-shared-libraries-target-depends-on" ]
#   then
#     COMPREPLY=($(compgen -A file -- "${COMP_WORDS[2]}"))
#     COMPREPLY+=($(compgen -W "-h --help" -- "${COMP_WORDS[2]}"))
#     return
#   fi

#   if [ "${#COMP_WORDS[@]}" == "4" ] && [ "${COMP_WORDS[1]}" == "copy-shared-libraries-target-depends-on" ]
#   then
#     COMPREPLY=($(compgen -A directory -- "${COMP_WORDS[3]}"))
#     COMPREPLY+=($(compgen -W "-h --help" -- "${COMP_WORDS[3]}"))
#     return
#   fi

  # Options and arguments available for the main command
  if [ $COMP_CWORD -eq 1 ]
  then
    local options="-h --help"
    local commands="get-shared-libraries-target-depends-on copy-shared-libraries-target-depends-on"

    COMPREPLY=($(compgen -W "$options $commands" -- "$cur"))
    return
  fi

  # Options and arguments available for the copy-shared-libraries-target-depends-on command
  if [ $COMP_CWORD -ge 2 ] && [ "${COMP_WORDS[1]}" == "copy-shared-libraries-target-depends-on" ]
  then
    local options="-h --help"

    # Nothing have been typed, suggest options and possible entry for first positional argument
    if [ $COMP_CWORD -eq 2 ]
    then
      COMPREPLY=($(compgen -W "$options" -- "$cur"))
      COMPREPLY+=($(compgen -A file -- "$cur"))
      return
    fi

    # A option is about to be typed
    if [[ $cur == -* ]]
    then
      COMPREPLY=($(compgen -W "$options" -- "$cur"))
      return
    fi

    # At least a option or a positional argument have been typed
    if [ $COMP_CWORD -gt 2 ]
    then
      if [[ $prev == -* ]]
      then
        # We are at first positional argument
        COMPREPLY=($(compgen -A file -- "$cur"))
      else
        # Hmm... FIXME
        COMPREPLY=($(compgen -A directory -- "$cur"))
      fi
    fi
  fi

}

complete -F _mdtdeployutils_completions mdtdeployutils
