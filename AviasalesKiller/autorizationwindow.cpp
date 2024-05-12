#include "autorizationwindow.h"

#include "globals.h"
#include "registrationwindow.h"
#include "ticketswindow.h"
#include "ui_autorizationwindow.h"

AutorizationWindow::AutorizationWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AutorizationWindow) {
  ui->setupUi(this);
}

AutorizationWindow::~AutorizationWindow() { delete ui; }

void AutorizationWindow::tryAutorization(QString login, QString password) {
  db = QSqlDatabase::database("autorization");
  if (getSqlConnection(db)) {
    if (login.trimmed().isEmpty()) {
      showMessageBox("Поле 'Логин' должно быть заполнено.", "Действие прервано",
                     QMessageBox::Ok, QMessageBox::Warning);
      return;
    }
    if (password.trimmed().isEmpty()) {
      showMessageBox("Поле 'Пароль' должно быть заполнено.",
                     "Действие прервано", QMessageBox::Ok,
                     QMessageBox::Warning);
      return;
    }
    QSqlQuery query(db);
    query.prepare(
        "SELECT Username, Password FROM Users WHERE UserName = ? AND Password "
        "= ?");
    query.addBindValue(login);
    query.addBindValue(password);
    query.exec();
    if (query.next()) {
      TicketsWindow *mainForm = new TicketsWindow(login);
      this->hide();
      mainForm->show();
      while (mainForm->isVisible()) {
        QApplication::processEvents();
      }
      ui->Login_Edit->clear();
      ui->Password_Edit->clear();
      ui->Login_Edit->setFocus();
      this->show();
    } else {
      showMessageBox("Неверный логин или пароль.\nПользователь не обнаружен.",
                     "Неудачно", QMessageBox::Ok, QMessageBox::Warning);
    }
  } else {
    showMessageBox("Отсутствует подключение к БД.", "Действие прервано",
                   QMessageBox::Ok, QMessageBox::Critical);
  }
}

void AutorizationWindow::on_Login_Button_clicked() {
  QString login = ui->Login_Edit->text();
  QString password = ui->Password_Edit->text();

  tryAutorization(login, password);
}

void AutorizationWindow::on_Registration_Button_clicked() {
  RegistrationWindow *registrationForm = new RegistrationWindow();
  registrationForm->show();
}
