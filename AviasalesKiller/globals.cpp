#include "globals.h"

bool getSqlConnection(QSqlDatabase db) {
  db.setHostName("127.0.0.1");
  db.setPort(3306);
  db.setDatabaseName("ticket");
  db.setUserName("root");
  db.setPassword("password");
  return db.open();
}
int showMessageBox(QString message, QString title, QMessageBox::Button button,
                   QMessageBox::Icon icon) {
  QMessageBox msg;
  msg.setWindowTitle(title);
  msg.setText(message);
  msg.setDefaultButton(button);
  msg.setIcon(icon);
  return msg.exec();
}
int showMessageBox(QString message, QString title,
                   QList<QMessageBox::Button> buttons, QMessageBox::Icon icon) {
  QMessageBox msg;
  msg.setWindowTitle(title);
  msg.setText(message);
  for (QMessageBox::Button button : buttons) {
    msg.addButton(button);
  }
  msg.setDefaultButton(buttons.at(buttons.length() - 1));
  msg.setIcon(icon);
  return msg.exec();
}
