#ifndef STARTCHALLENGEDIALOG_H
#define STARTCHALLENGEDIALOG_H

#include <QDialog>

namespace Ui {
class StartChallengeDialog;
}

class StartChallengeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartChallengeDialog(QWidget *parent = nullptr);
    ~StartChallengeDialog();

    QString selectedDifficulty() const;
    QString selectedCategory() const;

private slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonCancel_clicked();
    void on_comboBoxDifficulty_currentTextChanged(const QString &difficulty);

private:
    Ui::StartChallengeDialog *ui;

    QString difficulty;
    QString category;

    void loadDifficulties();
    void loadCategories(const QString &difficulty);
    int countAvailableQuestions(const QString &difficulty, const QString &category);
};

#endif // STARTCHALLENGEDIALOG_H