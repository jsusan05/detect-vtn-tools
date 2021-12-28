#!/usr/bin/python

####################################################################################
# extractprefix.py
# Description   :  Script to Process data.txt which is a BGP dump file
# Authors       :  Jinu Susan
# 
####################################################################################

#File includes
import sys
import time
import re
from sets import Set
import os

#Global variables

####################################################################################
# Start Main loop
####################################################################################
filename = "all_degree.txt"
numlines=0
#open files
degfile = open("degree.txt", "w+")
with open(filename) as infile:
        for line in infile:
                #extract the prefix                
                splitlist = line.split(':')
                print >> degfile, "%s:%s" % (splitlist[0],splitlist[1])
                numlines = numlines+1
degfile.close()                

#Record end time
print "Wrote %d "%(numlines)
####################################################################################
# End Main loop
####################################################################################
