#ifndef REGISTRATIONWINDOW_H
#define REGISTRATIONWINDOW_H

#include <QMainWindow>

#include "qsqldatabase.h"

namespace Ui {
class RegistrationWindow;
}

class RegistrationWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit RegistrationWindow(QWidget* parent = nullptr);
  ~RegistrationWindow();
 private slots:
  void on_Registration_Button_clicked();

 private:
  Ui::RegistrationWindow* ui;
  QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "registration");
};

#endif  // REGISTRATIONWINDOW_H
