#!/bin/bash

# ShellScript for clean build/ directory.

# [ Usage ]
#   1. chmod +x CleanCmake.sh
#   2. ./CleanCmake.sh

echo -e "############# CLEAN CMAKE #############"

echo -e "- Static Library ----------------------"
rm -vrf *.a

echo -e "- Cmake Files -------------------------"
rm -rf  CMakeFiles
rm -vrf cmake_install.cmake
rm -vrf CMakeCache.txt
rm -vrf install_manifest.txt

echo -e "- build Files -------------------------"
rm -vrf Makefile
rm -vrf runFile

echo -e "############## CLEAN DONE #############"
