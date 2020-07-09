### 1. Building ld and gold.

```
git clone git://sourceware.org/git/binutils-gdb.git
cd binutils-gdb
mkdir build
CC=afl-gcc CXX=afl-g++ ./configure --prefix=$PWD/build --enable-gold --disable-shared
make
make install
```

### 2. Compiling test cases.

`gcc -c testcases_src/*.c && mv *.o testcases/`

### 3. Running the test cases.

#### 3.1. 

AFL may ask you to execute the following lines:

```
echo core >/proc/sys/kernel/core_pattern
echo performance | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

#### 3.2. 

Use `./run_gold.sh` or `./run_ld.sh` to start fuzzing.

TODO: libc and crt paths are hardcoded :( .

### 4. Saving the findings.

Use `./save.py findings_gold` or `./save.py findings_ld` to save the findings into MongoDB.

You can export a collection with `mongoexport --db=ftl_afl --collection=gold --out=db/ftl_afl_gold.json`.

