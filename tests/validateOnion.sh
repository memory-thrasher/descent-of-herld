#!/bin/bash

mkdir -p build/debug/tests 2>/dev/null

#NOTE: I can use libc++ for test cases but not in the release builds bc I don't want to require the client have it installed
clang++ -I "build/shaders" --std=c++20 -stdlib=libc++ -Wl,-rpath,/usr/local/lib/x86_64-unknown-linux-gnu -DDEBUG -g -DVK_NO_PROTOTYPES -DVULKAN_HPP_NO_EXCEPTIONS -I$WITEBASE -I$WITEBASE/shared -Wall tests/validateOnion.cpp -fuse-ld=lld -lvulkan -lrt -o build/debug/tests/validateOnion -Wno-unused-function || exit 1

build/debug/tests/validateOnion
validOnion=$?
if [ $validOnion -eq 0 ]; then
    echo onion validation successful
    exit 0
else
    echo onion validation failed
    exit 1
fi


