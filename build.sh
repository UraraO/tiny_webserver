#!/bin/sh

cmake -S . -B ./bin
cmake --build ./bin

LINK_FILE=./tiny_WebServer
if [ ! -f "$LINK_FILE" ]; then
  cp -l ./bin/src/tiny_WebServer .
fi

echo ~build finish~