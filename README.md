### 1. Building fuzzers

#### 1.1 Fuzzers 

Please refer to [AFL](https://github.com/google/AFL/blob/master/docs/INSTALL)'s, [AFLSmart](https://github.com/aflsmart/aflsmart#installation
)'s and [AFL++](https://github.com/AFLplusplus/AFLplusplus#building-and-installing-afl)'s documentation.

#### 1.2 Yet another mutator

To compile `yet_another_mutator` run the following lines:

```
cd yet_another_mutator
mkdir build && cd build
cmake ../
make
```

### 2. Download, patch and build the linkers

#### 2.1. [OPTIONAL] Apply patches to Gold and LLD

`patch binutils/gold/output.cc patches/gold.patch` and `patch llvm/llvm/lib/Support/FileOutputBuffer.cpp patches/lld.patch` for Gold and LLD respectively.

#### 2.2. [OPTIONAL] Apply splitter.c

In `binutils/gold/main.cc`, `binutils/ld/ldmain.c` and `llvm/lld/tools/lld/lld.cpp` rename `main` to `the_real_main` and then append contents of `splitter.c` to the modified files.

#### 2.3. Downloading and building

TODO

### 3. Generate testcases

Just type `./testcases.sh`. Configurations for fuzzing will apper in `work_dir`.
If you want to add a new configuration, create a directory in `testcases`, put your testcase in there and write building rules to `build.sh`.

### 4. Fuzzing

#### 4.1. [OPTIONAL] Prepare for fuzzing

AFL may ask you to execute the following lines:

```
echo core >/proc/sys/kernel/core_pattern
echo performance | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

#### 4.2. Fuzz

Run `screen timeout -s INT 12h <path_to_afl-fuzz> -i in -o sync_dir -M fuzzer01 -- <path_to_linker> -o a.out -dynamic-linker /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o @@ /usr/lib/x86_64-linux-gnu/libc.so /usr/lib/x86_64-linux-gnu/crtn.o` to spawn a master process. Replace `-M` to `-S` to spawn a slave process. **Don't forget to increase the number for each instance in `fuzzer01`**. 

*When fuzzing LLD be sure to pass `-t 10000 -m 4G` (or any other reasonable limits) to AFL.*

*When fuzzing with `yet_another_mutator` pass `AFL_CUSTOM_MUTATOR_LIBRARY=yet_another_mutator/build/libyet_another_mutator.so` to AFLSmart.*

*When fuzzing with AFLSmart pass `-w peach -g <model>.xml -x <dict>.dict`.*

#### 4.3. [OPTIONAL] Remove duplicate crashes

Use `remove_duplicates.py` like this: `python3 remove_duplicates.py --sync_dir work_dir/<configuration>/sync_dir --showmap_path AFL/afl-showmap -- <path to gold>`.

### 5. [OPTIONAL] Save the results

It requires to have MongoDB installed.
Save the results with `./save.py work_dir/<configuration>`.

### 6. Check out `result.ods` and `presentation.odp` C:

