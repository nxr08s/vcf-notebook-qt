#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QtSql>
#include <QFile>
#include <QTableWidget>
#include <QTableWidgetItem>

class SqlDataBase{

private:
    QSqlDatabase db;

public:
    SqlDataBase(QString);
    bool addNote(QString,QString,QByteArray,int);
    bool updateNote(QString,QString,QByteArray,int,bool,bool,bool);
    bool removeNote(int);
    bool removeAll();
    QString getLastError();
    bool loadWholeTable(QTableWidget*);
    bool isOpen();

};

#endif // SQLDATABASE_H
