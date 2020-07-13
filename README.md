### 1. Building ld and gold.

```
git clone git://sourceware.org/git/binutils-gdb.git
cd binutils-gdb
```

Apply `output.patch` to `gold/output.cc`.

```
mkdir build
CC=afl-gcc CXX=afl-g++ ./configure --prefix=$PWD/build --enable-gold --disable-shared
make
make install
```

### 2. Compiling test cases.

#### 2.1. Single file.

`gcc -c testcases_src/*.c && mv *.o testcases/`

#### 2.2. Multiple files.

```
gcc -c testcases_ar_src/*.c
ar rvs test.a testcases_ar_src/main.o testcases_ar_src/f.o
mv test.a testcases/
```

### 3. Running the test cases.

#### 3.1. 

AFL may ask you to execute the following lines:

```
echo core >/proc/sys/kernel/core_pattern
echo performance | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

#### 3.2. 

Use `./run_gold.sh` or `./run_ld.sh` or `./run_lld.sh` to start fuzzing.

TODO: libc and crt paths are hardcoded :( .

### 4. Saving the findings.

Use `./save.py findings_gold` or `./save.py findings_ld` or `./save.py findings.lld` to save the findings into MongoDB.

You can export a collection with `mongoexport --db=ftl_afl --collection=<either gold, ld, lld> --out=db/ftl_afl_<either gold, ld, lld>.json`.

