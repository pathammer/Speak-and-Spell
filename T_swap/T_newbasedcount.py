#!/usr/bin/python

import os
import string
from itertools import *
from math import *
import sys

def localTransCount(transitions):
	Count = {}
	Count["<eps>"] = 0

	for start in chain(string.ascii_lowercase):
		Count[start] = 0

	for t in transitions:
		i = t.split()[0]
		if i != '<eps>':
			i = i[0]
		Count[i] += 1
	return Count

print("./T_count [input count txt file] [output transducer T txt&fst]")
filename1 = sys.argv[1]
filename2 = sys.argv[2]

SmoothConstant = 0.1

localSums = {}
localSums["<eps>"] = 0

for start in chain(string.ascii_lowercase):
	localSums[start] = 0

CountFile = open(filename1, 'r')
lines = CountFile.readlines()
CountFile.close()

TotalCount = 0
Counts = {}
for line in lines:
	[input, output, count] = line.split()
	# drop zero swaps
	# if len(input) == 2 and int(count) == 0:
	#  	continue
	Counts[input+' '+output] = int(count)
	if input != '<eps>':
		input = input[0]
	localSums[input] += int(count)
	TotalCount += int(count)

############################################################


while True:
	localTransition = localTransCount(Counts.keys())
	removeCount = 0;
	Probs = {}
	for transition, count in Counts.items():
		[input, output] = transition.split()
		if input == "<eps>":
			prob = int(count) / TotalCount
		else:
			input = input[0]
			prob = (float(count) / localSums[input]) * ((TotalCount - localSums["<eps>"]) / float(TotalCount))
		
		prob = prob * (1 - SmoothConstant) + SmoothConstant / localTransition[input]
		prob = -log(prob)
		Probs[transition] = prob

	for transition, prob in Probs.items():
		[input, output] = transition.split()
		if len(input) == 2:
			replaceProb = Probs[input[0]+' '+output[0]] + Probs[input[1]+' '+output[1]]
			if replaceProb < prob:
				Counts.pop(transition)
				removeCount +=1

	if removeCount == 0:
		break

probLines = []
state = 1

for transition, prob in Probs.items():
	[input, output] = transition.split()

	if len(input) == 2:
		probLine = '0 ' + str(state) + ' ' + input[0] + ' ' + output[0] + ' ' + str(prob) + '\n'
		probLines.append(probLine)
		probLine = str(state) + ' 0 ' + input[1] + ' ' + output[1] + ' 0\n'
		probLines.append(probLine)
		state += 1
	else:
		probLine = '0 0 ' + input + ' ' + output + ' ' + str(prob) + '\n'
		probLines.append(probLine)

probLines.append('0 0 </w> </w> 0\n')
probLines.append('0 0 </s> </s> 0\n')
probLines.append('0\n')

TFile = open(filename2 + ".txt", 'w')
TFile.writelines(probLines)
TFile.close()

os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt " + filename2 + ".txt" + " > " + filename2 + ".fst")
# os.system("fstdraw --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt <  " + filename2 + ".fst" + " | dot -Tps > " + filename2 + ".ps")
