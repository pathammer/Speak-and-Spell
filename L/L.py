#!/usr/bin/env python3

import os
import re

symbolList = []
g = open("../word_symbols.txt")
#g = open("test_word_symbols.txt")

while True:
	tmp = g.readline().strip().split(' ')[0]
	if tmp == '':
		break   #EOF
	if re.match(".*[^a-z].*", tmp): #if there is non a-z letter, reject.
		continue
	symbolList.append(tmp)

g.close()

stateIndex = 0

class Node:
	def __init__(self):
		self.children = {}
		
		global stateIndex
		self.index = stateIndex
		stateIndex = stateIndex + 1

root = Node()


for symbol in symbolList:
	node = root
	for letter in symbol:
		if letter in node.children:
			pass
		else:
			node.children[letter] = Node()
		node = node.children[letter]
	node.children["</w>"] = (0, symbol)
	#stateIndex = stateIndex + 1
	 
t = open("L.txt", "w")
#t = open("test_L.txt", "w")

def x(node):
	global stateIndex, t
	currentStateIndex = stateIndex
	
	for letter, target in node.children.items():
		if letter == "</w>":
			print(node.index, node.children["</w>"][0], "</w>", node.children["</w>"][1], file=t)
			#print(node.children["</w>"][0], file=t)
		else:
			print(node.index, node.children[letter].index, letter, "<eps>", file=t)
			x(node.children[letter])
			
x(root)
print(0, stateIndex, "</s>", "</s>", file=t)
print(stateIndex, file=t)

t.close()

os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../word_symbols.txt L.txt | fstarcsort > L.fst")
#os.system("fstdraw --isymbols=../letter_symbols.txt --osymbols=../word_symbols.txt < L.fst | dot -Tps > L.ps") #takes too long time

#os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../word_symbols.txt test_L.txt | fstarcsort > test_L.fst")
#os.system("fstdraw --isymbols=../letter_symbols.txt --osymbols=../word_symbols.txt < test_L.fst | dot -Tps > test_L.ps")


print("finished!")
