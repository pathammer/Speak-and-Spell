#!/usr/bin/env python3

# state state input output weight

#fstcompile --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt transitions.txt > test.fst
#fstdraw --isymbols=../letter_symbols.txt --osymbols=../letter_symbols.txt < test.fst | dot -Tps > test.ps

import os
import string
from itertools import *
from math import *
import sys

os.system("./T_init.py")
os.system("./T_count.py T_init.fst tmp_count0.txt")

for i in range(0, 10):
	os.system("./T_newbasedcount.py tmp_count" + str(i) + ".txt tmp_T" + str(i))
	os.system("./T_count.py tmp_T" + str(i) + ".fst tmp_count" + str(i + 1) + ".txt")
