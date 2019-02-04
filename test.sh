#!/bin/bash

try() {
  expected="$1"
  input="$2"

  ./0cc "$input" > tmp.s
  if [ $? -ne 0 ]; then
    echo "0cc error : $input"
    exit 1
  fi
  gcc -o tmp tmp.s
  if [ $? -ne 0 ]; then
    echo "compile error : $input"
    nl -w2 tmp.s
    exit 1
  fi
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input
=> $actual"
  else
    echo "$expected expected, but got $actual"
    nl -w2 tmp.s
    exit 1
  fi
}

try 0 "0;"
try 42 "42;"

try 4 "1+3;"
try 255 "2-3;"
try 21 "5+20-4;"

try 21 "5 + 20 - 4;"
try 38 "1 * 3 + 5 * 7;"
try 56 "1 * (3 + 5) * 7;"
try 1 "3 / 2; 2 - 1;"
try 6 "a = z = 3 * 2;"
try 7 "c = 2; b = 1; c * 3 + b;"
try 7 "foo = 2; bar = 1; foo * 3 + bar;"
try 6 "poo = 1; poop = poo + 2; hoge = 3; poop + hoge;"
try 1 "1 == 1;"
try 0 "1 != 1;"

echo OK
