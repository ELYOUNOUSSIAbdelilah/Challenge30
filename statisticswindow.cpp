#include "statisticswindow.h"
#include "ui_statisticswindow.h"
#include "dashboardwindow.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QMap>
#include <QList>

StatisticsWindow::StatisticsWindow(const QString &username, const QString &role, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StatisticsWindow)
    , currentUsername(username)
    , currentRole(role)
    , backgroundLabel(nullptr)
    , scoreChartView(nullptr)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/icon.ico"));
    setWindowTitle("Challenge 30 - Statistics");

    // Background
    backgroundLabel = new QLabel(this);
    QString imagePath = QCoreApplication::applicationDirPath() + "/../../images/background.png";
    QPixmap bg(imagePath);
    backgroundLabel->setPixmap(bg);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(this->rect());
    backgroundLabel->lower();

    ui->centralwidget->setStyleSheet("background: transparent;");

    // Main frame
    ui->frameStatistics->setStyleSheet(
        "#frameStatistics {"
        "background-color: rgba(0, 0, 0, 95);"
        "border-radius: 18px;"
        "}"
        );
    ui->widgetScoreChart->setStyleSheet(
        "background-color: rgba(255, 255, 255, 235);"
        "border-radius: 12px;"
        );
    // Title
    ui->labelTitle->setText("Statistics");
    ui->labelTitle->setStyleSheet(
        "color: white;"
        "font-size: 28px;"
        "font-weight: bold;"
        );

    // Main labels
    QString normalLabelStyle =
        "color: white;"
        "font-size: 18px;"
        "font-weight: 600;";

    ui->labelUsername->setStyleSheet(normalLabelStyle);
    ui->labelGamesPlayed->setStyleSheet(normalLabelStyle);
    ui->labelTotalQuestions->setStyleSheet(normalLabelStyle);
    ui->labelBestPlayer->setStyleSheet(normalLabelStyle);

    ui->labelBestScore->setStyleSheet(
        "color: #22c55e;"
        "font-size: 18px;"
        "font-weight: bold;"
        );

    ui->labelAverageScore->setStyleSheet(
        "color: #38bdf8;"
        "font-size: 18px;"
        "font-weight: 600;"
        );

    // Back button
    ui->pushButtonBack->setStyleSheet(
        "QPushButton {"
        "background-color: #111827;"
        "color: white;"
        "border: 1px solid white;"
        "border-radius: 10px;"
        "padding: 10px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1f2937;"
        "}"
        );

    // Table style
    ui->tableWidgetCategoryStats->setColumnCount(3);
    ui->tableWidgetCategoryStats->setHorizontalHeaderLabels(
        QStringList() << "Category" << "Questions" << "Percentage"
        );

    ui->tableWidgetCategoryStats->setStyleSheet(
        "QTableWidget {"
        "background-color: rgba(255, 255, 255, 230);"
        "color: #0f172a;"
        "border: none;"
        "border-radius: 10px;"
        "gridline-color: #cbd5e1;"
        "font-size: 14px;"
        "}"
        "QHeaderView::section {"
        "background-color: #2563eb;"
        "color: white;"
        "padding: 8px;"
        "border: none;"
        "font-weight: bold;"
        "}"
        );

    ui->tableWidgetCategoryStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetCategoryStats->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidgetCategoryStats->verticalHeader()->setVisible(false);
    ui->tableWidgetCategoryStats->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetCategoryStats->setSelectionMode(QAbstractItemView::NoSelection);

    loadStatistics();
    loadCategoryStatistics();
    loadScoreChart();

    // Fade-in animation
    this->setWindowOpacity(0.0);

    QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setDuration(400);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    this->showMaximized();
}

StatisticsWindow::~StatisticsWindow()
{
    delete ui;
}

void StatisticsWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (backgroundLabel) {
        backgroundLabel->setGeometry(this->rect());
    }
}

