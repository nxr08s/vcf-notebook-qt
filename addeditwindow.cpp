#include "addeditwindow.h"
#include "ui_addeditwindow.h"

// add window
addEditWindow::addEditWindow(QWidget *parent) : QWidget(parent), ui(new Ui::addEditWindow){
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    action = ADD;
    nameFlag = false;
    phoneFlag = false;
    photoFlag = false;

    this->setWindowTitle("Добавить запись");
    ui->actionButton->setText("Добавить");

    connect(ui->actionButton,SIGNAL(clicked(bool)),this,SLOT(actionButtonClick()));
    connect(ui->cancelButton,SIGNAL(clicked(bool)),this,SLOT(cancelButtonClick()));
    connect(ui->photoOpenButton,SIGNAL(clicked(bool)),this,SLOT(fileLoadButtonClick()));
}

//edit window
addEditWindow::addEditWindow(QWidget *parent,QTableWidget* table) : QWidget(parent), ui(new Ui::addEditWindow){
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    action = EDIT;
    nameFlag = false;
    phoneFlag = false;
    photoFlag = false;

    this->setWindowTitle("Изменить запись");
    ui->actionButton->setText("Сохранить");
    pix.loadFromData(table->item(table->currentRow(),2)->data(Qt::UserRole).toByteArray(),"JPG");
    ui->photoBox->setPixmap(pix);
    ui->nameEdit->setText(table->item(table->currentRow(),0)->text());
    ui->phoneEdit->setText(table->item(table->currentRow(),1)->text());


    connect(ui->actionButton,SIGNAL(clicked(bool)),this,SLOT(actionButtonClick()));
    connect(ui->cancelButton,SIGNAL(clicked(bool)),this,SLOT(cancelButtonClick()));
    connect(ui->photoOpenButton,SIGNAL(clicked(bool)),this,SLOT(fileLoadButtonClick()));
    connect(ui->nameEdit,SIGNAL(textChanged(QString)),this,SLOT(nameEditChanged()));
    connect(ui->phoneEdit,SIGNAL(textChanged(QString)),this,SLOT(phoneEditChanged()));
}

addEditWindow::~addEditWindow(){
    delete ui;
}

void addEditWindow::actionButtonClick(){
    if ((ui->nameEdit->text() == "") | (ui->phoneEdit->text() == "")){
        QMessageBox::warning(this,"Ошибка","Все поля должны быть заполнены.",QMessageBox::Ok);
        return;
    }
    QByteArray arr;
    QBuffer buf(&arr);
    buf.open(QIODevice::WriteOnly);
    pix.save(&buf,"JPG");
    if (action == ADD)
        emit addAnswer(ui->nameEdit->text(),ui->phoneEdit->text(),arr);
    if (action == EDIT)
        emit editAnswer(ui->nameEdit->text(),ui->phoneEdit->text(),arr,nameFlag,phoneFlag,photoFlag);
    this->close();

}

void addEditWindow::cancelButtonClick(){
    this->close();
}

void addEditWindow::fileLoadButtonClick(){
    pix = QPixmap();
    if(!pix.load(QFileDialog::getOpenFileName(this,"Выбрать фото","","Images (*.png *.xpm *.jpg)"))){
        QMessageBox::warning(this,"Ошибка","Не удалось открыть файл",QMessageBox::Ok);
        return;
    }
    photoFlag = true;
    pix = pix.scaled(96,96,Qt::KeepAspectRatio);
    ui->photoBox->setPixmap(pix);
}

void addEditWindow::nameEditChanged(){
    nameFlag = true;
}

void addEditWindow::phoneEditChanged(){
    phoneFlag = true;
}
