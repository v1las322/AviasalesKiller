#ifndef AUTORIZATIONWINDOW_H
#define AUTORIZATIONWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui {
class AutorizationWindow;
}
QT_END_NAMESPACE

class AutorizationWindow : public QMainWindow {
  Q_OBJECT

 public:
  AutorizationWindow(QWidget *parent = nullptr);
  ~AutorizationWindow();

  void tryAutorization(QString, QString);

 private slots:
  void on_Login_Button_clicked();

  void on_Registration_Button_clicked();

 private:
  Ui::AutorizationWindow *ui;
  QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "autorization");
};
#endif  // AUTORIZATIONWINDOW_H
