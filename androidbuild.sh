#!/bin/sh
curl -LO https://dl.google.com/android/repository/android-ndk-r13b-linux-x86_64.zip
unzip -qo android-ndk*
NDK="android-ndk-r13b/" CMAKE="/usr/local/bin/cmake" android/compile.sh all
mkdir build
mv Server/*.zip build/
