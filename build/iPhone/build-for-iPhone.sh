#!/bin/sh

make clean
cp CMakeCacheSim.txt CMakeCache.txt
make -j5
for i in {amy,b64,gwen,ink,mowgli,uriparser}; do mv lib"$i".a lib"$i"S.a; done
make clean
cp CMakeCacheiPhone.txt CMakeCache.txt
make -j5
for i in {amy,b64,gwen,ink,mowgli,uriparser}; do mv lib"$i".a lib"$i"I.a; done

for i in {amy,b64,gwen,ink,mowgli,uriparser}; do lipo -create lib"$i"I.a lib"$i"S.a -output lib"$i".a; done
rm *I.a *S.a

echo "*** Built iPhone OS and iPhone Simulator binaries."
