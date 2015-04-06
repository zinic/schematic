#!/bin/sh

SCRIPT_PATH=$(dirname $0)
SOURCE_PATH=$SCRIPT_PATH/src/python

if [ -z "$NO_COMPILE" ]; then
    # Set include paths for important things
    CFLAGS="${CFLAGS} -I ./includes/uthash/src/utstring.h"
    CFLAGS="${CFLAGS} -I ./includes/uthash/src/utarray.h"
    CFLAGS="${CFLAGS} -I ./includes/uthash/src/utlist.h"
    CFLAGS="${CFLAGS} -I ./includes/uthash/src/uthash.h"

    # Compile the project
    CFLAGS=$CFLAGS python setup.py build_ext --inplace > /tmp/output 2>&1

    if [ $? -ne 0 ]; then
        cat /tmp/output
    fi

    # Remove the temp file
    rm /tmp/output >> /dev/null 2>&1
fi

# Run the code if we compiled okay
PYTHONPATH=$SOURCE_PATH python $SOURCE_PATH/schematic/main.py $@
