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
def is_tier1_asn(asn):
        iasn = int(asn)		
        if iasn==3356 or iasn==7018 or iasn==701 or iasn==3549 or iasn==2914 or iasn==3257 or iasn==1299 or iasn==1239 or iasn==3561 or iasn==6762 or iasn==209 or iasn==3320 or iasn==2828 or iasn==6453 or iasn==6461 or iasn==174:
                return True
        else:
                return False

####################################################################################
# Start Main loop
####################################################################################
if len(sys.argv)>1:
	filename = sys.argv[1]
else:
	print "Please input a file"
	sys.exit()

#record the start time
start = time.time()

#open files
ASNPathDic = dict()
with open(filename) as infile:
        for line in infile:
                #extract the prefix                
                splitline = line.split(':')
                prefix = splitline[0]
                next_hop = splitline[1]
                path = splitline[2]
                path = path.rstrip("\r\n")
                path_nodes = path.split()                
                key = path

                if key not in ASNPathDic:						
			ASNPathDic.update({key:Set([])})
		ASNPathDic.__getitem__(key).add(next_hop)					
        
#write the unique asn paths based on the originating node
tier1_folder_path = "..//data//tier1_paths"
nontier1_folder_path = "..//data//non_tier1_paths"
if not os.path.exists(tier1_folder_path): os.makedirs(tier1_folder_path)
if not os.path.exists(nontier1_folder_path): os.makedirs(nontier1_folder_path)
tot_records = 0
tier1_records = 0
non_tier1_records = 0
for k in ASNPathDic:
        node_list=k.split(" ")
        if is_tier1_asn(node_list[0]) is True:
                node_file=open(tier1_folder_path+"//"+node_list[0], "a+")
                tier1_records=tier1_records+1
        else:
                node_file=open(nontier1_folder_path+"//"+node_list[0], "a+")
                non_tier1_records=non_tier1_records+1
        tot_records=tot_records+1
        print >> node_file, "%s" % (k)
        node_file.close()

#Record end time
end = time.time()
elapsed=end-start
print "Wrote %d paths: %d to %s and %d paths to %s in %d seconds"%(tot_records, tier1_records, tier1_folder_path, non_tier1_records,nontier1_folder_path,elapsed)
####################################################################################
# End Main loop
####################################################################################
