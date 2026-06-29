#include "registerdialog.h"
#include "ui_registerdialog.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include "databasemanager.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
    setWindowTitle("Challenge 30 - Register");

    ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    ui->lineEditConfirmPassword->setEchoMode(QLineEdit::Password);

    ui->lineEditUsername->setPlaceholderText("Enter username");
    ui->lineEditPassword->setPlaceholderText("Enter password");
    ui->lineEditConfirmPassword->setPlaceholderText("Confirm password");

    this->setStyleSheet(
        "QDialog {"
        "background-color: #f8fafc;"
        "border-radius: 12px;"
        "}"
        );

    ui->labelTitle->setText("Register New Account");
    ui->labelTitle->setStyleSheet(
        "color: #0f172a;"
        "font-size: 24px;"
        "font-weight: bold;"
        );

    QString labelStyle =
        "color: #334155;"
        "font-size: 15px;"
        "font-weight: 600;";

    ui->labelUsername->setStyleSheet(labelStyle);
    ui->labelPassword->setStyleSheet(labelStyle);
    ui->labelConfirmPassword->setStyleSheet(labelStyle);

    QString lineEditStyle =
        "QLineEdit {"
        "background-color: white;"
        "color: black;"
        "border: 2px solid #cbd5e1;"
        "border-radius: 10px;"
        "padding: 8px 12px;"
        "font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "border: 2px solid #2563eb;"
        "}";

    ui->lineEditUsername->setStyleSheet(lineEditStyle);
    ui->lineEditPassword->setStyleSheet(lineEditStyle);
    ui->lineEditConfirmPassword->setStyleSheet(lineEditStyle);

    ui->pushButtonCreateAccount->setStyleSheet(
        "QPushButton {"
        "background-color: #2563eb;"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 10px 18px;"
        "font-size: 15px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1d4ed8;"
        "}"
        );

    ui->pushButtonCancel->setStyleSheet(
        "QPushButton {"
        "background-color: #111827;"
        "color: white;"
        "border: 1px solid white;"
        "border-radius: 10px;"
        "padding: 10px 18px;"
        "font-size: 15px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1f2937;"
        "}"
        );
    this->setFixedSize(this->size());
    connect(ui->lineEditConfirmPassword, &QLineEdit::returnPressed,
            this, &RegisterDialog::on_pushButtonCreateAccount_clicked);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_pushButtonCancel_clicked()
{
    close();
}

void RegisterDialog::on_pushButtonCreateAccount_clicked()
{
    QString username = ui->lineEditUsername->text().trimmed();
    QString password = ui->lineEditPassword->text();
    QString confirmPassword = ui->lineEditConfirmPassword->text();

    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Register", "Please fill in all fields.");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Register", "Passwords do not match.");
        ui->lineEditConfirmPassword->clear();
        ui->lineEditConfirmPassword->setFocus();
        return;
    }

    if (password.length() < 4) {
        QMessageBox::warning(this, "Register", "Password must be at least 4 characters.");
        ui->lineEditPassword->setFocus();
        return;
    }

    QString salt = DatabaseManager::generateSalt();
    QString hashedPassword = DatabaseManager::hashPassword(password, salt);

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash, salt, role) VALUES (?, ?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);
    query.addBindValue(salt);
    query.addBindValue("user");


    if (!query.exec()) {
        QString errorText = query.lastError().text();

        if (errorText.contains("UNIQUE", Qt::CaseInsensitive)) {
            QMessageBox::warning(this, "Register", "Username already exists. Choose another one.");
            ui->lineEditUsername->setFocus();
        } else {
            QMessageBox::critical(this, "Database Error", errorText);
        }
        return;
    }

    QMessageBox::information(this, "Register", "Account created successfully.");
    accept();
}