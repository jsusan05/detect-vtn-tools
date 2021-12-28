#!/usr/bin/python

####################################################################################
# extractasn.py
# Description   :  Script to Process data.txt which is a BGP dump file
# Authors       :  Jinu Susan
# 
####################################################################################

#File includes
import sys
import time
import re
from sets import Set

#Global variables
ASNDic = dict()

outfilename = "output.txt"
#limit_filename = "invalid_asn.txt"
prefix_filename = "prefixpaths.txt"
#curly_filename = "curly.txt"
#loop_filename = "loops.txt"

####################################################################################
#process the path and creates a list of unique ASNs with neighbors
####################################################################################
def extract_asn(line):			
	stripleft = line.replace('{',' ');
	stripright = stripleft.replace('}',' ');
	stripright = stripright.replace(',',' ')
	aspath = stripright.split()
	prev = ''
	for asn in aspath:			 
		if asn!='':                        
			if asn not in ASNDic:						
				ASNDic.update({asn:Set([])})						
			if len(prev)>0:
				if asn!=prev:					
					ASNDic.__getitem__(asn).add(prev)
					ASNDic.__getitem__(prev).add(asn)
			prev = asn			


class ASPathProperty:
  iscurly = False
  isloop = False
  hasInvalidAsn = False
  san_path = ''
  raw_path = ''

####################################################################################
# Checks if given asn is invalid if it falls in ranges below
# 0				Reserved
# 23456				AS_TRANS
# 64000-64495			Reserved
# 64496-64511			Reserved
# 64512-65534			Reserved
# 65535				Reserved
# 65536-65551			Reserved
# 65552-131071			Reserved
# 133632-196607			Unallocated
# 202240-262143			Unallocated
# 263680-327679			Unallocated
# 328704-393215			Unallocated
# 394240-4199999999		Unallocated
# 4200000000-4294967294	        Reserved 	
# 4294967295			Reserved
####################################################################################
def is_invalid_asn(asn):
        iasn = int(asn)
        if iasn==0 or iasn==23456 or (iasn>=64000 and iasn<=64495) or (iasn>=64496 and iasn<=64511) or (iasn>=64512 and iasn<=65534) or (iasn==65535)\
           or (iasn>=65536 and iasn<=65551) or (iasn>=65552 and iasn<=131071) or (iasn>=133632 and iasn<=196607)\
           or (iasn>=202240 and iasn<=262143) or (iasn>=263680 and iasn<=327679) or (iasn>=328704 and iasn<=393215)\
           or (iasn>=394240 and iasn<=4199999999) or (iasn>=4200000000 and iasn<=4294967294) or (iasn==4294967295):
                return True
        else:
                return False
        
####################################################################################
# Process the ASPath
# Create a sanitized line which has no loops, large ASNs or curly braces
# Create a raw path which has large asns, loops and curly braces
####################################################################################
def sanitize_line(line):        
        san_list = []
        path_prop = ASPathProperty()
        left_curly = line.find('{')
        right_curly = line.find('}')
        stripleft = line.rsplit('{', 1)[0]
        stripright = stripleft.replace(',',' ')
	aspath = stripleft.split()
	prev = ''
	
	for asn in aspath:			 
		if asn!='':
                        #Remove consecutive duplicated                      
			if prev!='' and int(asn)==int(prev):                                
                                prev = asn
                                continue
                        #Check if there is a invalid asn and halt
                        elif is_invalid_asn(asn) is True:
                                path_prop.hasInvalidAsn = True                                
                                return path_prop
			else:
                                san_list.append(asn)
                                
			prev = asn

	san_curly_list = []
        if left_curly >= 0 and right_curly >= 0:                             
                line = line[left_curly+1:right_curly]
                line = line.rstrip("\r\n")
                line = line.replace(',',' ')
                aspath = line.split()                
                prev = ''
                for asn in aspath:			 
                        if asn!='':
                                #Remove duplicates
                                if prev!='' and int(asn)==int(prev):
                                        prev = asn
                                        continue                               
                                else:
                                        san_curly_list.append(asn)
                                        
                                prev = asn


        san_str = " ".join(san_list)        
        #multiple ASNs within {}
        if len(san_curly_list)>1:
                path_prop.iscurly = True
                curly_str = " ".join(san_curly_list)
                path_prop.raw_path = san_str+'{'+ curly_str + '}'
                        
        #single ASN within {}
        elif len(san_curly_list)==1:                
                path_prop.iscurly = False
                #if the one in {} and the last ASN in path are not the same
                if int(san_list[len(san_list)-1])!=int(san_curly_list[0]):
                        #Check if invalid asn
                        if is_invalid_asn(san_curly_list[0]) is True:
                                path_prop.hasInvalidAsn = True
                                return  path_prop                              
                        else:
                                san_list.append(san_curly_list[0])                
                san_str = " ".join(san_list)
                                                       
        #Check for loops
        n = Set([])
        
        for q in san_list:
              n.add(q)

        if len(san_list)>len(n):              
                path_prop.isloop=True                
                
        path_prop.san_path = san_str
        
        return path_prop        

