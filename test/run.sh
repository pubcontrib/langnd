#!/bin/sh

# run.sh: test for expected behavior from a langnd program build
#
# Usage:
#     run.sh program

PROGRAM=$1

if [ ! -f ./run.sh ]
then
    printf 'working directory is not the same this script'"'"'s directory\n' 1>&2
    exit 1
fi

. ./assert.sh

crash()
{
    cleanup
    exit 1
}

trap crash INT QUIT ABRT
introduce

. suite/token/unknown.sh
. suite/token/whitespace.sh
. suite/token/string.sh
. suite/token/identifier.sh
. suite/token/keyword.sh

. suite/expression/null.sh
. suite/expression/boolean.sh
. suite/expression/number.sh
. suite/expression/string.sh
. suite/expression/list.sh
. suite/expression/map.sh
. suite/expression/function.sh
. suite/expression/assignment.sh
. suite/expression/invoke.sh
. suite/expression/branch.sh
. suite/expression/loop.sh
. suite/expression/catch.sh
. suite/expression/throw.sh
. suite/expression/return.sh
. suite/expression/break.sh
. suite/expression/continue.sh
. suite/expression/snippet.sh

. suite/function/add.sh
. suite/function/subtract.sh
. suite/function/multiply.sh
. suite/function/divide.sh
. suite/function/modulo.sh
. suite/function/truncate.sh
. suite/function/and.sh
. suite/function/or.sh
. suite/function/not.sh
. suite/function/precedes.sh
. suite/function/succeeds.sh
. suite/function/equals.sh
. suite/function/write.sh
. suite/function/read.sh
. suite/function/delete.sh
. suite/function/query.sh
. suite/function/freeze.sh
. suite/function/thaw.sh
. suite/function/type.sh
. suite/function/cast.sh
. suite/function/get.sh
. suite/function/set.sh
. suite/function/unset.sh
. suite/function/merge.sh
. suite/function/length.sh
. suite/function/keys.sh
. suite/function/sort.sh

conclude
