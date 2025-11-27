rm -rf bin
mkdir -p bin
g++ -Wall -Wextra -O2 -g test_zset.cpp -o bin/test_zset
./bin/test_zset
