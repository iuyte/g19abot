#!/bin/bash

cd /home/iuyte/src/robots/batmobile

git whatchanged > log.txt
rm docs/index.html
tools/cleanup.py log.txt docs/index.html
rm log.txt
