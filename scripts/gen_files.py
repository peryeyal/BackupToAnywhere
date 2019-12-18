#!/bin/bash

s3_glacier_chunk_size=1048576

mkdir -p ./rand_files
dd if=/dev/urandom of=./rand_files/sample1.txt bs=$s3_glacier_chunk_size count=1
dd if=/dev/urandom of=./rand_files/sample2.txt bs=$s3_glacier_chunk_size count=1
dd if=/dev/urandom of=./rand_files/sample3.txt bs=$s3_glacier_chunk_size count=1
