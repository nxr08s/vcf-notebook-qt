#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "addeditwindow.h"
#include "sqldatabase.h"
#include "vcfexchange.h"

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);

    // Set tableMenu
    addAct = new QAction("Добавить запись",ui->tableMenu);
    importAct = new QAction("Импорт",ui->tableMenu);
    exportAct = new QAction("Экспорт всех записей",ui->tableMenu);
    clearAct = new QAction("Очистить таблицу",ui->tableMenu);

    ui->tableMenu->addAction(addAct);
    ui->tableMenu->addSeparator();
    ui->tableMenu->addAction(importAct);
    ui->tableMenu->addAction(exportAct);
    ui->tableMenu->addSeparator();
    ui->tableMenu->addAction(clearAct);

    connect(addAct,SIGNAL(triggered(bool)),this,SLOT(addAction()));
    connect(importAct,SIGNAL(triggered(bool)),this,SLOT(importAction()));
    connect(exportAct,SIGNAL(triggered(bool)),this,SLOT(exportAction()));
    connect(clearAct,SIGNAL(triggered(bool)),this,SLOT(clearTableAction()));
    connect(ui->tableWidget,SIGNAL(cellActivated(int,int)),this,SLOT(clickOnTable(int)));
    connect(ui->tableWidget,SIGNAL(cellPressed(int,int)),this,SLOT(clickOnTable(int)));

    // Set table headers
    QStringList headers;
    headers << "Имя" << "Номер телефона";
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->hideColumn(2); // hide photo column
    ui->tableWidget->hideColumn(3); // hide id column

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Set context menu for table

    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenutable()));
    connect(ui->tableWidget,SIGNAL(cellEntered(int,int)),this,SLOT(clickOnTable(int)));

    // Load from db & create table
    db = new SqlDataBase("phoneTable.db3");
    db->loadWholeTable(ui->tableWidget);
}

MainWindow::~MainWindow(){
    delete ui;
}

int MainWindow::getFreeId(){
    int id = -1;
    bool check;
    for (int i = 0 ; i <= 100 ; i++){
        check = false;
        for(int j = 0 ; j < ui->tableWidget->rowCount() ; j++){
            if (ui->tableWidget->item(j,3)->data(Qt::UserRole).toInt() == i){
                check = true;
                break;
            }
        }
        if (!check){
            id = i;
            break;
        }
    }
    return id;
}

void MainWindow::exportAction(){
    QString fileName = QFileDialog::getExistingDirectory(this,"Выберите путь сохранения файла","/home",QFileDialog::ShowDirsOnly);
    fileName+="\\export.vcf";
    qDebug() << fileName;
    vcfExchange vcf(fileName,vcfExchange::VCF_EXPORT);
    for(int i = 0 ; i < ui->tableWidget->rowCount();i++)
        vcf.exportData(
                    ui->tableWidget->item(i,0)->text(),
                    ui->tableWidget->item(i,1)->text(),
                    ui->tableWidget->item(i,2)->data(Qt::UserRole).toByteArray()
                    );
}

void MainWindow::importAction(){
    QString fname = QFileDialog::getOpenFileName(this,"Выберите файл для импорта","/home","vCard файлы (*.vcf)");
    vcfExchange vcf(fname,vcfExchange::VCF_IMPORT);

    while(vcf.dataAvailable()){
        QString name,
                phone;
        QByteArray photo;
        if(vcf.importData(&name,&phone,&photo)){
            QTableWidgetItem *item;
            int id = getFreeId();
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());

            item = new QTableWidgetItem(name);
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,0,item);

            item = new QTableWidgetItem(phone);
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,1,item);

            QPixmap pix;
            pix.loadFromData(QByteArray::fromBase64(photo),"JPG");
            pix = pix.scaled(96,96,Qt::KeepAspectRatio);
            QByteArray arr;
            QBuffer buf(&arr);
            buf.open(QIODevice::WriteOnly);
            pix.save(&buf,"JPG");
            photo = arr;
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole,photo);
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,2,item);

            item = new QTableWidgetItem;
            item->setData(Qt::UserRole,id);
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,3,item);

            db->addNote(name,phone,photo,id);
        }
    }
}

