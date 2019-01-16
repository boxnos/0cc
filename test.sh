#!/bin/bash

try() {
  expected="$1"
  input="$2"

  ./0cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    nl -w2 tmp.s
    exit 1
  fi
}

try 0 0
try 42 42

try 4 1+3
try 255 2-3
try 21 '5+20-4'

try 21 '5 + 20 - 4'
try 38 '1 * 3 + 5 * 7'
try 56 '1 * (3 + 5) * 7'
try 1 '3 / 2'
# try 41 " 12 + + 34 - 5 "
# try hoge 2*3

echo OK
