#!/bin/bash
set -e

DIR=$(cd "$(dirname "$0")" && pwd)
g++ -std=c++11 -Wall -I"$DIR/../scheme1" "$DIR/../scheme1/orderedList.cpp" "$DIR/test_orderedList.cpp" -o "$DIR/test_orderedList"
"$DIR/test_orderedList"
