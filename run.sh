#!/bin/sh

die () {
    echo >&2 "$@"
    exit 1
}

[ "$#" -eq 1 ] || die "args: a, ad, l, ld, g, gd, all, alld"

if [ $1 == 'a' ]
then
	./scripts/run-apple.sh
elif [ $1 == 'ad' ]
then
	./scripts/run-apple-debug.sh
elif [ $1 == 'l' ]
then
	./scripts/run-llvm.sh
elif [ $1 == 'ld' ]
then
	./scripts/run-llvm-debug.sh
elif [ $1 == 'g' ]
then
	./scripts/run-gcc.sh
elif [ $1 == 'gd' ]
then
	./scripts/run-gcc-debug.sh
elif [ $1 == 'all' ]
then
	./scripts/run-all.sh
elif [ $1 == 'alld' ]
then
	./scripts/run-all-debug.sh
fi
