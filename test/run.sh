#!/bin/sh

# run.sh: test for expected behavior from a langnd program build
#
# Usage:
#     test/run.sh program

PROGRAM=$1

. test/assert.sh

if [ ! -f "$PROGRAM" -o ! -x "$PROGRAM" ]
then
    printf 'build artifact is not an executable file\n' 1>&2
    exit 1
fi

introduce

. test/suite/token/unknown.sh
. test/suite/token/whitespace.sh
. test/suite/token/string.sh
. test/suite/token/identifier.sh
. test/suite/token/keyword.sh

. test/suite/statement/null.sh
. test/suite/statement/boolean.sh
. test/suite/statement/number.sh
. test/suite/statement/string.sh
. test/suite/statement/assignment.sh
. test/suite/statement/invoke.sh
. test/suite/statement/branch.sh
. test/suite/statement/loop.sh

. test/suite/function/add.sh
. test/suite/function/subtract.sh
. test/suite/function/multiply.sh
. test/suite/function/divide.sh
. test/suite/function/modulo.sh
. test/suite/function/truncate.sh
. test/suite/function/and.sh
. test/suite/function/or.sh
. test/suite/function/not.sh
. test/suite/function/precedes.sh
. test/suite/function/succeeds.sh
. test/suite/function/equals.sh
. test/suite/function/write.sh
. test/suite/function/freeze.sh
. test/suite/function/type.sh
. test/suite/function/cast.sh
. test/suite/function/get.sh
. test/suite/function/set.sh
. test/suite/function/unset.sh
. test/suite/function/merge.sh
. test/suite/function/length.sh

conclude
