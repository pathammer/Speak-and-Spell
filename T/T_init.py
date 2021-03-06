#!/usr/bin/env python3

# state state input output weight

#fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt transitions.txt > test.fst
#fstdraw --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt < test.fst | dot -Tps > test.ps

import os
import string


t = open("T_init.txt", "w")

stateNumber = 1
for input in string.ascii_lowercase:
	for output in string.ascii_lowercase:
		if input == output:
			print(0, 0, input, output, 0, file=t)
		else:
			print(0, 0, input, output, 1, file=t)
#			print(0, stateNumber, input, output, 1, file=t)
#			print(stateNumber, 0, output, input, 0, file=t)
			
			
for input in string.ascii_lowercase:
	print(0, 0, input, '<eps>', 1, file=t)

for output in string.ascii_lowercase:
	print(0, 0, '<eps>', output, 1, file=t)

print(0, 0, '</w>','</w>', file=t)
print(0, 0, '</s>','</s>', file=t)
print(0, file=t)


t.close()

os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt T_init.txt > T_init.fst")
os.system("fstdraw --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt < T_init.fst | dot -Tps > T_init.ps")

print("finished!")


#q("hello","in.txt")
#q("helol","out.txt")
#os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt in.txt > in.fst")
#os.system("fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt out.txt > out.fst")
#os.system("fstcompose in.fst T.fst > first.fst")
#os.system("fstcompose first.fst out.fst > second.fst")
