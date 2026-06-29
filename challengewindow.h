#ifndef CHALLENGEWINDOW_H
#define CHALLENGEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QLabel>
#include <QResizeEvent>

namespace Ui {
class ChallengeWindow;
}

class ChallengeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChallengeWindow(const QString &username,
                             const QString &role,
                             const QString &difficulty,
                             const QString &category,
                             QWidget *parent = nullptr);
    ~ChallengeWindow();
protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButtonOptionA_clicked();
    void on_pushButtonOptionB_clicked();
    void on_pushButtonOptionC_clicked();
    void on_pushButtonOptionD_clicked();
    void updateTimer();

private:
    Ui::ChallengeWindow *ui;

    void loadQuestionIds();
    QString getCorrectAnswerText() const;
    void resetAnswerButtonStyles();
     void animateTimerPulse();
    void showAnswerFeedback(const QString &selectedAnswer);
    void setAnswerButtonsEnabled(bool enabled);
    void loadCurrentQuestion();
    void checkAnswer(const QString &selectedAnswer);

    QVector<int> questionIds;
    int currentQuestionIndex = 0;
    int correctAnswers = 0;
    int wrongAnswers = 0;
    QString correctAnswer;
    QTimer *timer;
    int timeLeft = 10;
    int score = 0;
    QLabel *backgroundLabel;

    QString currentUsername;
    QString currentRole;
    QString currentDifficulty;
    QString selectedDifficulty;
    QString selectedCategory;
};

#endif // CHALLENGEWINDOW_H