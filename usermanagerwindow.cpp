#include "usermanagerwindow.h"
#include "ui_usermanagerwindow.h"
#include "databasemanager.h"
#include "dashboardwindow.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QAbstractAnimation>

UserManagerWindow::UserManagerWindow(const QString &username,
                                     const QString &role,
                                     QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UserManagerWindow)
    , backgroundLabel(nullptr)
    , currentUsername(username)
    , currentRole(role)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
    setWindowTitle("Challenge 30 - User Manager");

    backgroundLabel = new QLabel(this);
    QString imagePath = QCoreApplication::applicationDirPath() + "/../../images/background.png";
    QPixmap bg(imagePath);
    backgroundLabel->setPixmap(bg);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(this->rect());
    backgroundLabel->lower();

    ui->centralwidget->setStyleSheet("background: transparent;");

    ui->frameUserManager->setStyleSheet(
        "#frameUserManager {"
        "background-color: rgba(0, 0, 0, 95);"
        "border-radius: 18px;"
        "}"
        );

    ui->labelTitle->setText("User Manager");
    ui->labelTitle->setStyleSheet(
        "color: white;"
        "font-size: 28px;"
        "font-weight: bold;"
        );

    ui->tableWidgetUsers->setColumnCount(3);
    ui->tableWidgetUsers->setHorizontalHeaderLabels(
        QStringList() << "ID" << "Username" << "Role"
        );

    ui->tableWidgetUsers->setStyleSheet(
        "QTableWidget {"
        "background-color: rgba(255, 255, 255, 230);"
        "color: #0f172a;"
        "border: none;"
        "border-radius: 10px;"
        "gridline-color: #cbd5e1;"
        "font-size: 14px;"
        "}"
        "QHeaderView::section {"
        "background-color: #2563eb;"
        "color: white;"
        "padding: 8px;"
        "border: none;"
        "font-weight: bold;"
        "}"
        );

    ui->tableWidgetUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetUsers->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidgetUsers->verticalHeader()->setVisible(false);
    ui->tableWidgetUsers->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetUsers->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetUsers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetUsers->setAlternatingRowColors(true);

    QString blueButtonStyle =
        "QPushButton {"
        "background-color: #2563eb;"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 10px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1d4ed8;"
        "}";

    QString darkButtonStyle =
        "QPushButton {"
        "background-color: #111827;"
        "color: white;"
        "border: 1px solid white;"
        "border-radius: 10px;"
        "padding: 10px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1f2937;"
        "}";

    ui->pushButtonRefresh->setStyleSheet(blueButtonStyle);
    ui->pushButtonPromote->setStyleSheet(blueButtonStyle);
    ui->pushButtonDemote->setStyleSheet(blueButtonStyle);
    ui->pushButtonDelete->setStyleSheet(darkButtonStyle);
    ui->pushButtonBack->setStyleSheet(darkButtonStyle);

    loadUsers();

    this->setWindowOpacity(0.0);

    QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setDuration(400);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    this->showMaximized();
}

UserManagerWindow::~UserManagerWindow()
{
    delete ui;
}

void UserManagerWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (backgroundLabel) {
        backgroundLabel->setGeometry(this->rect());
    }
}

bool UserManagerWindow::isValidUserRow(int row) const
{
    if (row < 0) {
        return false;
    }

    QTableWidgetItem *idItem = ui->tableWidgetUsers->item(row, 0);
    return idItem != nullptr;
}

void UserManagerWindow::loadUsers()
{
    ui->tableWidgetUsers->clearContents();
    ui->tableWidgetUsers->setRowCount(0);

    QSqlQuery query("SELECT id, username, role FROM users ORDER BY username");

    int row = 0;
    while (query.next()) {
        ui->tableWidgetUsers->insertRow(row);

        for (int col = 0; col < 3; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidgetUsers->setItem(row, col, item);
        }

        row++;
    }
}

void UserManagerWindow::on_pushButtonRefresh_clicked()
{
    loadUsers();
}

void UserManagerWindow::on_pushButtonPromote_clicked()
{
    int row = ui->tableWidgetUsers->currentRow();
    if (!isValidUserRow(row)) {
        QMessageBox::warning(this, "User Manager", "Please select a user first.");
        return;
    }

    QString username = ui->tableWidgetUsers->item(row, 1)->text();
    QString role = ui->tableWidgetUsers->item(row, 2)->text();

    if (role == "admin") {
        QMessageBox::information(this, "User Manager", "This user is already an admin.");
        return;
    }

    if (DatabaseManager::updateUserRole(username, "admin")) {
        QMessageBox::information(this, "Success", username + " is now an admin.");
        loadUsers();
    } else {
        QMessageBox::warning(this, "Error", "Failed to update user role.");
    }
}

void UserManagerWindow::on_pushButtonDemote_clicked()
{
    int row = ui->tableWidgetUsers->currentRow();
    if (!isValidUserRow(row)) {
        QMessageBox::warning(this, "User Manager", "Please select a user first.");
        return;
    }

    QString username = ui->tableWidgetUsers->item(row, 1)->text();
    QString role = ui->tableWidgetUsers->item(row, 2)->text();

    if (role == "user") {
        QMessageBox::information(this, "User Manager", "This user is already a normal user.");
        return;
    }

    if (username == currentUsername) {
        QMessageBox::warning(this, "Access Denied", "You cannot demote yourself.");
        return;
    }

    if (DatabaseManager::updateUserRole(username, "user")) {
        QMessageBox::information(this, "Success", username + " is now a normal user.");
        loadUsers();
    } else {
        QMessageBox::warning(this, "Error", "Failed to update user role.");
    }
}

void UserManagerWindow::on_pushButtonDelete_clicked()
{
    int row = ui->tableWidgetUsers->currentRow();
    if (!isValidUserRow(row)) {
        QMessageBox::warning(this, "User Manager", "Please select a user first.");
        return;
    }

    QString username = ui->tableWidgetUsers->item(row, 1)->text();

    if (username == currentUsername) {
        QMessageBox::warning(this, "Access Denied", "You cannot delete your own account.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Delete User",
        "Are you sure you want to delete user: " + username + "?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::No) {
        return;
    }

    if (DatabaseManager::deleteUser(username)) {
        QMessageBox::information(this, "Success", "User deleted successfully.");
        loadUsers();
    } else {
        QMessageBox::warning(this, "Error", "Failed to delete user.");
    }
}

void UserManagerWindow::on_pushButtonBack_clicked()
{
    DashboardWindow *dashboard = new DashboardWindow(currentUsername, currentRole);
    dashboard->show();
    this->close();
}