void StatisticsWindow::loadStatistics()
{
    ui->labelUsername->setText("Username: <b>" + currentUsername + "</b>");

    // Personal stats
    QSqlQuery query;
    query.prepare("SELECT COUNT(*), MAX(score), AVG(score) FROM scores WHERE username = ?");
    query.addBindValue(currentUsername);

    if (!query.exec()) {
        qDebug() << "Failed to load personal statistics:" << query.lastError().text();
        return;
    }

    int gamesPlayed = 0;
    int bestScore = 0;
    double averageScore = 0.0;

    if (query.next()) {
        gamesPlayed = query.value(0).toInt();
        bestScore = query.value(1).toInt();
        averageScore = query.value(2).toDouble();
    }

    ui->labelGamesPlayed->setText("Games Played: <b>" + QString::number(gamesPlayed) + "</b>");
    ui->labelBestScore->setText("Best Score: <b>" + QString::number(bestScore) + "</b>");
    ui->labelAverageScore->setText("Average Score: <b>" + QString::number(averageScore, 'f', 2) + "</b>");

    // Total questions
    QSqlQuery questionQuery("SELECT COUNT(*) FROM questions");
    int totalQuestions = 0;
    if (questionQuery.next()) {
        totalQuestions = questionQuery.value(0).toInt();
    }
    ui->labelTotalQuestions->setText("Total Questions: <b>" + QString::number(totalQuestions) + "</b>");

    // Best player overall
    QSqlQuery bestPlayerQuery(
        "SELECT username, MAX(score) "
        "FROM scores "
        "GROUP BY username "
        "ORDER BY MAX(score) DESC "
        "LIMIT 1"
        );

    QString bestPlayer = "N/A";
    int bestPlayerScore = 0;

    if (bestPlayerQuery.next()) {
        bestPlayer = bestPlayerQuery.value(0).toString();
        bestPlayerScore = bestPlayerQuery.value(1).toInt();
    }

    ui->labelBestPlayer->setText(
        "Best Player: <b>" + bestPlayer + "</b> (Score: " + QString::number(bestPlayerScore) + ")"
        );
}

void StatisticsWindow::loadCategoryStatistics()
{
    ui->tableWidgetCategoryStats->setRowCount(0);

    QSqlQuery totalQuery("SELECT COUNT(*) FROM questions");
    int totalQuestions = 0;
    if (totalQuery.next()) {
        totalQuestions = totalQuery.value(0).toInt();
    }

    QSqlQuery query(
        "SELECT category, COUNT(*) "
        "FROM questions "
        "GROUP BY category "
        "ORDER BY COUNT(*) DESC"
        );

    int row = 0;
    while (query.next()) {
        QString category = query.value(0).toString();
        int count = query.value(1).toInt();
        double percentage = (totalQuestions > 0) ? (count * 100.0 / totalQuestions) : 0.0;

        ui->tableWidgetCategoryStats->insertRow(row);

        QTableWidgetItem *categoryItem = new QTableWidgetItem(category);
        QTableWidgetItem *countItem = new QTableWidgetItem(QString::number(count));
        QTableWidgetItem *percentageItem = new QTableWidgetItem(QString::number(percentage, 'f', 1) + "%");

        categoryItem->setTextAlignment(Qt::AlignCenter);
        countItem->setTextAlignment(Qt::AlignCenter);
        percentageItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidgetCategoryStats->setItem(row, 0, categoryItem);
        ui->tableWidgetCategoryStats->setItem(row, 1, countItem);
        ui->tableWidgetCategoryStats->setItem(row, 2, percentageItem);

        row++;
    }

    if (row == 0) {
        ui->tableWidgetCategoryStats->setRowCount(1);
        ui->tableWidgetCategoryStats->setSpan(0, 0, 1, 3);

        QTableWidgetItem *item = new QTableWidgetItem("No category data available");
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetCategoryStats->setItem(0, 0, item);
    }
}

void StatisticsWindow::on_pushButtonBack_clicked()
{
    DashboardWindow *dashboard = new DashboardWindow(currentUsername, currentRole);
    dashboard->show();
    this->close();
}
void StatisticsWindow::loadScoreChart()
{
    if (scoreChartView) {
        delete scoreChartView;
        scoreChartView = nullptr;
    }

    QMap<QString, QList<int>> categoryScores;

    QSqlQuery query;
    query.prepare("SELECT category, score FROM scores WHERE username = ?");
    query.addBindValue(currentUsername);

    if (!query.exec()) {
        qDebug() << "Failed to load score chart data:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString category = query.value(0).toString();
        int score = query.value(1).toInt();
        categoryScores[category].append(score);
    }

    QBarSet *set = new QBarSet("Average Score");
    QStringList categories;

    for (auto it = categoryScores.begin(); it != categoryScores.end(); ++it) {
        const QString &category = it.key();
        const QList<int> &scores = it.value();

        if (scores.isEmpty())
            continue;

        int total = 0;
        for (int s : scores)
            total += s;

        double average = static_cast<double>(total) / scores.size();

        *set << average;
        categories << category;
    }

    if (categories.isEmpty()) {
        delete set;
        return;
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(0.7);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Scores by Category");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundVisible(false);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("Score");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

     chart->legend()->setVisible(false);

    scoreChartView = new QChartView(chart);
    scoreChartView->setRenderHint(QPainter::Antialiasing);
    scoreChartView->setStyleSheet("background: transparent; border: none;");

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->widgetScoreChart->layout());
    if (!layout) {
        layout = new QVBoxLayout(ui->widgetScoreChart);
        layout->setContentsMargins(10, 10, 10, 10);
    }

    layout->addWidget(scoreChartView);
}
