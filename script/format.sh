#!/bin/bash

if ! [ -x "$(command -v clang-format)" ]; then
    echo 'Error: clang-format is not installed.' >&2
    echo '`brew install clang-format` first' >&2
    exit 1
fi

tmpfile=$(mktemp /tmp/clang-format.XXXXXX)
clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" $1 > $tmpfile
mv $tmpfile $1
