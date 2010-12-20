#ifndef GUI_H
#define GUI_H

#include <QtGui/QMainWindow>
#include "ui_gui.h"

#include <iostream>
#include <string>
#include <sstream>
#include <spellchecklib.h>

using namespace std;

class gui: public QMainWindow {
Q_OBJECT

public:
	gui(QWidget *parent = 0);
	~gui();

private:
	Ui::guiClass ui;
public slots:
	void CorrectAll() {
		string text = ui.textEdit->document()->toPlainText().toStdString();
		vector<vector<string> > refined = RefineText(text);
		cout << text;
		string full;
		for (int i = 0; i < refined.size(); ++i) {
			vector<string> result = GetBestSentenceCandidate(refined[i], ui.doubleSpinBox->value());
			for (int i = 0; i < result.size(); ++i)
				full = full + result[i] + " ";
			full = full + "\n\n";
		}
		ui.textEdit_2->document()->setPlainText(full.c_str());
	}
	void CorrectWord() {
		string text = ui.textEdit->document()->toPlainText().toStdString();
		vector<string> refined = RefineText(text, ui.textEdit->textCursor().position());
		vector<string> refined2 = refined;
		refined2.pop_back();

		vector<WordWeight> ww = GetSingleWordCandidates(refined2, refined.back(),
				ui.doubleSpinBox->value());
		ui.listWidget->clear();
		for (int i = 0; i < ww.size(); ++i) {
			//cout << SYMBOL.Word->Find(ww[i].word).c_str() << "\n";
			stringstream ss;// (stringstream::in | stringstream::out);
			ss << ww[i].weight;


			ui.listWidget->addItem((SYMBOL.Word->Find(ww[i].word)+ "\t" + ss.str()).c_str());
		}
	}
};

#endif // GUI_H
