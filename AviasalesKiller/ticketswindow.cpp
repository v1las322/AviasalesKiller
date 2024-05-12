#include "ticketswindow.h"

#include "globals.h"
#include "ui_ticketswindow.h"

TicketsWindow::TicketsWindow(const QString &login, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::TicketsWindow), UserLogin(login) {
  ui->setupUi(this);
  db = QSqlDatabase::database("buytickets");
  if (getSqlConnection(db)) {
    QSqlQuery query(db);
    ui->user->setText("Имя пользователя: " + UserLogin);
    ui->user->adjustSize();
    query.prepare("SELECT Email FROM Users WHERE Username = ?");
    query.addBindValue(UserLogin);
    query.exec();
    query.next();
    QString UserEmail = query.value(0).toString();
    ui->mail->setText("Email пользователя: " + UserEmail);
    ui->mail->adjustSize();
  }
  on_All_Tickets_Button_clicked();
  OutputBuyTicketTable();
}

TicketsWindow::~TicketsWindow() { delete ui; }

void TicketsWindow::on_All_Tickets_Button_clicked() {
  QSqlQuery query(db);
  QString sqlQuery =
      "SELECT StartPoint, FinishPoint, StartDate, FinishDate, PriceTicket, "
      "CountTicket FROM Flight WHERE 1=1";
  QString fromPoint = ui->From_Edit->text().toLower();
  QString toPoint = ui->To_Edit->text().toLower();

  if (!fromPoint.isEmpty()) {
    sqlQuery += " AND LOWER(StartPoint) LIKE '%" + fromPoint + "%'";
  }
  if (!toPoint.isEmpty()) {
    sqlQuery += " AND LOWER(FinishPoint) LIKE '%" + toPoint + "%'";
  }
  query.prepare(sqlQuery);
  query.exec();
  OutputTicketsTable(query);
}

void TicketsWindow::on_Search_Button_clicked() {
    if(!ui->CheckData->isChecked()) {
      QSqlQuery query(db);
      QString fromPoint = ui->From_Edit->text().toLower();
      QString toPoint = ui->To_Edit->text().toLower();
      QDate selectedDate = ui->Date_Edit->date();

      QString sqlQuery =
          "SELECT StartPoint, FinishPoint, StartDate, FinishDate, PriceTicket, "
          "CountTicket FROM Flight WHERE 1=1";

      if (selectedDate.isValid()) {
        sqlQuery +=
            " AND DATE(StartDate) = '" + selectedDate.toString("yyyy-MM-dd") + "'";
      }
      if (!fromPoint.isEmpty()) {
        sqlQuery += " AND LOWER(StartPoint) LIKE '%" + fromPoint + "%'";
      }
      if (!toPoint.isEmpty()) {
        sqlQuery += " AND LOWER(FinishPoint) LIKE '%" + toPoint + "%'";
      }
      query.prepare(sqlQuery);
      query.exec();
      OutputTicketsTable(query);
    } else {
        on_All_Tickets_Button_clicked();
    }
}

void TicketsWindow::OutputTicketsTable(QSqlQuery &query) {
  if (query.exec()) {
    QStandardItemModel *model = new QStandardItemModel();

    int columnCount = 7;
    model->setColumnCount(columnCount);

    QStringList headers = {"Отправление",   "Прибытие", "Дата отправления",
                           "Дата прибытия", "Цена",     "Мест",
                           "Покупка"};
    model->setHorizontalHeaderLabels(headers);

    int row = 0;
    while (query.next()) {
      for (int column = 0; column < columnCount - 1; column++) {
        if (column == 2 || column == 3) {
          QDateTime dateTime = query.value(column).toDateTime();
          QString formattedDateTime = dateTime.toString("yyyy-MM-dd hh:mm");
          QStandardItem *item = new QStandardItem(formattedDateTime);
          model->setItem(row, column, item);

        } else {
          QString cellValue = query.value(column).toString();
          QStandardItem *item = new QStandardItem(cellValue);
          model->setItem(row, column, item);
        }
      }
      ui->Flying_Table->setModel(model);
      CreateButtonsBuyTicket(columnCount, row, model);
      row++;
    }
    ui->Flying_Table->resizeColumnsToContents();
    ui->Flying_Table->setModel(model);

  } else {
    showMessageBox("Ошибка не удалось открыть список рейсов.", "Неудачно",
                   QMessageBox::Ok, QMessageBox::Warning);
  }
}

