#!/bin/bash
mkdir xcode
./clean.sh
cd xcode
cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 -G Xcode ..
xcodebuild -project hellcat.xcodeproj/
