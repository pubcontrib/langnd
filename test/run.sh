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

if [ ! -f "$PROGRAM" -o ! -x "$PROGRAM" ]
then
    printf 'build artifact is not an executable file\n' 1>&2
    exit 1
fi

. ./assert.sh

introduce

. suite/token/unknown.sh
. suite/token/whitespace.sh
. suite/token/string.sh
. suite/token/identifier.sh
. suite/token/keyword.sh

. suite/statement/null.sh
. suite/statement/boolean.sh
. suite/statement/number.sh
. suite/statement/string.sh
. suite/statement/assignment.sh
. suite/statement/invoke.sh
. suite/statement/branch.sh
. suite/statement/loop.sh

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
. suite/function/freeze.sh
. suite/function/type.sh
. suite/function/cast.sh
. suite/function/get.sh
. suite/function/set.sh
. suite/function/unset.sh
. suite/function/merge.sh
. suite/function/length.sh

conclude
