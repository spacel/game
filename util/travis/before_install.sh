#! /bin/bash

if [ "$CXX" = "g++" ]; then
	sudo apt-get install -y g++-5 gcc-5
        export CXX=g++-5
        export CC=gcc-5
fi

