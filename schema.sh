#!/bin/sh

SCRIPT_PATH=$(dirname $0)
SOURCE_PATH=$SCRIPT_PATH/src/python

PYTHONPATH=$SOURCE_PATH python $SOURCE_PATH/schematic/main.py $@
