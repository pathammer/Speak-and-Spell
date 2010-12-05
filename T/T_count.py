#!/usr/bin/env python3

# state state input output weight

#fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt transitions.txt > test.fst
#fstdraw --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt < test.fst | dot -Tps > test.ps

import os
import string
from itertools import *

#initialize count
count = {}
for input in chain(string.ascii_lowercase,['<eps>']):
	for output in chain(string.ascii_lowercase,['<eps>']):
		count[(input,output)] = 0


def wordTransitions(word, fileName):
	f = open(fileName, "w")
	stateNumber = 0
	for letter in word:
		print(stateNumber, stateNumber+1, letter, letter, file=f)
		stateNumber = stateNumber + 1
	print(stateNumber, file=f)


misspell = open("machine_readable3.txt",'r')


while True:
	line = misspell.readline()
	if line == "":
		break
	print(line)
	wrong = line.split('\t')[0].lower()
	correct = line.split('\t')[1].lower()

	wordTransitions(wrong,"WrongTransitions.txt")
	wordTransitions(correct,"CorrectTransitions.txt")
	
	os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt WrongTransitions.txt > WrongTransitions.fst")
	os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt CorrectTransitions.txt > CorrectTransitions.fst")
	
	os.system("fstcompose WrongTransitions.fst T.fst > Composed1.fst")
	os.system("fstcompose Composed1.fst CorrectTransitions.fst > Composed2.fst")
	os.system("fstshortestpath Composed2.fst > ComposedShortestPath.fst")
	os.system("fstprint --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt ComposedShortestPath.fst > ComposedShortestPath.txt")
	
	countFile = open("ComposedShortestPath.txt",'r')
	while True:
		line2 = countFile.readline()
		if line2 == "":
			break
		line2 = line2[:-1]
		if len(line2.split('\t')) < 4:
			continue
		count[(line2.split('\t')[2],line2.split('\t')[3])] = count[(line2.split('\t')[2],line2.split('\t')[3])] + 1
		

CountFile = open("count.txt",'w')
for key, value in count.items():
	print(key[0], key[1], value,file=CountFile)
#wordfst("hello","in.txt")
#wordfst("helol","out.txt")
#os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt in.txt > in.fst")
#os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt out.txt > out.fst")
#os.system("fstcompose in.fst T.fst > first.fst")
#os.system("fstcompose first.fst out.fst > second.fst")