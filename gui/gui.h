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
	void test() {
		init();

		string text = ui.textEdit->document()->toPlainText().toStdString();
		vector<vector<string> > refined = RefineText(text);

		////////////////////////////////////////////
//		vector<string> pass;
//		pass.push_back(refined[0][refined[0].size() - 3]);
//		pass.push_back(refined[0][refined[0].size() - 2]);
//		vector<WordWeight> result = GetSingleWordCandidates(pass, refined[0].back(), 2);
//
//		stringstream ss(stringstream::in | stringstream::out);
//		for (int i = 0; i < result.size(); ++i) {
//			ss << SYMBOL.Word->Find(result[i].word).c_str() << "\t" << result[i].weight << "\n";
//		}
		//////////////////////////////////////////
		GetBestSentenceCandidate(refined[0],0);
		/////////////////////////////////////////
		//ui.textEdit_2->document()->setPlainText(ss.str().c_str());

		cleanup();
	}

};

#endif // GUI_H
