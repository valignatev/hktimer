#/env/bin sh
set -e
mkdir -p $(pwd)/build

g++ -D DEBUG hktimer.cpp -o $(pwd)/build/hktimer \
    -Wall -Wextra -pedantic -std=c++20
