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

You should install some packages to perform the Urho3D compilation
```
apt-get install libxrandr-dev libasound2-dev
```

You will also need a C++11 compatible compiler toolchain, like GCC 5.3 or Clang 3.7 and cmake

Before compiling Spacel Game you will need to compile Urho3D as a standalone library

```
cd lib/Urho3D
mkdir build
cd build
cmake .. -DURHO3D_SAMPLES=0 -DURHO3D_C++11=1 -DURHO3D_DATABASE_SQLITE=1 -DURHO3D_TOOLS=0
make
```

You will need the following dependencies for unit tests
```
apt-get install libcppunit-dev
```

And now you can compile the game

```
cd <PROJECT_ROOT>
mkdir build
cd build
cmake ..
make
```

## Compilation options

* BUILD_UNITTESTS (TRUE): build the unit tests

## Included libraries

* Urho3D 1.6-HEAD
* jsoncpp 1.7.2

## Contribute

For contributing please add the following hook to .git/hooks/pre-commit file (it should be executable)

```
#! /bin/sh
git-clang-format -f

python2.7 ./util/travis/checkstyle.py
if [ "$?" != "0" ]; then
	echo "Code formatting was wrong !"
	exit 1;
fi
```

This hook will permit you to prevent some coding style errors before commit.

## Documentations

### UI/Client/Server communication

![UI_Client_Server_Communication](https://raw.githubusercontent.com/spacel/game/master/doc/communication_scheme.svg)
