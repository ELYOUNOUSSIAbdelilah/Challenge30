#include "startchallengedialog.h"
#include "ui_startchallengedialog.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

StartChallengeDialog::StartChallengeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StartChallengeDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
    setWindowTitle("Challenge 30 - Start Challenge");

    ui->labelTitle->setText("Start New Challenge");
    ui->labelTitle->setStyleSheet(
        "color: #0f172a;"
        "font-size: 26px;"
        "font-weight: bold;"
        );

    QString labelStyle =
        "color: #334155;"
        "font-size: 15px;"
        "font-weight: 600;";

    ui->labelDifficulty->setStyleSheet(labelStyle);
    ui->labelCategory->setStyleSheet(labelStyle);

    QString comboStyle =
        "QComboBox {"
        "background-color: white;"
        "color: black;"
        "border: 2px solid #cbd5e1;"
        "border-radius: 10px;"
        "padding: 8px 12px;"
        "font-size: 14px;"
        "}"
        "QComboBox:focus {"
        "border: 2px solid #2563eb;"
        "}";

    ui->comboBoxDifficulty->setStyleSheet(comboStyle);
    ui->comboBoxCategory->setStyleSheet(comboStyle);

    ui->pushButtonStart->setStyleSheet(
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

    this->setStyleSheet(
        "QDialog {"
        "background-color: #f8fafc;"
        "}"
        );

    connect(ui->comboBoxDifficulty, &QComboBox::currentTextChanged,
            this, &StartChallengeDialog::on_comboBoxDifficulty_currentTextChanged);

    loadDifficulties();
    loadCategories(ui->comboBoxDifficulty->currentText());
}

StartChallengeDialog::~StartChallengeDialog()
{
    delete ui;
}

QString StartChallengeDialog::selectedDifficulty() const
{
    return difficulty;
}

QString StartChallengeDialog::selectedCategory() const
{
    return category;
}

void StartChallengeDialog::loadDifficulties()
{
    ui->comboBoxDifficulty->clear();

    QSqlQuery query("SELECT DISTINCT difficulty FROM questions ORDER BY difficulty");
    while (query.next()) {
        ui->comboBoxDifficulty->addItem(query.value(0).toString());
    }

    if (ui->comboBoxDifficulty->count() == 0) {
        ui->comboBoxDifficulty->addItems({"Easy", "Medium", "Hard"});
    }
}

void StartChallengeDialog::loadCategories(const QString &difficulty)
{
    ui->comboBoxCategory->clear();

    QSqlQuery query;
    query.prepare("SELECT DISTINCT category FROM questions WHERE difficulty = ? ORDER BY category");
    query.addBindValue(difficulty);

    if (!query.exec()) {
        qDebug() << "Failed to load categories:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        ui->comboBoxCategory->addItem(query.value(0).toString());
    }
}

int StartChallengeDialog::countAvailableQuestions(const QString &difficulty, const QString &category)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM questions WHERE difficulty = ? AND category = ?");
    query.addBindValue(difficulty);
    query.addBindValue(category);

    if (!query.exec()) {
        qDebug() << "Failed to count available questions:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

void StartChallengeDialog::on_comboBoxDifficulty_currentTextChanged(const QString &difficulty)
{
    loadCategories(difficulty);
}

void StartChallengeDialog::on_pushButtonStart_clicked()
{
    difficulty = ui->comboBoxDifficulty->currentText();
    category = ui->comboBoxCategory->currentText();

    if (difficulty.isEmpty() || category.isEmpty()) {
        QMessageBox::warning(this, "Start Challenge", "Please select both difficulty and category.");
        return;
    }

    int availableQuestions = countAvailableQuestions(difficulty, category);
    if (availableQuestions == 0) {
        QMessageBox::warning(this, "Start Challenge", "No questions available for this selection.");
        return;
    }

    accept();
}

void StartChallengeDialog::on_pushButtonCancel_clicked()
{
    reject();
}