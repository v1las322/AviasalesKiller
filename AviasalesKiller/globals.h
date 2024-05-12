#ifndef GLOBALS_H
#define GLOBALS_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

bool getSqlConnection(QSqlDatabase);
int showMessageBox(QString, QString, QMessageBox::Button, QMessageBox::Icon);
int showMessageBox(QString, QString, QList<QMessageBox::Button>,
                   QMessageBox::Icon);

#endif  // GLOBALS_H