void MainWindow::addAction(){
    addEditWindow *wnd = new addEditWindow(nullptr);
    connect(wnd,SIGNAL(addAnswer(QString,QString,QByteArray)),this,SLOT(addWindowAnswer(QString,QString,QByteArray)));
    wnd->show();
}

void MainWindow::clearTableAction(){
    if(ui->tableWidget->rowCount() == 0)
        return;
    if(QMessageBox::question(this,"Очистка таблицы","Вы действительно хотите очистить таблицу\n(данные будут потеряны навсегда)?","Да","Нет"))
        return;
    db->removeAll();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
}

void MainWindow::editAction(){
    addEditWindow *wnd = new addEditWindow(nullptr,ui->tableWidget);
    connect(wnd,SIGNAL(editAnswer(QString,QString,QByteArray,bool,bool,bool)),this,SLOT(editWindowAnswer(QString,QString,QByteArray,bool,bool,bool)));
    wnd->show();
}

void MainWindow::removeAction(){
    db->removeNote(ui->tableWidget->item(ui->tableWidget->currentRow(),3)->data(Qt::UserRole).toInt());
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
}

void MainWindow::contextMenutable(){

    QMenu *tablContexteMenu = new QMenu;

    tablContexteMenu->addAction("Изменить",this,SLOT(editAction()));
    tablContexteMenu->addSeparator();
    tablContexteMenu->addAction("Экспортировать",this,SLOT(exportItemAction()));
    tablContexteMenu->addSeparator();
    tablContexteMenu->addAction("Удалить",this,SLOT(removeAction()));

    tablContexteMenu->exec(QCursor::pos());
}

void MainWindow::clickOnTable(int x){
    QPixmap px;
    px.loadFromData(ui->tableWidget->item(x,2)->data(Qt::UserRole).toByteArray(),"JPG");
    ui->photoBox->setPixmap(px);
    ui->nameLabel->setText(ui->tableWidget->item(x,0)->text());
    ui->phoneLabel->setText(ui->tableWidget->item(x,1)->text());
}

void MainWindow::editWindowAnswer(QString name, QString phone, QByteArray photo, bool nameFlag, bool phoneFlag, bool photoFlag){
    db->updateNote(name,phone,photo,ui->tableWidget->item(ui->tableWidget->currentRow(),3)->data(Qt::UserRole).toInt(),nameFlag,phoneFlag,photoFlag);
    QTableWidgetItem *item;
    if(nameFlag){
        delete ui->tableWidget->item(ui->tableWidget->currentRow(),0);
        item = new QTableWidgetItem(name);
        ui->tableWidget->setItem(ui->tableWidget->currentRow(),0,item);
    }
    if(phoneFlag){
        delete ui->tableWidget->item(ui->tableWidget->currentRow(),1);
        item = new QTableWidgetItem(phone);
        ui->tableWidget->setItem(ui->tableWidget->currentRow(),1,item);
    }
    if(photoFlag){
        delete ui->tableWidget->item(ui->tableWidget->currentRow(),2);
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole,photo);
        ui->tableWidget->setItem(ui->tableWidget->currentRow(),2,item);
    }
}

void MainWindow::addWindowAnswer(QString name, QString phone, QByteArray photo){

    int id = getFreeId();

    db->addNote(name,phone,photo,id);

    ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
    int rowCount = ui->tableWidget->rowCount() - 1;

    QTableWidgetItem *item;

    item = new QTableWidgetItem(name);
    ui->tableWidget->setItem(rowCount,0,item);

    item = new QTableWidgetItem(phone);
    ui->tableWidget->setItem(rowCount,1,item);

    item = new QTableWidgetItem;
    item->setData(Qt::UserRole,photo);
    ui->tableWidget->setItem(rowCount,2,item);

    item = new QTableWidgetItem(id);
    ui->tableWidget->setItem(rowCount,3,item);
}

void MainWindow::exportItemAction(){
    QString fname = QFileDialog::getSaveFileName(this,"Выберите путь сохранения",ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text() + ".vcf");
    vcfExchange xprt(fname,vcfExchange::VCF_EXPORT);
    xprt.exportData(
            ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text(),
            ui->tableWidget->item(ui->tableWidget->currentRow(),1)->text(),
            ui->tableWidget->item(ui->tableWidget->currentRow(),2)->data(Qt::UserRole).toByteArray()
            );
}
