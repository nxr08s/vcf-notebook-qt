#include "sqldatabase.h"

SqlDataBase::SqlDataBase(QString dbPath){
    QSqlQuery query;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if (!db.open()){
        QFile tempFile(dbPath);
        tempFile.open(QIODevice::WriteOnly);
        tempFile.close();
        db.open();
        query.exec("create table dataTable(Name text,Phone text,Photo blob,id integer)");
    }
}

bool SqlDataBase::addNote(QString name,QString phone,QByteArray photo,int id){
    QSqlQuery query;
    query.prepare("insert into dataTable values(:name,:phone,:photo,:id)");
    query.bindValue(":name",name);
    query.bindValue(":phone",phone);
    query.bindValue(":photo",photo);
    query.bindValue(":id",id);
    return query.exec();
}

bool SqlDataBase::updateNote(QString name,QString phone,QByteArray photo,int id,bool nameFlag,bool phoneFlag,bool photoFlag){
    QSqlQuery query;
    // updating name
    if(nameFlag){
        query.prepare("update dataTable set Name=:newName where id=:id");
        query.bindValue(":newName",name);
        query.bindValue(":id",id);
        if(!query.exec())
            return false;

    }
    // updating phone
    if(phoneFlag){
        query.prepare("update dataTable set Phone=:newPhone where id=:id");
        query.bindValue(":newPhone",phone);
        query.bindValue(":id",id);
        if(!query.exec())
            return false;
    }
    // updating photo
    if(photoFlag){
        query.prepare("update dataTable set Photo=:newPhoto where id=:id");
        query.bindValue(":newPhoto",photo);
        query.bindValue(":id",id);
        if(!query.exec())
            return false;
    }
    return true;
}

bool SqlDataBase::removeNote(int id){
    QSqlQuery query;
    query.prepare("delete from dataTable where id=:id");
    query.bindValue(":id",id);
    return query.exec();
}

bool SqlDataBase::removeAll(){
    QSqlQuery query;
    return query.exec("delete from dataTable");
}

QString SqlDataBase::getLastError(){
    return db.lastError().text();
}

bool SqlDataBase::loadWholeTable(QTableWidget *table){
    QTableWidgetItem *item;
    QSqlQuery query;
    if(!query.exec("select * from dataTable"))
        return false;
    while(query.next()){
        table->setRowCount(table->rowCount() + 1);
        int rowCount = table->rowCount() - 1;

        item = new QTableWidgetItem(query.value("Name").toString());
        table->setItem(rowCount,0,item);

        item = new QTableWidgetItem(query.value("Phone").toString());
        table->setItem(rowCount,1,item);

        item = new QTableWidgetItem;
        item->setData(Qt::UserRole,query.value("Photo"));
        table->setItem(rowCount,2,item);

        item = new QTableWidgetItem;
        item->setData(Qt::UserRole,query.value("Id"));
        table->setItem(rowCount,3,item);
    }
    return true;
}

bool SqlDataBase::isOpen(){
    return db.isOpen();
}