####################################################################################
# Start Main loop
####################################################################################
filename = "../data/data.txt"

#record the start time
start = time.time()
tot_items = 0
#open files
prefix_line = ''
nexthop_line = ''
#invalid_file=open(limit_filename, "w+")
#curly_file = open(curly_filename, "w+")
#loop_file = open(loop_filename, "w+")
prefix_file = open(prefix_filename, "w+")
with open(filename) as infile:
        for line in infile:
                #extract the prefix
                if line.find("PREFIX")>=0:
                        prefix_line = (line.rsplit(':', 1)[1])
                        prefix_line = prefix_line.rstrip("\r\n")
                        prefix_line = prefix_line.lstrip(" ")
                        
                #write to prefix if there is no next hop
                elif line.find("/{")>=0:                                             
                        if (path_property.isloop is False) and (path_property.hasInvalidAsn is False):
                                print >> prefix_file, "%s:NA:%s" %(prefix_line, path_property.san_path)
                                tot_items = tot_items+1

                #write to prefix if there is a next hop
                elif line.find("NEXT_HOP")>=0:
                        nexthop_line = (line.rsplit(':', 1)[1])
                        nexthop_line = nexthop_line.rstrip("\r\n")
                        nexthop_line = nexthop_line.lstrip(" ")                        
                        if (path_property.isloop is False) and (path_property.hasInvalidAsn is False):
                                print >> prefix_file, "%s:%s:%s" %(prefix_line, nexthop_line, path_property.san_path)
                                tot_items = tot_items+1
                        
                #extract the sanitized path and write it to appropriate file else process it 
                elif line.find("ASPATH")>=0:
                        line = (line.rsplit(':', 1)[1])
                        line = line.rstrip("\r\n")
                        #sanitize the raw path
                        path_property = sanitize_line(line)
                        
                        #write sanitized line to prefix file
                        if len(nexthop_line) is 0:
                                nexthop_line = "NA"                                                                               

                        #Check if loop exists in path
                        #if path_property.isloop is True:
                        #        print >> loop_file, "%s:%s" %(prefix_line, line)
                                
                        #Check if a large ASN exists in path
                        #if path_property.hasInvalidAsn is True and path_property.isloop is False:
                        #        print >> invalid_file, "%s:%s" %(prefix_line, line)
                                
                        #Check if a multiple ASN exists in curly braces in path
                        #if path_property.iscurly is True:
                        #        print >> curly_file, "%s:%s" %(prefix_line, path_property.raw_path) 
                                
                        if path_property.isloop is False and path_property.hasInvalidAsn is False:
                                extract_asn(path_property.san_path)

        
                               
#close all files opened
prefix_file.close()
#loop_file.close()
#curly_file.close()
#invalid_file.close()


filename = "../data/oix-full-snapshot-2014-05-01-0200"
prefix_file = open(prefix_filename, "a+")
with open(filename) as infile:
        for line in infile:                
                #extract the prefix
                if line.find("*")==0:
                        prefix_line = (line.rsplit('*', 1)[1])
                        prefix_line = prefix_line.rstrip("\r\n")
                        prefix_list = line.split()
                        final = len(prefix_list)-7
                        i=6
                        path_line = ""
                        while len(prefix_list)-1 > i:
                                path_line = path_line + ' '+ prefix_list[i]
                                i = i+1
                        path_property = sanitize_line(path_line)
                        if (path_property.isloop is False) and (path_property.hasInvalidAsn is False):
                                print >> prefix_file, "%s:%s:%s" %(prefix_list[1], prefix_list[2], path_property.san_path)
                                extract_asn(path_property.san_path)
                                tot_items = tot_items+1
						
                                
        
                               
#close all files opened
prefix_file.close()

#write the output txt file for words to be examined
tot_keys = len(ASNDic.keys())
with open(outfilename, "w+") as outfile:
	for k in sorted(ASNDic, key=lambda k: len(ASNDic[k]), reverse=True):
                neighbor_str = " ".join(ASNDic[k])
                neighbor_str = '('+neighbor_str.strip('Set')+')'
		print >> outfile, "%d:%s:%s" % (len(ASNDic[k]),k,neighbor_str) 		
        

#Record end time
end = time.time()
elapsed=end-start
print "Wrote %d paths to prefixpaths.txt in %d seconds"%(tot_items,elapsed)
print "Wrote %d ASNs to output.txt in %d seconds"%(tot_keys,elapsed)

####################################################################################
# End Main loop
####################################################################################
