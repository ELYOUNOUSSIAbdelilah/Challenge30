#include "challengewindow.h"
#include "ui_challengewindow.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QRandomGenerator>
#include <QTimer>
#include <QCoreApplication>
#include <QPropertyAnimation>
#include <QPixmap>
#include <algorithm>

#include "dashboardwindow.h"
#include "databasemanager.h"
#include "resultdialog.h"
#include "soundmanager.h"
ChallengeWindow::ChallengeWindow(const QString &username,
                                 const QString &role,
                                 const QString &difficulty,
                                 const QString &category,
                                 QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChallengeWindow)
    , currentUsername(username)
    , currentRole(role)
    , selectedDifficulty(difficulty)
    , selectedCategory(category)
    , backgroundLabel(nullptr)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
    setWindowTitle("Challenge 30 - Quiz");

    backgroundLabel = new QLabel(this);
    QString imagePath = QCoreApplication::applicationDirPath() + "/../../images/background.png";
    QPixmap bg(imagePath);
    backgroundLabel->setPixmap(bg);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(this->rect());
    backgroundLabel->lower();

    ui->centralwidget->setStyleSheet("background: transparent;");

    ui->frameChallenge->setStyleSheet(
        "#frameChallenge {"
        "background-color: rgba(0, 0, 0, 95);"
        "border-radius: 18px;"
        "}"
        );

    ui->labelTitle->setStyleSheet(
        "color: white;"
        "font-size: 30px;"
        "font-weight: bold;"
        );

    QString infoStyle =
        "color: white;"
        "font-size: 18px;"
        "font-weight: 600;";

    ui->labelProgress->setStyleSheet(infoStyle);
    ui->labelCorrect->setStyleSheet(infoStyle);
    ui->labelWrong->setStyleSheet(infoStyle);
    ui->labelCategory->setStyleSheet(infoStyle);
    ui->labelDifficulty->setStyleSheet(infoStyle);
    ui->labelTimer->setStyleSheet(infoStyle);
    ui->labelScore->setStyleSheet(infoStyle);

    ui->labelQuestion->setStyleSheet(
        "background-color: rgba(255, 255, 255, 230);"
        "color: #0f172a;"
        "border-radius: 14px;"
        "padding: 20px;"
        "font-size: 24px;"
        "font-weight: bold;"
        );
    ui->labelQuestion->setWordWrap(true);
    ui->labelQuestion->setAlignment(Qt::AlignCenter);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ChallengeWindow::updateTimer);

    loadQuestionIds();
    loadCurrentQuestion();

    this->showMaximized();
}

ChallengeWindow::~ChallengeWindow()
{
    delete ui;
}

void ChallengeWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (backgroundLabel) {
        backgroundLabel->setGeometry(this->rect());
    }
}

void ChallengeWindow::loadQuestionIds()
{
    questionIds.clear();

    QSqlQuery query;
    query.prepare("SELECT id FROM questions WHERE difficulty = ? AND category = ?");
    query.addBindValue(selectedDifficulty);
    query.addBindValue(selectedCategory);

    if (!query.exec()) {
        qDebug() << "Error loading question IDs:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        questionIds.append(query.value(0).toInt());
    }

    std::shuffle(questionIds.begin(), questionIds.end(), *QRandomGenerator::global());
}