void TicketsWindow::CreateButtonsBuyTicket(int columnCount, int row,
                                           QStandardItemModel *model) {
  QPushButton *button = new QPushButton("Купить");
  ui->Flying_Table->setIndexWidget(model->index(row, columnCount - 1), button);
  connect(button, &QPushButton::clicked, this, &TicketsWindow::AddBuying);
}

int TicketsWindow::GetUserID(QSqlQuery &query) {
  query.prepare("SELECT UserID FROM Users WHERE Username = ?");
  query.addBindValue(UserLogin);
  query.exec();
  query.next();
  return query.value(0).toInt();
}

int TicketsWindow::GetFlyID(QString StartDate, QString FinishDate,
                            QString StartPoint, QString FinishPoint) {
  QSqlQuery query(db);

  query.prepare(
      "SELECT FlyID FROM `Flight` "
      "WHERE `StartDate` = ? "
      "AND FinishDate = ? "
      "AND StartPoint = ? "
      "AND FinishPoint = ?");
  query.addBindValue(StartDate);
  query.addBindValue(FinishDate);
  query.addBindValue(StartPoint);
  query.addBindValue(FinishPoint);
  if (!query.exec()) {
    showMessageBox("Неудалось получить id рейса", "Неудачно", QMessageBox::Ok,
                   QMessageBox::Warning);
  }
  query.next();
  return query.value(0).toInt();
}

void TicketsWindow::AddBuying() {
  QSqlQuery query(db);
  QModelIndex index = ui->Flying_Table->currentIndex();
  QStandardItemModel *model =
      qobject_cast<QStandardItemModel *>(ui->Flying_Table->model());
  QString StartDate = model->data(model->index(index.row(), 2)).toString();
  QString FinishDate = model->data(model->index(index.row(), 3)).toString();
  QString StartPoint = model->data(model->index(index.row(), 0)).toString();
  QString FinishPoint = model->data(model->index(index.row(), 1)).toString();
  int FlyID = GetFlyID(StartDate, FinishDate, StartPoint, FinishPoint);
  int UserID = GetUserID(query);
  QString Seat = GetFreeSeat(FlyID);
  query.prepare(
      "INSERT INTO `Tickets` (`UserID`, `FlyID`, `Seat`) "
      "VALUES (?, ?, ?)");
  query.addBindValue(UserID);
  query.addBindValue(FlyID);
  query.addBindValue(Seat);
  if (!query.exec()) {
    showMessageBox("Ошибка при покупке билета", "Неудачно", QMessageBox::Ok,
                   QMessageBox::Warning);
  } else {
    query.prepare("UPDATE Flight SET CountTicket = ? WHERE FlyID = ?");
    query.addBindValue(GetCountFreeSeat(FlyID));
    query.addBindValue(FlyID);
    query.exec();
    on_Search_Button_clicked();
    OutputBuyTicketTable();
  }
}

int TicketsWindow::GetCountFreeSeat(int FlyID) {
  QSqlQuery query(db);
  query.prepare(
      "SELECT COUNT(*) AS CountFreeSeat "
      "FROM (SELECT SeatFly.Seat FROM SeatFly "
      "EXCEPT "
      "SELECT SeatFly.Seat FROM SeatFly "
      "JOIN Tickets ON SeatFly.Seat = Tickets.Seat AND `FlyID` = ?) "
      "AS CountFreeSeat");
  query.addBindValue(FlyID);
  if (!query.exec()) {
    showMessageBox("Не прошел запрос на поиск свободных мест", "Неудачно",
                   QMessageBox::Ok, QMessageBox::Warning);
  }
  query.next();
  return query.value(0).toInt();
}

