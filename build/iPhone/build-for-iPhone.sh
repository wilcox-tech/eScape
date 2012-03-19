#!/bin/sh


echo "****  STARTING BUILD OF eScape for iPhone  AT  `date`  ****"
echo
echo


# Clean anything that happened before.
make clean || echo "Okay, we're starting fresh"
echo 


# Create simulator binaries
cmake -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
-DCMAKE_C_FLAGS:STRING="-miphoneos-version-min=3.0 -isysroot /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.0.sdk" \
-DCMAKE_CXX_FLAGS:STRING='-miphoneos-version-min=3.0 -isysroot /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.0.sdk' \
-DCMAKE_OSX_ARCHITECTURES:STRING=i386 \
-DCMAKE_OSX_SYSROOT:PATH=/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.0.sdk \
-DCMAKE_SHARED_LINKER_FLAGS:STRING='-isysroot /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.0.sdk' \
-DEXTRA_UA="$EXTRA_UA" \
-DBUILD_IPHONE:BOOL=ON \
-DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang \
-DOPENSSL_INCLUDE_DIR:PATH="$OPENSSL_INCLUDE" -DOPENSSL_SSL_LIBRARIES:FILEPATH="$OPENSSL_LIBS" \
../.. || (echo "Error: CMake failure."; exit)
make VERBOSE=1 -j5 || (echo "Error: Build failure."; exit)
for i in {amy,b64,gwen,ink,mowgli,uriparser}; do mv lib"$i".a lib"$i"S.a; done
make clean


# Clear out simulator cache
rm CMakeCache.txt


# Create iPhone binaries
cmake -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
-DCMAKE_C_FLAGS:STRING="-mthumb -miphoneos-version-min=3.0 -isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.0.sdk" \
-DCMAKE_CXX_FLAGS:STRING='-mthumb -miphoneos-version-min=3.0 -isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.0.sdk' \
-DCMAKE_OSX_ARCHITECTURES:STRING="armv6;armv7" \
-DCMAKE_OSX_SYSROOT:PATH=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.0.sdk \
-DCMAKE_SHARED_LINKER_FLAGS:STRING='-isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.0.sdk' \
-DEXTRA_UA="$EXTRA_UA" \
-DBUILD_IPHONE:BOOL=ON \
-DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang \
-DOPENSSL_INCLUDE_DIR:PATH="$OPENSSL_INCLUDE" -DOPENSSL_SSL_LIBRARIES:FILEPATH="$OPENSSL_LIBS" \
../.. || (echo "Error: CMake failure."; exit)
make -j5 || (echo "Error: Build failure."; exit)
for i in {amy,b64,gwen,ink,mowgli,uriparser}; do mv lib"$i".a lib"$i"I.a; done
make clean


# Clear out iPhone cache
rm CMakeCache.txt


# Compile into universal binaries
for i in {amy,b64,gwen,ink,mowgli,uriparser}; do lipo -create lib"$i"I.a lib"$i"S.a -output lib"$i".a; done
rm *I.a *S.a


echo "****  FINISHED BUILD OF eScape for iPhone  AT  `date`  ****"
