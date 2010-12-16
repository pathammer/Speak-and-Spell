#include <fst/fstlib.h>
#include <iostream>
#include <string>

class makeFst {

  struct Symbol
  {
	SymbolTable *Letter;
	SymbolTable *Word;
	Symbol()
	{
	  Letter = SymbolTable::ReadText("../letter_symbols.txt", false);
	  Word = SymbolTable::ReadText("../word_symbols.txt", false);
	}
  } SYMBOL;

  VectorFst<StdArc> StringToFst(char str[])
  {
	VectorFst < StdArc > result;
	VectorFst<StdArc>::StateId si1;
	VectorFst<StdArc>::StateId si2;
	
	si1 = result.AddState();
	si2 = si1;
	result.SetStart(si1);
	for (char *ptr = str; *ptr != '\0'; ++ptr)
	  {
		si2 = result.AddState();
		char letter[2] =
		  { *ptr, '\0' };
		VectorFst<StdArc>::Arc arc(SYMBOL.Letter->Find(letter),
								   SYMBOL.Letter->Find(letter), 0, si2);
		result.AddArc(si1, arc);
		si1 = si2;
	  }
	si2 = result.AddState();

	VectorFst<StdArc>::Arc arc(SYMBOL.Letter->Find("</w>"),
							   SYMBOL.Letter->Find("</w>"), 0, si2);
	result.AddArc(si1, arc);
	si1 = si2;

	si2 = result.AddState();

	VectorFst<StdArc>::Arc arc(SYMBOL.Letter->Find("</s>"),
							   SYMBOL.Letter->Find("</s>"), 0, si2);
	result.AddArc(si1, arc);
	si1 = si2;	

	result.SetFinal(si2, 0);

	//result.Write("test.fst");// for debug
	return result;

  }
};
