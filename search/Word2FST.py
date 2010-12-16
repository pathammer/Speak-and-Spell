import os
import string
from itertools import *
import sys

testName = sys.argv[1]
fstName = sys.argv[2]


def wordTransitions(word, fileName):
	f = open(fileName, "w")
	stateNumber = 0
	for letter in word:
		print(stateNumber, stateNumber+1, letter, letter, file=f)
		stateNumber = stateNumber + 1
	print(stateNumber, file=f)
	f.close()

wordTransitions(testName, fstName)
