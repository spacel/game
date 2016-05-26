# Spacel Game

The Spacel Game repository

[![Build Status](https://travis-ci.org/spacel/game.svg?branch=master)](https://travis-ci.org/spacel/game)

## Licence

This program is under the GPLv3 licence.

You should look at LICENCE file for more informations

## Compiling the project

First you need to clone the repository and its dependencies

```
git clone https://github.com/spacel/game.git spacelgame
cd spacelgame
git submodule sync --recursive && git submodule update --init --recursive
```

You will also need a C++14 compatible compiler toolchain, like GCC 5.3 or Clang 3.7 and cmake

Before compiling Spacel Game you will need to compile Urho3D as a standalone library

```
cd lib/Urho3D
mkdir build
cd build
cmake ..
make
```

And now you can compile the game

```
cd <PROJECT_ROOT>
mkdir build
cd build
cmake -D URHO3D_HOME=$(pwd)/../lib/Urho3D/build/ ..
make
```
