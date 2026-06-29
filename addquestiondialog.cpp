#include "addquestiondialog.h"
#include "ui_addquestiondialog.h"
#include "databasemanager.h"

#include <QMessageBox>
#include <QDebug>

AddQuestionDialog::AddQuestionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddQuestionDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));

    setWindowTitle("Add Question");

    ui->comboBoxDifficulty->clear();
    ui->comboBoxDifficulty->addItems({"Easy", "Medium", "Hard"});

    ui->comboBoxCorrect->clear();
    ui->comboBoxCorrect->addItems({"A", "B", "C", "D"});

    ui->comboBoxCategory->clear();
    ui->comboBoxCategory->addItems({
        "General Knowledge",
        "Football",
        "Islamic",
        "Geography",
        "Science",
        "Literature",
        "Math",
        "History",
        "Programming"
    });
}

AddQuestionDialog::~AddQuestionDialog()
{
    delete ui;
}

void AddQuestionDialog::setEditMode(int questionId,
                                    const QString &questionText,
                                    const QString &optionA,
                                    const QString &optionB,
                                    const QString &optionC,
                                    const QString &optionD,
                                    const QString &correctAnswer,
                                    const QString &difficulty,
                                    const QString &category)
{
    editMode = true;
    currentQuestionId = questionId;

    ui->lineEditQuestion->setText(questionText);
    ui->lineEditOptionA->setText(optionA);
    ui->lineEditOptionB->setText(optionB);
    ui->lineEditOptionC->setText(optionC);
    ui->lineEditOptionD->setText(optionD);

    ui->comboBoxCorrect->setCurrentText(correctAnswer);
    ui->comboBoxDifficulty->setCurrentText(difficulty);
    ui->comboBoxCategory->setCurrentText(category);

    ui->pushButtonAdd->setText("Save Changes");
    setWindowTitle("Edit Question");
}

void AddQuestionDialog::on_pushButtonAdd_clicked()
{
    QString question = ui->lineEditQuestion->text().trimmed();
    QString optionA = ui->lineEditOptionA->text().trimmed();
    QString optionB = ui->lineEditOptionB->text().trimmed();
    QString optionC = ui->lineEditOptionC->text().trimmed();
    QString optionD = ui->lineEditOptionD->text().trimmed();

    QString correct = ui->comboBoxCorrect->currentText();
    QString difficulty = ui->comboBoxDifficulty->currentText();
    QString category = ui->comboBoxCategory->currentText();

    if (question.isEmpty() || optionA.isEmpty() || optionB.isEmpty() ||
        optionC.isEmpty() || optionD.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please fill all fields.");
        return;
    }

    bool success = false;

    if (editMode) {
        success = DatabaseManager::updateQuestion(currentQuestionId,
                                                  question,
                                                  optionA,
                                                  optionB,
                                                  optionC,
                                                  optionD,
                                                  correct,
                                                  difficulty,
                                                  category);
    } else {
        success = DatabaseManager::addQuestion(question,
                                               optionA,
                                               optionB,
                                               optionC,
                                               optionD,
                                               correct,
                                               difficulty,
                                               category);
    }

    if (success) {
        QMessageBox::information(this, "Success",
                                 editMode ? "Question updated successfully!"
                                          : "Question added successfully!");
        accept();
    } else {
        QMessageBox::warning(this, "Error",
                             editMode ? "Failed to update question."
                                      : "Failed to add question.");
    }
}

void AddQuestionDialog::on_pushButtonCancel_clicked()
{
    close();
}