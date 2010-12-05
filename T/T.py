#!/usr/bin/env python3

# state state input output weight

#fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt transitions.txt > test.fst
#fstdraw --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt < test.fst | dot -Tps > test.ps

import os
import string

t = open("transitions.txt", "w")

stateNumber = 1
for input in string.ascii_lowercase:
	for output in string.ascii_lowercase:
		if input == output:
			print(0, 0, input, output, 0, file=t)
		else:
			print(0, 0, input, output, 1, file=t)
			print(0, stateNumber, input, output, 1, file=t)
			print(stateNumber, 0, output, input, 0, file=t)
			
			
for input in string.ascii_lowercase:
	print(0, 0, input, '<eps>', 1, file=t)

for output in string.ascii_lowercase:
	print(0, 0, '<eps>', output, 1, file=t)

print(0, file=t)


t.close()

os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt transitions.txt > T.fst")
os.system("fstdraw --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt < T.fst | dot -Tps > T.ps")

print("finished!")


def q(word, fileName):
	f = open(fileName, "w")
	stateNumber = 0
	for letter in word:
		print(stateNumber, stateNumber+1, letter, letter, file=f)
		stateNumber = stateNumber + 1
	print(stateNumber, file=f)

q("hello","in.txt")
q("helol","out.txt")
os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt in.txt > in.fst")
os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt out.txt > out.fst")
os.system("fstcompose in.fst T.fst > first.fst")
os.system("fstcompose first.fst out.fst > second.fst")