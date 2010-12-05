#!/usr/bin/env python3
# fsmcompile -i ../letter_symbols.txt  -o ../word_symbols.txt -t transitions.txt  > L.fsm
# fsmdraw -i ../letter_symbols.txt -o ../word_symbols.txt < L.fsm | dot -Tps > L.ps

import re

symbolList = []
g = open("../word_symbols.txt")
#g = open("labels.txt")

while True:
	tmp = g.readline().strip().split(' ')[0]
	if tmp == '':
		break   #EOF
	if re.match(".*[^a-z].*", tmp): #if there is non a-z letter, reject.
		continue
	symbolList.append(tmp)

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
	 
t = open("transitions.txt", "w")


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
print("finished!")
