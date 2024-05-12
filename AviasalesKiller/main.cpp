#include <QApplication>

#include "autorizationwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  AutorizationWindow w;
  w.show();
  return a.exec();
}
