#ifndef USERMANAGERWINDOW_H
#define USERMANAGERWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QResizeEvent>

namespace Ui {
class UserManagerWindow;
}

class UserManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserManagerWindow(const QString &username,
                               const QString &role,
                               QWidget *parent = nullptr);
    ~UserManagerWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButtonRefresh_clicked();
    void on_pushButtonPromote_clicked();
    void on_pushButtonDemote_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonBack_clicked();

private:
    Ui::UserManagerWindow *ui;
    QLabel *backgroundLabel;
    QString currentUsername;
    QString currentRole;

    void loadUsers();
    bool isValidUserRow(int row) const;
};

#endif // USERMANAGERWINDOW_H
