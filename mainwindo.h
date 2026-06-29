#ifndef MAINWINDO_H
#define MAINWINDO_H

#include <QMainWindow>

namespace Ui {
class MainWindo;
}

class MainWindo : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindo(QWidget *parent = nullptr);
    ~MainWindo();

private:
    Ui::MainWindo *ui;
};

#endif // MAINWINDO_H
