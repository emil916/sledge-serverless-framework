#!/bin/bash

utility="clang-format"
utility_version="$("$utility" --version 2>/dev/null)" || {
  echo "$utility not found in path!"
  exit 1
}

regex="version ([0-9]+).([0-9]+).([0-9]+)"
declare -i major=0
declare -i minor=0
declare -i patch=0
declare -i required_major=9
declare -i required_minor=0
declare -i required_patch=0

if [[ "$utility_version" =~ $regex ]]; then
  major="${BASH_REMATCH[1]}"
  minor="${BASH_REMATCH[2]}"
  patch="${BASH_REMATCH[3]}"
fi

if ((major < required_major)) || ((minor < required_minor)) || ((patch < required_patch)); then
  echo "$utility $required_major.$required_minor.$required_patch required, but is $major.$minor.$patch"
  exit 1
fi

find ./runtime -type f -path "./*.[ch]" |                             # Match all *.c and *.h files in ./runtime
  grep --invert-match -E "./runtime/jsmn/*|./runtime/http-parser/*" | # Excluding those in the jsmn or http-parser submodules
  xargs clang-format -i                                               # And format them with clang-format
