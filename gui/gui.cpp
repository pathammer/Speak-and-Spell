#include "gui.h"



gui::gui(QWidget *parent)
    : QMainWindow(parent)
{
	init();
	ui.setupUi(this);
}

gui::~gui()
{
	cleanup();
}
