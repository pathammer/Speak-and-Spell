#!/usr/bin/python
filename1 = "test.txt"
filename2 = "test2.txt"

file = open(filename1,'r')
output = open(filename2,'w')

while(True):
	line = file.readline()
	if len(line) == 0:
		break
	line = line.lower()
	sss = ""	
	for char in line:
		if char in "!.?":
			sss += "\n"
		elif char in "abcdefghijklmnopqrstuvwxyz ":
			sss += char
		else:
			pass
	output.write(sss)

		
			
