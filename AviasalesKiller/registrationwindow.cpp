#include "registrationwindow.h"

#include "globals.h"
#include "ui_registrationwindow.h"

RegistrationWindow::RegistrationWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::RegistrationWindow) {
  ui->setupUi(this);
}

RegistrationWindow::~RegistrationWindow() { delete ui; }

void RegistrationWindow::on_Registration_Button_clicked() {
  QString login = ui->Login_Edit->text();
  QString email = ui->Email_Edit->text();
  QString password = ui->Password_Edit->text();

  db = QSqlDatabase::database("registration");
  if (getSqlConnection(db)) {
    if (login.isEmpty()) {
      showMessageBox("Поле 'Логин' должно быть заполнено.", "Действие прервано",
                     QMessageBox::Ok, QMessageBox::Warning);
      return;
    }
    if (email.isEmpty()) {
      showMessageBox("Поле 'Email' должно быть заполнено.", "Действие прервано",
                     QMessageBox::Ok, QMessageBox::Warning);
      return;
    }
    if (password.isEmpty()) {
      showMessageBox("Поле 'Пароль' должно быть заполнено.",
                     "Действие прервано", QMessageBox::Ok,
                     QMessageBox::Warning);
      return;
    }
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Users (Username, Password, Email) VALUES (?, ?, ?)");
    query.addBindValue(login);
    query.addBindValue(password);
    query.addBindValue(email);
    if (!query.exec()) {
      showMessageBox("Пользователь уже зарегистрирован", "Действие прервано",
                     QMessageBox::Ok, QMessageBox::Warning);
    } else {
      showMessageBox("Пользователь зарегистрирован", "Удачно", QMessageBox::Ok,
                     QMessageBox::Warning);
      this->close();
    }

  } else {
    showMessageBox("Отсутствует подключение к БД.", "Действие прервано",
                   QMessageBox::Ok, QMessageBox::Critical);
  }
}