void ChallengeWindow::loadCurrentQuestion()
{
    if (questionIds.isEmpty()) {
        QMessageBox::warning(this,
                             "No Questions",
                             "No questions available for difficulty: " + selectedDifficulty +
                                 "\nand category: " + selectedCategory);

        DashboardWindow *dashboard = new DashboardWindow(currentUsername, currentRole);
        dashboard->show();
        this->close();
        return;
    }

    if (currentQuestionIndex >= questionIds.size()) {
        timer->stop();

        DatabaseManager::saveScore(currentUsername,
                                   score,
                                   selectedCategory,
                                   selectedDifficulty);

        DatabaseManager::ensurePlayerProgressExists(currentUsername);
        DatabaseManager::incrementGamesPlayed(currentUsername);
        DatabaseManager::addXp(currentUsername, score);

        QStringList unlockedNow;

        int gamesPlayed = DatabaseManager::getGamesPlayed(currentUsername);

        if (!DatabaseManager::hasAchievement(currentUsername, "first_game")) {
            if (DatabaseManager::unlockAchievement(currentUsername, "first_game")) {
                unlockedNow << "First Game";
            }
        }

        if (score > 0 && !DatabaseManager::hasAchievement(currentUsername, "first_win")) {
            if (DatabaseManager::unlockAchievement(currentUsername, "first_win")) {
                unlockedNow << "First Win";
            }
        }

        if (wrongAnswers == 0 && !DatabaseManager::hasAchievement(currentUsername, "perfect_score")) {
            if (DatabaseManager::unlockAchievement(currentUsername, "perfect_score")) {
                unlockedNow << "Perfect Score";
            }
        }

        if (gamesPlayed >= 5 && !DatabaseManager::hasAchievement(currentUsername, "five_games")) {
            if (DatabaseManager::unlockAchievement(currentUsername, "five_games")) {
                unlockedNow << "Five Games Played";
            }
        }

        if (!unlockedNow.isEmpty()) {
            QMessageBox::information(
                this,
                "Achievements Unlocked",
                "You unlocked:\n- " + unlockedNow.join("\n- ")
                );
        }

        ResultDialog dialog(currentUsername,
                            selectedDifficulty,
                            selectedCategory,
                            score,
                            correctAnswers,
                            wrongAnswers,
                            this);

        dialog.exec();

        if (dialog.playAgainRequested()) {
            ChallengeWindow *challenge = new ChallengeWindow(currentUsername,
                                                             currentRole,
                                                             selectedDifficulty,
                                                             selectedCategory);
            challenge->show();
        } else {
            DashboardWindow *dashboard = new DashboardWindow(currentUsername, currentRole);
            dashboard->show();
        }

        this->close();
        return;
    }

    int questionId = questionIds[currentQuestionIndex];

    QSqlQuery query;
    query.prepare("SELECT question_text, option_a, option_b, option_c, option_d, correct_answer, difficulty, category "
                  "FROM questions WHERE id = ?");
    query.addBindValue(questionId);

    if (!query.exec()) {
        qDebug() << "Error loading question:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        ui->labelProgress->setText("Question " + QString::number(currentQuestionIndex + 1) +
                                   " / " + QString::number(questionIds.size()));

        ui->labelCorrect->setText("Correct: " + QString::number(correctAnswers));
        ui->labelWrong->setText("Wrong: " + QString::number(wrongAnswers));
        ui->labelScore->setText("Score: " + QString::number(score));

        ui->labelQuestion->setText(query.value(0).toString());
        ui->pushButtonOptionA->setText(query.value(1).toString());
        ui->pushButtonOptionB->setText(query.value(2).toString());
        ui->pushButtonOptionC->setText(query.value(3).toString());
        ui->pushButtonOptionD->setText(query.value(4).toString());

        correctAnswer = query.value(5).toString();
        currentDifficulty = query.value(6).toString();

        ui->labelDifficulty->setText("Difficulty: " + currentDifficulty);
        ui->labelCategory->setText("Category: " + query.value(7).toString());

        resetAnswerButtonStyles();
        setAnswerButtonsEnabled(true);

        timeLeft = 10;
        ui->labelTimer->setText("Time: " + QString::number(timeLeft));
        ui->labelTimer->setStyleSheet("color: white; font-size: 18px; font-weight: 600;");
        timer->start(1000);
    }
}

void ChallengeWindow::checkAnswer(const QString &selectedAnswer)
{
    timer->stop();
    setAnswerButtonsEnabled(false);

    showAnswerFeedback(selectedAnswer);

    if (selectedAnswer == correctAnswer) {
        SoundManager::instance().playCorrect();

        int points = 10;

        if (currentDifficulty == "Medium")
            points = 20;
        else if (currentDifficulty == "Hard")
            points = 30;

        score += points;
        correctAnswers++;
    } else {
        SoundManager::instance().playWrong();
        wrongAnswers++;
    }

    ui->labelScore->setText("Score: " + QString::number(score));
    ui->labelCorrect->setText("Correct: " + QString::number(correctAnswers));
    ui->labelWrong->setText("Wrong: " + QString::number(wrongAnswers));

    QTimer::singleShot(1000, this, [this]() {
        currentQuestionIndex++;
        loadCurrentQuestion();
    });
}

void ChallengeWindow::setAnswerButtonsEnabled(bool enabled)
{
    ui->pushButtonOptionA->setEnabled(enabled);
    ui->pushButtonOptionB->setEnabled(enabled);
    ui->pushButtonOptionC->setEnabled(enabled);
    ui->pushButtonOptionD->setEnabled(enabled);
}

void ChallengeWindow::on_pushButtonOptionA_clicked()
{
    checkAnswer("A");
}

QString ChallengeWindow::getCorrectAnswerText() const
{
    if (correctAnswer == "A")
        return ui->pushButtonOptionA->text();
    else if (correctAnswer == "B")
        return ui->pushButtonOptionB->text();
    else if (correctAnswer == "C")
        return ui->pushButtonOptionC->text();
    else if (correctAnswer == "D")
        return ui->pushButtonOptionD->text();

    return "Unknown";
}

