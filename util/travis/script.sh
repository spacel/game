#! /bin/bash

mkdir -p build lib/Urho3D/build
cd lib/Urho3D/build/
cmake -DURHO3D_SSE=0 -DURHO3D_SAMPLES=0 -DURHO3D_MINIDUMPS=0 -DURHO3D_C++11=1 -DURHO3D_DATABASE_SQLITE=1 -DURHO3D_TOOLS=0 ..
make -j2

cd ../../../build/
cmake -D URHO3D_HOME=$(pwd)/../lib/Urho3D/build/ -DENABLE_UNITTESTS=1 ..
make -j2
