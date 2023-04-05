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
. suite/expression/argument.sh
. suite/expression/return.sh
. suite/expression/break.sh
. suite/expression/continue.sh
. suite/expression/throw.sh
. suite/expression/import.sh
. suite/expression/snippet.sh

. suite/core/add.sh
. suite/core/subtract.sh
. suite/core/multiply.sh
. suite/core/divide.sh
. suite/core/modulo.sh
. suite/core/truncate.sh
. suite/core/and.sh
. suite/core/or.sh
. suite/core/not.sh
. suite/core/precedes.sh
. suite/core/succeeds.sh
. suite/core/equals.sh
. suite/core/write.sh
. suite/core/read.sh
. suite/core/delete.sh
. suite/core/query.sh
. suite/core/evaluate.sh
. suite/core/freeze.sh
. suite/core/thaw.sh
. suite/core/type.sh
. suite/core/cast.sh
. suite/core/get.sh
. suite/core/set.sh
. suite/core/unset.sh
. suite/core/merge.sh
. suite/core/length.sh
. suite/core/keys.sh
. suite/core/sort.sh
. suite/core/inputs.sh

conclude
