#include "resultdialog.h"
#include "ui_resultdialog.h"
#include "soundmanager.h"

ResultDialog::ResultDialog(const QString &username,
                           const QString &difficulty,
                           const QString &category,
                           int score,
                           int correctAnswers,
                           int wrongAnswers,
                           QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResultDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
    ui->frameResult->setStyleSheet(
        "#frameResult {"
        "background-color: white;"
        "border-radius: 16px;"
        "padding: 18px;"
        "}"
        );
    ui->labelTitle->setStyleSheet(
        "font-size: 22px;"
        "font-weight: bold;"
        "color: #0f172a;"
        );
    this->setStyleSheet(
        "QDialog {"
        "background-color: #f8fafc;"
        "}"
        );

    ui->labelUsername->setStyleSheet(
        "font-size: 16px;"
        "color: #334155;"
        );

    ui->labelDifficulty->setStyleSheet(
        "font-size: 16px;"
        "color: #334155;"
        );

    ui->labelScore->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #2563eb;"
        );

    ui->labelCorrect->setStyleSheet(
        "font-size: 16px;"
        "font-weight: 600;"
        "color: #16a34a;"
        );

    ui->labelWrong->setStyleSheet(
        "font-size: 16px;"
        "font-weight: 600;"
        "color: #dc2626;"
        );

    ui->pushButtonPlayAgain->setStyleSheet(
        "QPushButton {"
        "background-color: #2563eb;"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 10px 18px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1d4ed8;"
        "}"
        );

    ui->pushButtonDashboard->setStyleSheet(
        "QPushButton {"
        "background-color: #111827;"
        "color: white;"
        "border: 1px solid white;"
        "border-radius: 10px;"
        "padding: 10px 18px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1f2937;"
        "}"
        );

    ui->labelUsername->setText("Username: " + username);
    ui->labelDifficulty->setText("Difficulty: " + difficulty);
    ui->labelCategory->setText("Category: " + category);
    ui->labelScore->setText("Final Score: " + QString::number(score));
    ui->labelCorrect->setText("Correct Answers: " + QString::number(correctAnswers));
    ui->labelWrong->setText("Wrong Answers: " + QString::number(wrongAnswers));
    SoundManager::instance().playFinish();
    if (score >= 100) {
        setWindowTitle("Excellent Result");
    } else if (score >= 50) {
        setWindowTitle("Good Result");
    } else {
        setWindowTitle("Quiz Result");
    }
}

ResultDialog::~ResultDialog()
{
    delete ui;
}

bool ResultDialog::playAgainRequested() const
{
    return playAgain;
}

void ResultDialog::on_pushButtonDashboard_clicked()
{
    playAgain = false;
    accept();
}

void ResultDialog::on_pushButtonPlayAgain_clicked()
{
    playAgain = true;
    accept();
}