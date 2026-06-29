#ifndef LEADERBOARDWINDOW_H
#define LEADERBOARDWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QResizeEvent>

namespace Ui {
class LeaderboardWindow;
}

class LeaderboardWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LeaderboardWindow(const QString &username, const QString &role, QWidget *parent = nullptr);
    ~LeaderboardWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButtonBack_clicked();

private:
    Ui::LeaderboardWindow *ui;
    QString currentUsername;
    QString currentRole;
    QLabel *backgroundLabel;

    void loadScores();
};

#endif // LEADERBOARDWINDOW_H