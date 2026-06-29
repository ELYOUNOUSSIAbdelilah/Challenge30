#ifndef ADDQUESTIONDIALOG_H
#define ADDQUESTIONDIALOG_H

#include <QDialog>

namespace Ui {
class AddQuestionDialog;
}

class AddQuestionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddQuestionDialog(QWidget *parent = nullptr);
    ~AddQuestionDialog();

    void setEditMode(int questionId,
                     const QString &questionText,
                     const QString &optionA,
                     const QString &optionB,
                     const QString &optionC,
                     const QString &optionD,
                     const QString &correctAnswer,
                     const QString &difficulty,
                     const QString &category);

private slots:
    void on_pushButtonAdd_clicked();
    void on_pushButtonCancel_clicked();

private:
    Ui::AddQuestionDialog *ui;
    bool editMode = false;
    int currentQuestionId = -1;
};

#endif // ADDQUESTIONDIALOG_H