QString TicketsWindow::GetFreeSeat(int FlyID) {
  QSqlQuery query(db);
  query.prepare(
      "SELECT SeatFly.Seat "
      "FROM SeatFly "
      "EXCEPT "
      "SELECT SeatFly.Seat "
      "FROM SeatFly "
      "JOIN Tickets ON SeatFly.Seat = Tickets.Seat AND `FlyID` = ?");
  query.addBindValue(FlyID);
  if (!query.exec()) {
    showMessageBox("Неудалось отправить запрос на получение свободного места",
                   "Неудачно", QMessageBox::Ok, QMessageBox::Warning);
  }
  if (!query.next()) {
    showMessageBox("Свободных мест нет", "Неудачно", QMessageBox::Ok,
                   QMessageBox::Warning);
  }
  return query.value(0).toString();
}

void TicketsWindow::OutputBuyTicketTable() {
  QSqlQuery query(db);
  int UserID = GetUserID(query);
  query.prepare(
      "SELECT StartPoint, FinishPoint, StartDate, FinishDate, PriceTicket, "
      "SeatFly.Seat "
      "FROM Flight "
      "JOIN Tickets ON Flight.FlyID = Tickets.FlyID "
      "JOIN SeatFly ON Tickets.Seat = SeatFly.Seat "
      "WHERE Tickets.UserID = ? ");
  query.addBindValue(UserID);
  query.exec();
  QStandardItemModel *model = new QStandardItemModel();
  int columnCount = 7;
  model->setColumnCount(columnCount);
  QStringList headers = {"Отправление",   "Прибытие", "Дата отправления",
                         "Дата прибытия", "Цена",     "Местo",
                         "Вернуть"};
  model->setHorizontalHeaderLabels(headers);
  int row = 0;
  while (query.next()) {
    for (int column = 0; column < columnCount - 1; column++) {
      if (column == 2 || column == 3) {
        QDateTime dateTime = query.value(column).toDateTime();
        QString formattedDateTime = dateTime.toString("yyyy-MM-dd hh:mm");
        QStandardItem *item = new QStandardItem(formattedDateTime);
        model->setItem(row, column, item);
      } else {
        QString cellValue = query.value(column).toString();
        QStandardItem *item = new QStandardItem(cellValue);
        model->setItem(row, column, item);
      }
    }
    ui->Buy_Ticket_Table->setModel(model);
    CreateButtonsReturnTicket(columnCount, row, model);
    row++;
  }
  ui->Buy_Ticket_Table->resizeColumnsToContents();
  ui->Buy_Ticket_Table->setModel(model);
}

void TicketsWindow::CreateButtonsReturnTicket(int columnCount, int row,
                                              QStandardItemModel *model) {
  QPushButton *button = new QPushButton("Вернуть");
  ui->Buy_Ticket_Table->setIndexWidget(model->index(row, columnCount - 1),
                                       button);
  connect(button, &QPushButton::clicked, this, &TicketsWindow::DeleteBuying);
}

void TicketsWindow::DeleteBuying() {
  QSqlQuery query(db);
  QModelIndex index = ui->Buy_Ticket_Table->currentIndex();
  QStandardItemModel *model =
      qobject_cast<QStandardItemModel *>(ui->Buy_Ticket_Table->model());
  QString StartDate = model->data(model->index(index.row(), 2)).toString();
  QString FinishDate = model->data(model->index(index.row(), 3)).toString();
  QString StartPoint = model->data(model->index(index.row(), 0)).toString();
  QString FinishPoint = model->data(model->index(index.row(), 1)).toString();
  QString Seat = model->data(model->index(index.row(), 5)).toString();
  int FlyID = GetFlyID(StartDate, FinishDate, StartPoint, FinishPoint);
  int UserID = GetUserID(query);
  query.prepare(
      "DELETE FROM Tickets WHERE UserID = ? AND FlyID = ? AND Seat = ?");
  query.addBindValue(UserID);
  query.addBindValue(FlyID);
  query.addBindValue(Seat);
  if (!query.exec()) {
    showMessageBox("Ошибка при возврате билета", "Неудачно", QMessageBox::Ok,
                   QMessageBox::Warning);
  } else {
    query.prepare("UPDATE Flight SET CountTicket = ? WHERE FlyID = ?");
    query.addBindValue(GetCountFreeSeat(FlyID));
    query.addBindValue(FlyID);
    query.exec();
    on_Search_Button_clicked();
    OutputBuyTicketTable();
  }
}
