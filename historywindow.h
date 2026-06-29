#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QResizeEvent>

namespace Ui {
class HistoryWindow;
}

class HistoryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HistoryWindow(const QString &username, const QString &role, QWidget *parent = nullptr);
    ~HistoryWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButtonBack_clicked();

private:
    Ui::HistoryWindow *ui;
    QString currentUsername;
    QString currentRole;
    QLabel *backgroundLabel;

    void loadHistory();
};

#endif // HISTORYWINDOW_H