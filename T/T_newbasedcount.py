#!/usr/bin/env python3

# state state input output weight

#fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt transitions.txt > test.fst
#fstdraw --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt < test.fst | dot -Tps > test.ps

import os
import string
from itertools import *
from math import *
import sys

print("./T_count [input count txt file] [output transducer T txt&fst]")
filename1 = sys.argv[1]
filename2 = sys.argv[2]

SmoothConstant = 0.1
CountFile = open(filename1, 'r')

#while True:
#	line = CountFile.readline()
#	if line == "":
#		break
#	elements = line[0:-1].split(' ')
#	elements[2] = -log((float(elements[2])+1)/20000)
#	
#	
#	print('0', '0', elements[0], elements[1], elements[2])
#print('0')
Count = {}
Count["<eps>"] = 0
Count["a"] = 0
Count["b"] = 0
Count["c"] = 0
Count["d"] = 0
Count["e"] = 0
Count["f"] = 0
Count["g"] = 0
Count["h"] = 0
Count["i"] = 0
Count["j"] = 0
Count["k"] = 0
Count["l"] = 0
Count["m"] = 0
Count["n"] = 0
Count["o"] = 0
Count["p"] = 0
Count["q"] = 0
Count["r"] = 0
Count["s"] = 0
Count["t"] = 0
Count["u"] = 0
Count["v"] = 0
Count["w"] = 0
Count["x"] = 0
Count["y"] = 0
Count["z"] = 0

TotalCount = 0

while True:
	line = CountFile.readline()
	if line == "":
		break
	elements = line[0:-1].split(' ')
	Count[elements[0]] = Count[elements[0]] + int(elements[2])
	TotalCount = TotalCount + int(elements[2])

CountFile.close()
############################################################
CountFile = open(filename1, 'r')
TFile = open(filename2 + ".txt", 'w')

while True:
	line = CountFile.readline()
	if line == "":
		break
	elements = line[0:-1].split(' ')
	if elements[0] == "<eps>":
		prob = int(elements[2]) / TotalCount
	else:
		prob = int(elements[2]) / Count[elements[0]] * (TotalCount - Count["<eps>"]) / TotalCount
		
	prob = prob * (1 - SmoothConstant) + SmoothConstant / (26 + 27)
	prob = -log(prob)
	print(0, 0, elements[0], elements[1], prob, file=TFile)
	
print(0, 0, '</w>', '</w>', 0,file=TFile)
print(0, 0, '</s>', '</s>', 0, file=TFile)
print(0, file=TFile)

CountFile.close()
TFile.close()

os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt " + filename2 + ".txt" + " > " + filename2 + ".fst")
os.system("fstdraw --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt <  " + filename2 + ".fst" + " | dot -Tps > " + filename2 + ".ps")
