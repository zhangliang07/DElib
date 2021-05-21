#include "evomaindialog.h"
#include <QApplication>


int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	EvoMainDialog w;
	w.show();
	return a.exec();
}
