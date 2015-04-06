#!/bin/sh

SCRIPT_PATH=$(dirname $0)
SOURCE_PATH=$SCRIPT_PATH/src/python

# Set include paths for important things
CFLAGS="${CFLAGS} -I ./includes/uthash/src/utstring.h"
CFLAGS="${CFLAGS} -I ./includes/uthash/src/utarray.h"
CFLAGS="${CFLAGS} -I ./includes/uthash/src/utlist.h"
CFLAGS="${CFLAGS} -I ./includes/uthash/src/uthash.h"

# Compile the project
CFLAGS=$CFLAGS python setup.py build_ext --inplace > /tmp/output 2>&1

# Run the code if we compiled okay
if [ $? -eq 0 ]; then
    PYTHONPATH=$SOURCE_PATH python $SOURCE_PATH/schematic/main.py $@
else
    cat /tmp/output
fi

# Remove the temp file
rm /tmp/output >> /dev/null 2>&1
