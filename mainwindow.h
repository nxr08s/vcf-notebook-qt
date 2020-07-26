#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QMessageBox>

#include "sqldatabase.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    SqlDataBase *db;

    QAction *addAct;
    QAction *importAct;
    QAction *exportAct;
    QAction *clearAct;

    int getFreeId();


private slots:
    void exportAction();
    void importAction();
    void addAction();
    void clearTableAction();
    void contextMenutable();
    void editAction();
    void removeAction();
    void clickOnTable(int);
    void editWindowAnswer(QString,QString,QByteArray,bool,bool,bool);
    void addWindowAnswer(QString,QString,QByteArray);
    void exportItemAction();
};

#endif // MAINWINDOW_H
