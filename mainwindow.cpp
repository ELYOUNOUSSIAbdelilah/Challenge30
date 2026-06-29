#include "mainwindow.h"
#include "registerdialog.h"
#include "dashboardwindow.h"
#include "ui_mainwindow.h"
#include "databasemanager.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QResizeEvent>
#include <QCoreApplication>
#include <QPixmap>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , backgroundLabel(nullptr)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));

    ui->frameContainer->setMinimumSize(900, 500);
    ui->frameImage->setMinimumWidth(420);
    ui->frameLogin->setMinimumWidth(420);

    ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    ui->pushButtonTogglePassword->setText("Show");

    ui->pushButtonTogglePassword->setStyleSheet(
        "QPushButton {"
        "background-color: #111827;"
        "color: white;"
        "border: 1px solid white;"
        "border-radius: 10px;"
        "padding: 6px 10px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1f2937;"
        "}"
        );

    connect(ui->lineEditUsername, &QLineEdit::returnPressed,
            this, &MainWindow::on_pushButtonLogin_clicked);

    connect(ui->lineEditPassword, &QLineEdit::returnPressed,
            this, &MainWindow::on_pushButtonLogin_clicked);

    ui->lineEditUsername->setPlaceholderText("Enter username");
    ui->lineEditPassword->setPlaceholderText("Enter password");

    ui->labelOverlayTitle->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 36px;"
        "font-weight: bold;"
        "background: transparent;"
        "letter-spacing: 1px;"
        "}"
        );

    ui->labelOverlaySubtitle->setStyleSheet(
        "QLabel {"
        "color: rgba(255, 255, 255, 200);"
        "font-size: 18px;"
        "font-weight: normal;"
        "background: transparent;"
        "font-style: italic;"
        "}"
        );

    ui->labelOverlayTitle->setText("Challenge 30");
    ui->labelOverlayTitle->setAlignment(Qt::AlignCenter);
    ui->labelOverlaySubtitle->setText("Test your knowledge");
    ui->labelOverlaySubtitle->setAlignment(Qt::AlignCenter);

    QString basePath = QCoreApplication::applicationDirPath();
    QString bgPath = basePath + "/../../images/background.png";
    QString leftImagePath = basePath + "/../../images/image.png";

    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(this->rect());
    backgroundLabel->lower();

    QPixmap bg(bgPath);
    if (!bg.isNull()) {
        backgroundLabel->setPixmap(bg);
    } else {
        qDebug() << "Failed to load background from disk";
    }

    ui->centralwidget->setStyleSheet("background: transparent;");

    QPixmap pix(leftImagePath);
    if (!pix.isNull()) {
        ui->labelImage->setText("");
        ui->labelImage->setAlignment(Qt::AlignCenter);

        QPixmap scaledPix = pix.scaled(
            ui->labelImage->size(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation
            );
        ui->labelImage->setPixmap(scaledPix);
    } else {
        qDebug() << "Failed to load left image from disk";
        ui->labelImage->setText("Image not found");
    }

    ui->horizontalLayout->setStretch(0, 2);
    ui->horizontalLayout->setStretch(1, 3);

    ui->frameLogin->setStyleSheet(
        "#frameLogin {"
        "background-color: rgba(255, 255, 255, 235);"
        "border-top-right-radius: 20px;"
        "border-bottom-right-radius: 20px;"
        "padding: 35px;"
        "}"
        );

    ui->frameImage->setStyleSheet(
        "#frameImage {"
        "border-top-left-radius: 20px;"
        "border-bottom-left-radius: 20px;"
        "background: transparent;"
        "}"
        );

    ui->frameOverlay->setStyleSheet(
        "#frameOverlay {"
        "background-color: transparent;"
        "}"
        );

    ui->pushButtonLogin->setStyleSheet(
        "QPushButton {"
        "background-color: #2563eb;"
        "color: white;"
        "border: none;"
        "border-radius: 12px;"
        "padding: 12px;"
        "font-size: 17px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1d4ed8;"
        "}"
        "QPushButton:pressed {"
        "background-color: #1e40af;"
        "}"
        );

    ui->pushButtonRegister->setStyleSheet(
        "QPushButton {"
        "background-color: #111827;"
        "color: white;"
        "border: 1px solid white;"
        "border-radius: 12px;"
        "padding: 12px;"
        "font-size: 17px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1f2937;"
        "}"
        "QPushButton:pressed {"
        "background-color: #0f172a;"
        "}"
        );

    ui->lineEditUsername->setStyleSheet(
        "QLineEdit {"
        "background-color: white;"
        "border: 2px solid #e5e7eb;"
        "border-radius: 14px;"
        "padding: 12px;"
        "font-size: 15px;"
        "color: #111827;"
        "}"
        "QLineEdit:focus {"
        "border: 2px solid #2563eb;"
        "}"
        );

    ui->lineEditPassword->setStyleSheet(
        "QLineEdit {"
        "background-color: white;"
        "border: 2px solid #e5e7eb;"
        "border-radius: 14px;"
        "padding: 12px;"
        "font-size: 15px;"
        "color: #111827;"
        "}"
        "QLineEdit:focus {"
        "border: 2px solid #2563eb;"
        "}"
        );

    this->showMaximized();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (backgroundLabel) {
        backgroundLabel->setGeometry(this->rect());
    }

    QString basePath = QCoreApplication::applicationDirPath();
    QString leftImagePath = basePath + "/../../images/image.png";

    QPixmap pix(leftImagePath);
    if (!pix.isNull()) {
        QPixmap scaledPix = pix.scaled(
            ui->labelImage->size(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation
            );
        ui->labelImage->setPixmap(scaledPix);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonLogin_clicked()
{
    QString username = ui->lineEditUsername->text().trimmed();
    QString password = ui->lineEditPassword->text();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter your username.");
        return;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter your password.");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT password_hash, salt, role FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    if (query.next()) {
        QString storedHash = query.value(0).toString();
        QString salt = query.value(1).toString();
        QString role = query.value(2).toString();

        QString hashedPassword = DatabaseManager::hashPassword(password, salt);

        if (hashedPassword == storedHash) {
            QMessageBox::information(this, "Login", "Login successful!");

            DashboardWindow *dashboard = new DashboardWindow(username, role);
            dashboard->show();
            this->close();
        } else {
            QMessageBox::warning(this, "Login", "Invalid username or password.");
            ui->lineEditPassword->clear();
            ui->lineEditPassword->setFocus();
        }
    } else {
        QMessageBox::warning(this, "Login", "Invalid username or password.");
        ui->lineEditPassword->clear();
        ui->lineEditPassword->setFocus();
    }
}

void MainWindow::on_pushButtonRegister_clicked()
{
    RegisterDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_pushButtonTogglePassword_clicked()
{
    if (ui->lineEditPassword->echoMode() == QLineEdit::Password) {
        ui->lineEditPassword->setEchoMode(QLineEdit::Normal);
        ui->pushButtonTogglePassword->setText("Hide");
    } else {
        ui->lineEditPassword->setEchoMode(QLineEdit::Password);
        ui->pushButtonTogglePassword->setText("Show");
    }

    ui->lineEditPassword->setFocus();
}
