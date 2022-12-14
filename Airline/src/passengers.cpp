#include "passengers.h"
#include "ui_passengers.h"

Passengers::Passengers(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Passengers) {
  ui->setupUi(this);

  QObject::connect(ui->btnClose, SIGNAL(clicked(bool)), this,
                   SLOT(CloseBaseDataPassengers()));
  QObject::connect(ui->btnAdd, SIGNAL(clicked(bool)), this,
                   SLOT(InsertBaseDataPassengers()));
  QObject::connect(ui->btnDelete, SIGNAL(clicked(bool)), this,
                   SLOT(DeleteBaseDataPassengers()));
  QObject::connect(ui->btnUpdate, SIGNAL(clicked(bool)), this,
                   SLOT(UpdateBaseDataPassengers()));
  QObject::connect(ui->actionClearDataBase, SIGNAL(triggered(bool)), this,
                   SLOT(ToEmptyPassengers()));
  QObject::connect(ui->actionOrderBy, SIGNAL(triggered(bool)), this,
                   SLOT(OrderByPassengers()));
  QObject::connect(ui->actionFindRecord, SIGNAL(triggered(bool)), this,
                   SLOT(FindRecordPassengers()));

  this->setWindowTitle("Passengers table");
}

Passengers::~Passengers() {
  delete ui;
  delete model;
}

auto Passengers::ClearLine() noexcept -> void {
  ui->LineCodePassenger->clear();
  ui->LineDateOfBirth->clear();
  ui->LineFatherland->clear();
  ui->LineGender->clear();
  ui->LineName->clear();
  ui->LinePassport->clear();
  ui->LineSurname->clear();
}

auto Passengers::ConnectPassengers() -> void {
  const QString path = "./PassengerA.db";
  p_db = QSqlDatabase::addDatabase("QSQLITE");
  p_db.setDatabaseName(path);

  if (!p_db.open()) {
    log.error("Cannot open dataBase" + p_db.lastError().text());
  } else {
    log.info("Connect to DataBase successfully,::Passengers(...)");
    CreateTableBaseDataPassengers();
  }
}

auto Passengers::CreateTableBaseDataPassengers() noexcept -> void {
  const QString table = "CREATE TABLE Passengers ( "
                        "Name VARCHAR(15),"
                        "Surname VARCHAR(15),"
                        "Fatherland VARCHAR(15),"
                        "Passport VARCHAR(15),"
                        "Birth DATE,"
                        "Gender VARCHAR(5),"
                        "Code SMALLINT UNSIGNED);";

  QSqlQuery query(p_db);

  if (!query.exec(table)) {
    log.error(query.lastError().text() + " ::CreateTablePassengers(...)");
  }

  model = new QSqlTableModel(this);
  model->setTable("Passengers");
  model->select();
  ui->tableView->setModel(model);
}

void Passengers::InsertBaseDataPassengers() {
  const QString name = ui->LineName->text();
  const QString surname = ui->LineSurname->text();
  const QString fatherland = ui->LineFatherland->text();
  const QString passport = ui->LinePassport->text();
  const QString birth_date = ui->LineDateOfBirth->text();
  const QString gender = ui->LineGender->text();
  const QString code = ui->LineCodePassenger->text();

  const QString insert = "INSERT INTO Passengers "
                         "(name,surname,fatherland,passport,birth,gender,code) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?)";
  model->select();

  QSqlQuery query(p_db);
  if (query.prepare(insert)) {
    query.addBindValue(name);
    query.addBindValue(surname);
    query.addBindValue(fatherland);
    query.addBindValue(passport);
    query.addBindValue(birth_date);
    query.addBindValue(gender);
    query.addBindValue(code);
    query.exec();
  } else {
    log.error(query.lastError().text() + " ::InsertPassengers(...)");
  }

  ClearLine();
}

auto Passengers::ChoiceUpdatePassengers() noexcept -> QString {
  const QString text = QInputDialog::getText(
      this, tr("Enter new code for update "), tr("Enter:"), QLineEdit::Normal,
      QDir::home().dirName());

  return text;
}

void Passengers::UpdateBaseDataPassengers() {
  const QString update_old = ChoiceUpdatePassengers();
  const QString old_code = ui->LineCodePassenger->text();
  const QString update =
      "UPDATE Passengers SET code=:update_old WHERE code=:old_code";

  if (ui->LineCodePassenger->text().isEmpty()) {
    QMessageBox::warning(this, "The Error",
                         "You did not enter a name that you have updated");
  }

  QSqlQuery query(p_db);

  query.prepare(update);
  query.bindValue(":update_old", update_old);
  query.bindValue(":old_code", old_code);

  if (!query.exec()) {
    log.error(query.lastError().text() + " ::UpdatePassengers(...)");
  } else {
    log.info(update_old + " Success update on -> " + old_code);
  }

  model->select();

  ClearLine();
}

void Passengers::DeleteBaseDataPassengers() {
  const QString delete_code = ui->LineCodePassenger->text();
  const QString delete_tb = "DELETE FROM Passengers WHERE code=:code";

  if (ui->LineCodePassenger->text().isEmpty()) {
    QMessageBox::warning(this, "The Error",
                         "You did not enter code that you have delete");
  }

  QSqlQuery query(p_db);
  query.prepare(delete_tb);
  query.bindValue(":code", delete_code);

  if (!query.exec()) {
    log.error(query.lastError().text() + " ::DeletePassengers(...)");
  } else {
    log.info(delete_code + " Removed successfully");
  }

  model->select();

  ClearLine();
}

bool Passengers::CloseBaseDataPassengers() {
  if (p_db.isOpen()) {
    p_db.close();
    QSqlDatabase::removeDatabase("Passengers");
    return true;
  } else {
    return false;
  }
}

bool Passengers::OrderByPassengers() {

  QSqlQuery query(p_db);
  if (query.exec("SELECT * FROM Passengers ORDER BY Code DESC, Name ASC")) {
    log.error(query.lastError().text() + " ::OrderByPassengers(...)");
    return false;
  } else {
    model->select();
    return true;
  }
}

bool Passengers::ToEmptyPassengers() {

  QSqlQuery query(p_db);
  if (query.exec("DROP FROM Passengers")) {
    log.error(query.lastError().text() + " ::ToEmptyPassengers(...)");
    return false;
  } else {
    model->select();
    return true;
  }
}
