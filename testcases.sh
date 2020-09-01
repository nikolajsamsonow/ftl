#!/bin/bash

WORK_DIR=work_dir
LINKERS=(gold ld lld)
FUZZERS=(afl aflsmart aflpp)
TESTCASES=$(ls testcases)

mkdir -p $WORK_DIR

for testcase in ${TESTCASES[@]}
do
	(cd testcases/${testcase};./build.sh)

	for linker in ${LINKERS[@]}
	do
		for fuzzer in ${FUZZERS[@]}
		do
			mkdir -p ${WORK_DIR}/${fuzzer}_${linker}_${testcase}
			cp -r testcases/${testcase}/* ${WORK_DIR}/${fuzzer}_${linker}_${testcase}/
		done
	done
done	

