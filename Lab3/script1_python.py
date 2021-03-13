#! /usr/bin/env python3

# Script input params: script1_python.py [file_name] [who] [perm]
# Input 
# 	- file_name: name of a file that contains the result of "ls -l" (bash)
# 	- who: user|group|others
# 	- perm: r|w|x
#
# Body
# 	This script reads the input file and puts the content in a tuple list.
# 	The tuples contain the first and last element of each line (permissions and filename).
#	Then iterates the list calling test_perms function.

import sys
import os.path

# _____________________________________Functions______________________________________________

# Check if the tuple file has activated the inputed permission (r|w|x) for user|group|others.
# Shows it on screen.
def test_perms(tupla, who, perm):
	
	who_pos = { "user": 1, "group": 4, "others": 7 }
	perm_pos = { 'r': 0, 'w': 1, 'x': 2 }
	pos = who_pos.get(who) + perm_pos.get(perm)
	
	if tupla[0][pos] != '-':
		print("The file", tupla[1], "has the", who, perm, "permission activated")

# _____________________________________Main code______________________________________________

# Error control
if len(sys.argv) != 4:
	sys.exit("Error: incorrect number of parameters")

file_name = sys.argv[1]
if not os.path.isfile(file_name):
	sys.exit("Error: the 1st param file doesn't exist")

who = sys.argv[2]
who_list = ["user", "group", "others"]
if not who in who_list:
	sys.exit("Error: the 2nd param should be user|group|others")

perm = sys.argv[3]
perm_list = ['r','w','x']
if not perm in perm_list:
	sys.exit("Error: the 3rd param should be r|w|x")


# Read file
file = open(file_name, 'r')
flines_list = file.readlines()
flines_list.pop(0)
file.close()

# Create structures
perms_list = []
for line in flines_list:
	split_line = line.split()
	perm_fname = (split_line[0], split_line[8])
	perms_list.append(perm_fname)

# Function call
for elem in perms_list:
	test_perms(elem, who, perm)
