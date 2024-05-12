#ifndef TICKETSWINDOW_H
#define TICKETSWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardItemModel>

namespace Ui {
class TicketsWindow;
}

class TicketsWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit TicketsWindow(const QString &login, QWidget *parent = nullptr);
  ~TicketsWindow();

 private slots:
  void on_Search_Button_clicked();

  void on_All_Tickets_Button_clicked();

 private:
  Ui::TicketsWindow *ui;
  QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "buytickets");
  QString UserLogin;

  void OutputTicketsTable(QSqlQuery &query);
  void CreateButtonsBuyTicket(int columnCount, int row,
                              QStandardItemModel *model);
  void CreateButtonsReturnTicket(int columnCount, int row,
                                 QStandardItemModel *model);
  void OutputBuyTicketTable();
  int GetUserID(QSqlQuery &query);
  int GetFlyID(QString StartDate, QString FinishDate, QString StartPoint,
               QString FinishPoint);
  int GetCountFreeSeat(int FlyID);
  QString GetFreeSeat(int FlyID);
  void AddBuying();
  void DeleteBuying();
};

#endif  // TICKETSWINDOW_H
