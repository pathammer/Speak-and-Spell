#!/usr/bin/python

alphabets = ['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z']

Finalstate = 0

state = 1
lines = []
for go in range(len(alphabets)):
    for back in range(len(alphabets)):
        if alphabets[go] == alphabets[back]:
            continue;
        line = str(Finalstate) + ' ' + str(state) + ' ' + alphabets[go] + ' ' + alphabets[back] + ' 1 \n'
        lines.append(line)
        line = str(state) + ' ' + str(Finalstate) + ' ' + alphabets[back] + ' ' + alphabets[go] + ' 0 \n'
        lines.append(line)
        state += 1

f = open('swapTransitions.txt' ,'w')
f.writelines(lines)
f.close()
