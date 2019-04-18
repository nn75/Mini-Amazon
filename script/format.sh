#!/bin/bash

if ! [ -x "$(command -v clang-format)" ]; then
    echo 'Error: clang-format is not installed.' >&2
    echo '`brew install clang-format` first' >&2
    exit 1
fi

if [ $# != 1 ] ; then
	echo "USAGE: $0 <target_dir>"
	exit -1;
fi

for f in $(ls $1/*.h $1/*.cpp); do
    if [[ "$f" == *.pb.* ]]; then
        continue
    fi
    echo "Formating: " $f
    tmpfile=$(mktemp /tmp/clang-format.XXXXXX)
    clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" $f > $tmpfile
    mv $tmpfile $f
done

