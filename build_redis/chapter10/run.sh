rm -rf bin
mkdir -p bin
g++ -Wall -Wextra -O2 -g test_avl.cpp -o bin/test_avl
./bin/test_avl
