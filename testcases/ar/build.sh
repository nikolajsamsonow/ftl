#!/bin/bash

mkdir -p in
gcc -c f.c
gcc -c main.c
ar rvs test.a f.o main.o
mv test.a in
rm f.o main.o

