#!/bin/bash
find . -name 'cmake_install.cmake' -exec rm -rf {} \+
find . -name 'CMakeCache.txt' -exec rm -rf {} \+
find . -name 'CMakeFiles' -exec rm -rf {} \+
find . -name 'Makefile' -exec rm -rf {} \+
