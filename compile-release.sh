export CXXFLAGS="-Ofast -Wall"
rm CMakeCache.txt -fr && rm CMakeFiles -fr
cmake -DCMAKE_BUILD_TYPE=Release ./
time make
