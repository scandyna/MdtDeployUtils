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

  # Arguments available for the main parser
  if [ "${#COMP_WORDS[@]}" == "2" ]
  then
    COMPREPLY=($(compgen -W "get-shared-libraries-target-depends-on copy-shared-libraries-target-depends-on -h --help" -- "${COMP_WORDS[1]}"))
    return
  fi

  # Arguments available for the get-shared-libraries-target-depends-on command
  if [ "${#COMP_WORDS[@]}" == "3" ] && [ "${COMP_WORDS[1]}" == "get-shared-libraries-target-depends-on" ]
  then
    COMPREPLY=($(compgen -W "-h --help" -- "${COMP_WORDS[2]}"))
    return
  fi

  # Arguments available for the copy-shared-libraries-target-depends-on command
  if [ "${#COMP_WORDS[@]}" == "3" ] && [ "${COMP_WORDS[1]}" == "copy-shared-libraries-target-depends-on" ]
  then
    COMPREPLY=($(compgen -A directory))
    COMPREPLY+=($(compgen -W "-h --help" -- "${COMP_WORDS[2]}"))
    return
  fi

}

complete -F _mdtdeployutils_completions mdtdeployutils
