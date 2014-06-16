export CXXFLAGS="-O0 -Wall"
rm CMakeCache.txt -fr && rm CMakeFiles -fr
cmake -DCMAKE_BUILD_TYPE=Debug ./                                                                                                                                                           
time make
