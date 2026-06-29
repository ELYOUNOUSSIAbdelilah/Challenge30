#ifndef QUESTIONMANAGERWINDOW_H
#define QUESTIONMANAGERWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QResizeEvent>

namespace Ui {
class QuestionManagerWindow;
}

class QuestionManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QuestionManagerWindow(const QString &username,
                                   const QString &role,
                                   QWidget *parent = nullptr);
    ~QuestionManagerWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButtonAddQuestion_clicked();
    void on_pushButtonEditQuestion_clicked();
    void on_pushButtonDeleteQuestion_clicked();
    void on_pushButtonBack_clicked();

    void loadCategories();
    void loadQuestions(const QString &searchText = "", const QString &category = "All Categories");
    void on_lineEditSearch_textChanged(const QString &text);
    void on_comboBoxCategoryFilter_currentTextChanged(const QString &text);

private:
    Ui::QuestionManagerWindow *ui;
    QLabel *backgroundLabel;
    QString currentUsername;
    QString currentRole;

    bool isValidQuestionRow(int row) const;
};

#endif // QUESTIONMANAGERWINDOW_H