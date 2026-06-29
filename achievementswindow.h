#ifndef ACHIEVEMENTSWINDOW_H
#define ACHIEVEMENTSWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QResizeEvent>

namespace Ui {
class AchievementsWindow;
}

class AchievementsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AchievementsWindow(const QString &username,
                                const QString &role,
                                QWidget *parent = nullptr);
    ~AchievementsWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButtonBack_clicked();

private:
    Ui::AchievementsWindow *ui;
    QLabel *backgroundLabel;
    QString currentUsername;
    QString currentRole;

    void loadAchievements();
};

#endif // ACHIEVEMENTSWINDOW_H
