#include "lolstats.h"
#include <QtWidgets/QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	LoLStats w;
	w.show();
	return a.exec();
}