void ChallengeWindow::on_pushButtonOptionB_clicked()
{
    checkAnswer("B");
}

void ChallengeWindow::on_pushButtonOptionC_clicked()
{
    checkAnswer("C");
}

void ChallengeWindow::on_pushButtonOptionD_clicked()
{
    checkAnswer("D");
}

void ChallengeWindow::updateTimer()
{
    timeLeft--;
    ui->labelTimer->setText("Time: " + QString::number(timeLeft));

    if (timeLeft == 3 || timeLeft == 2 || timeLeft == 1) {
        SoundManager::instance().playWarning();
        animateTimerPulse();
        ui->labelTimer->setStyleSheet("color: #dc2626; font-size: 18px; font-weight: bold;");
    } else if (timeLeft <= 5) {
        ui->labelTimer->setStyleSheet("color: #f59e0b; font-size: 18px; font-weight: bold;");
    } else {
        ui->labelTimer->setStyleSheet("color: white; font-size: 18px; font-weight: 600;");
    }

    if (timeLeft <= 0) {
        timer->stop();
        setAnswerButtonsEnabled(false);

        wrongAnswers++;
        ui->labelWrong->setText("Wrong: " + QString::number(wrongAnswers));

        showAnswerFeedback("");

        QTimer::singleShot(1000, this, [this]() {
            currentQuestionIndex++;
            loadCurrentQuestion();
        });
    }
}

void ChallengeWindow::resetAnswerButtonStyles()
{
    QString defaultStyle =
        "QPushButton {"
        "background-color: #2563eb;"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 12px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1d4ed8;"
        "}"
        "QPushButton:disabled {"
        "background-color: #9ca3af;"
        "color: #f3f4f6;"
        "}";

    ui->pushButtonOptionA->setStyleSheet(defaultStyle);
    ui->pushButtonOptionB->setStyleSheet(defaultStyle);
    ui->pushButtonOptionC->setStyleSheet(defaultStyle);
    ui->pushButtonOptionD->setStyleSheet(defaultStyle);
}

void ChallengeWindow::showAnswerFeedback(const QString &selectedAnswer)
{
    resetAnswerButtonStyles();

    QPushButton *selectedButton = nullptr;
    QPushButton *correctButton = nullptr;

    if (selectedAnswer == "A") selectedButton = ui->pushButtonOptionA;
    else if (selectedAnswer == "B") selectedButton = ui->pushButtonOptionB;
    else if (selectedAnswer == "C") selectedButton = ui->pushButtonOptionC;
    else if (selectedAnswer == "D") selectedButton = ui->pushButtonOptionD;

    if (correctAnswer == "A") correctButton = ui->pushButtonOptionA;
    else if (correctAnswer == "B") correctButton = ui->pushButtonOptionB;
    else if (correctAnswer == "C") correctButton = ui->pushButtonOptionC;
    else if (correctAnswer == "D") correctButton = ui->pushButtonOptionD;

    QString greenStyle =
        "QPushButton {"
        "background-color: #16a34a;"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 12px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}";

    QString redStyle =
        "QPushButton {"
        "background-color: #dc2626;"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 12px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}";

    if (correctButton) {
        correctButton->setStyleSheet(greenStyle);
    }

    if (selectedButton && selectedAnswer != correctAnswer) {
        selectedButton->setStyleSheet(redStyle);
    }
}
void ChallengeWindow::animateTimerPulse()
{
    if (ui->labelTimer->property("animating").toBool())
        return;

    ui->labelTimer->setProperty("animating", true);

    QRect original = ui->labelTimer->geometry();
    QRect bigger = original.adjusted(-5, -5, 5, 5);

    QPropertyAnimation *grow = new QPropertyAnimation(ui->labelTimer, "geometry");
    grow->setDuration(120);
    grow->setStartValue(original);
    grow->setEndValue(bigger);

    QPropertyAnimation *shrink = new QPropertyAnimation(ui->labelTimer, "geometry");
    shrink->setDuration(120);
    shrink->setStartValue(bigger);
    shrink->setEndValue(original);

    connect(grow, &QPropertyAnimation::finished, shrink, [shrink]() {
        shrink->start(QAbstractAnimation::DeleteWhenStopped);
    });

    connect(shrink, &QPropertyAnimation::finished, this, [this]() {
        ui->labelTimer->setProperty("animating", false);
    });

    grow->start(QAbstractAnimation::DeleteWhenStopped);
}