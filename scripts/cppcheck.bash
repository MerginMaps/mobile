#!/usr/bin/env bash

set -eu

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Using CPPCHECK"
cppcheck --version


LOG_FILE=/tmp/cppcheck_mminput.txt

rm -f ${LOG_FILE}
echo "Checking... ${SCRIPT_DIR}/.."

# we have many functions used in QML only, so suppress unusedFunction
cppcheck --library=qt.cfg \
         --inline-suppr \
         --template='{file}:{line},{severity},{id},{message}' \
         --enable=all \
         --inconclusive \
         --suppress="unusedFunction" \
         --std=c++17 \
         -i app/test \
         ${SCRIPT_DIR}/../app \
         ${SCRIPT_DIR}/../core \
         >>${LOG_FILE} 2>&1 &

PID=$!
while kill -0 $PID 2>/dev/null; do
    printf "."
    sleep 1
done
echo " done"
if ! wait $PID; then
    echo "cppcheck failed"
    exit 1
fi

ret_code=0

cat ${LOG_FILE} | grep -v -e "syntaxError," -e "cppcheckError," > ${LOG_FILE}.tmp
mv ${LOG_FILE}.tmp ${LOG_FILE}

ERROR_CATEGORIES=("clarifyCalculation" "duplicateExpressionTernary" "redundantCondition" "postfixOperator" "functionConst" "unsignedLessThanZero" "duplicateBranch" "missingOverride")

for category in "style" "performance" "portability"; do
    if grep "${category}," ${LOG_FILE} >/dev/null; then
        echo "******************************************************************************************"
        echo "INFO: Issues in '${category}' category found, but not considered as making script to fail:"
        echo "******************************************************************************************"
        grep "${category}," ${LOG_FILE} | grep -v $(printf -- "-e %s, " "${ERROR_CATEGORIES[@]}")
        echo ""
    fi
done

for category in "error" "warning" "${ERROR_CATEGORIES[@]}"; do
    if test "${category}" != ""; then
        if grep "${category}," ${LOG_FILE}  >/dev/null; then
            echo "******************************************************************************************"
            echo "ERROR: Issues in '${category}' category found:"
            echo "******************************************************************************************"
            grep "${category}," ${LOG_FILE}
            echo ""
            echo "******************************************************************************************"
            echo "${category} check failed !"
            ret_code=1
        fi
    fi
done

if [ ${ret_code} = 0 ]; then
    echo "******************************************************************************************"
    echo "cppcheck succeeded"
fi

exit ${ret_code}