#!/usr/bin/python3

import os
import string
from itertools import *
import sys

def wordTransitions(word, fileName):
	f = open(fileName, "w")
	stateNumber = 0
	for letter in word:
		print(stateNumber, stateNumber+1, letter, letter, file=f)
		stateNumber = stateNumber + 1
	print(stateNumber, file=f)



print("./T_count [T transducer file] [output count txt file]")
filename1 = sys.argv[1]
filename2 = sys.argv[2]


#initialize count
count = {}
for input in chain(string.ascii_lowercase,['<eps>']):
	for output in chain(string.ascii_lowercase,['<eps>']):
		if input=='<eps>' and output=='<eps>':
			continue
		count[(input,output)] = 0

for input in chain(string.ascii_lowercase):
	for output in chain(string.ascii_lowercase):
		if input==output:
			continue
		count[(input + output, output+input)] = 0

misspell = open("machine_readable3.txt",'r')
# misspell = open("test.txt",'r')

flag = True
totalProb = 0
while flag:
	line = misspell.readline()
	if line == "":
		break
	print(line)
	wrong = line.split('\t')[0].lower()
	correct = line.split('\t')[1].lower()

	wordTransitions(wrong,"tmp_WrongWord.txt")
	wordTransitions(correct,"tmp_CorrectWord.txt")

	os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt tmp_WrongWord.txt > tmp_WrongWord.fst")
	os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt tmp_CorrectWord.txt > tmp_CorrectWord.fst")
	
	os.system("fstcompose tmp_WrongWord.fst " + filename1 + " > tmp_Composed1.fst")
	os.system("fstcompose tmp_Composed1.fst tmp_CorrectWord.fst > tmp_Composed2.fst")
	os.system("fstshortestpath tmp_Composed2.fst > tmp_ComposedShortestPath.fst")
	os.system("fstprint --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt tmp_ComposedShortestPath.fst > tmp_ComposedShortestPath.txt")
	
	countFile = open("tmp_ComposedShortestPath.txt",'r')
	lines = countFile.readlines()
	for idx, line2 in enumerate(lines):
		sline = line2.split();
		if len(sline) < 4:
			continue
		
		if len(sline) == 5:
			totalProb += float(sline[4])
		try:
			prevline = lines[idx-1].split()
			if sline[3] == prevline[2] and sline[2] == prevline[3]:
				count[(sline[2] + sline[3], sline[3] + sline[2])] += 1
				count[(sline[3], sline[2])] -= 1
				continue
		except:	
		        pass
		
		count[(sline[2], sline[3])] += 1
	#flag = False

os.remove("tmp_WrongWord.txt")
os.remove("tmp_WrongWord.fst")
os.remove("tmp_CorrectWord.txt")
os.remove("tmp_CorrectWord.fst")
os.remove("tmp_Composed1.fst")
os.remove("tmp_Composed2.fst")
os.remove("tmp_ComposedShortestPath.fst")
os.remove("tmp_ComposedShortestPath.txt")

CountFile = open(filename2,'w')
for key, value in count.items():
	print(key[0], key[1], value,file=CountFile)
CountFile.close()

logFile = open('totalProb.log','a')
line = filename1 + str(totalProb) + '\n'
logFile.write(line)
logFile.close()
