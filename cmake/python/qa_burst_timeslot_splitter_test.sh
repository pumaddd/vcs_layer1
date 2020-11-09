#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH
export PYTHONPATH=/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/swig:/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/misc_utils:$PYTHONPATH
/usr/bin/python2 /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/qa_burst_timeslot_splitter.py 
