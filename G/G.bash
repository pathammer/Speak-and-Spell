#!/bin/bash

filename=train
farcompilestrings -i ../word_symbols.txt $filename.txt > $filename.far

ngram=3
grmcount -n$ngram -s 2 -f 3 $filename.far > $filename.$ngram.counts.fsm
grmmake $filename.$ngram.counts.fsm > $filename.$ngram.lm.fsm
fsmprint $filename.$ngram.lm.fsm | gawk '{print $1 "\t" $2 "\t" $3 "\t" $3 "\t" $4}' |fstcompile > $filename.$ngram.lm.fst
#this takes too long time
#fstdraw --isymbols=../word_symbols.txt --osymbols=../word_symbols.txt < $filename.$ngram.lm.fst | dot -Tps > $filename.$ngram.lm.ps

rm $filename.$ngram.counts.fsm
rm $filename.$ngram.lm.fsm



##
rm $filename.far