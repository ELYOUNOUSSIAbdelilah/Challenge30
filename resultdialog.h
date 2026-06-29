#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>

namespace Ui {
class ResultDialog;
}

class ResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultDialog(const QString &username,
                          const QString &difficulty,
                          const QString &category,
                          int score,
                          int correctAnswers,
                          int wrongAnswers,
                          QWidget *parent = nullptr);
    ~ResultDialog();

    bool playAgainRequested() const;

private slots:
    void on_pushButtonDashboard_clicked();
    void on_pushButtonPlayAgain_clicked();

private:
    Ui::ResultDialog *ui;
    bool playAgain = false;
};

#endif // RESULTDIALOG_H