#!/bin/sh

isam=$(which automake)
isac=$(which autoconf)
if [ -z "$isam" ] || [ -z "$isac" ]; then
	echo "-> ERROR: please install automake and autoconf"
	exit
fi
echo "--> Running autoreconf..."
autoreconf --install
if [ $? = 0 ]; then
	echo "--> Running configure..."
	./configure
else
	echo "--> ERROR: something went wrong, exiting..."
fi
if [ $? = 0 ]; then
	echo "--> Running make..."
	make
else
	echo "--> ERROR: something went wrong, exiting..."
	exit
fi
mv -v src/landnd $PWD
echo "--> All done :)"
