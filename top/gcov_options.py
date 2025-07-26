#!/usr/bin/python3
"""This script contains the REPOSITORY specific options when running generating
   code coverage reports.  It is intended to be called when the 'tca.py' script
   runs
"""
import os

gcov_exclude_options = r'--exclude=.*/xpkgs/.* --exclude=.*src/Kit/Json/Arduino.h --exclude=.*src/Kit/Json/ArduinoHelpers.cpp --exclude=.*src/Kit/Type/enum.h --exclude-lines-by-pattern .*KIT_SYSTEM_TRACE.* --exclude-lines-by-pattern .*KIT_SYSTEM_ASSERT.* --gcov-ignore-errors=no_working_dir_found'

# MAIN
if __name__ == '__main__':
	#print( gcov_exclude_options )
	print("")