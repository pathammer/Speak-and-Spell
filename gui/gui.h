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
	map<float, int> SortWordWeight(vector<WordWeight> ww) {
		map<int, float> mymap;
		for (int i = 0; i < ww.size(); ++i) {
			//cout << mymap[ww[i].word];
			if (mymap[ww[i].word] == 0 || mymap[ww[i].word] > ww[i].weight) {
				mymap[ww[i].word] = ww[i].weight;
			}
		}
		map<float, int> mymap2;
		for (map<int, float>::iterator i = mymap.begin(); i != mymap.end(); ++i) {
			mymap2[i->second] = i->first;
		}
		return mymap2;

	}
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
		map<int, float> mymap;
		for (int i = 0; i < ww.size(); ++i) {
			//cout << mymap[ww[i].word];
			if (mymap[ww[i].word] == 0 || mymap[ww[i].word] > ww[i].weight) {
				mymap[ww[i].word] = ww[i].weight;
			}
		}
		map<float, int> mymap2;
		for (map<int, float>::iterator i = mymap.begin(); i != mymap.end(); ++i) {
			mymap2[i->second] = i->first;

		}
		for (map<float, int>::iterator i = mymap2.begin(); i != mymap2.end(); ++i) {
			stringstream ss;// (stringstream::in | stringstream::out);
			ss << i->first;
			ui.listWidget->addItem((SYMBOL.Word->Find(i->second) + "\t" + ss.str()).c_str());
		}
	}
	string ColorText(string text, float colorscale){
		colorscale -= 0;
		colorscale *= 15;
		int intcolorscale = max(min((int)colorscale,255),0);
		char *str = new char[8];
		sprintf(str, "%lX", (unsigned long)(intcolorscale));
		return string() + "<font color=\"#" + str + "0000\">" + text + "</font>";
	}
	void Analysis_() {

		string text = ui.textEdit->document()->toPlainText().toStdString();
		vector<vector<string> > refined = RefineText(text);
		vector<vector<WordWeight> > result =
				Analysis(refined[0], (float) ui.doubleSpinBox->value());
		string final;
		for (int i = 0; i < result.size(); ++i) {
			map<float, int> mymap = SortWordWeight(result[i]);
			stringstream ss;// (stringstream::in | stringstream::out);
			ss << mymap.begin()->first;
			final += ColorText(refined[0][i],mymap.begin()->first) + " ";
		}
		//ui.textEdit_2->document()->setPlainText(final.c_str());
		ui.textEdit_2->document()->setHtml(final.c_str());
	}
};

#endif // GUI_H
