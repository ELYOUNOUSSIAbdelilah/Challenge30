#ifndef STATISTICSWINDOW_H
#define STATISTICSWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QResizeEvent>

namespace Ui {
class StatisticsWindow;
}

class StatisticsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StatisticsWindow(const QString &username, const QString &role, QWidget *parent = nullptr);
    ~StatisticsWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButtonBack_clicked();

private:
    Ui::StatisticsWindow *ui;
    QString currentUsername;
    QString currentRole;
    QLabel *backgroundLabel;
    QChartView *scoreChartView;

    void loadStatistics();
    void loadCategoryStatistics();
    void loadScoreChart();
};
#endif // STATISTICSWINDOW_H