#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QResizeEvent>

namespace Ui {
class DashboardWindow;
}

class DashboardWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DashboardWindow(const QString &username, const QString &role, QWidget *parent = nullptr);
    ~DashboardWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButtonLogout_clicked();
    void on_pushButtonStartChallenge_clicked();
    void on_pushButtonLeaderboard_clicked();
    void on_pushButtonStatistics_clicked();
    void on_pushButtonManageQuestions_clicked();
    void on_pushButtonHistory_clicked();
    void on_pushButtonManageUsers_clicked();
    void on_pushButtonAchievements_clicked();


private:
    Ui::DashboardWindow *ui;
    QString currentUsername;
    QString currentRole;
    QLabel *backgroundLabel;

    void loadDashboardStats();
};

#endif // DASHBOARDWINDOW_H