#ifndef ADDEDITWINDOW_H
#define ADDEDITWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>

namespace Ui {
class addEditWindow;
}

class addEditWindow : public QWidget{
    Q_OBJECT

public:
    explicit addEditWindow(QWidget *parent);                        // add window
    explicit addEditWindow(QWidget *parent,QTableWidget* table);    // edit window
    ~addEditWindow();

private:
    enum windowPurpose{
        EDIT = 0,
        ADD = 1
    };
    Ui::addEditWindow *ui;
    int action;
    QPixmap pix;
    bool nameFlag;
    bool phoneFlag;
    bool photoFlag;

signals:
    void editAnswer(QString,QString,QByteArray,bool,bool,bool);
    void addAnswer(QString,QString,QByteArray);

private slots:
    void actionButtonClick();
    void cancelButtonClick();
    void fileLoadButtonClick();
    void nameEditChanged();
    void phoneEditChanged();
};

#endif // ADDEDITWINDOW_H
