#ifndef VCFEXCHANGE_H
#define VCFEXCHANGE_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QRegExp>


class vcfExchange : public QObject{
    Q_OBJECT

private:
    QFile file;
    QTextStream fstream;
public:
    enum vcfActions{
        VCF_IMPORT = 0,
        VCF_EXPORT = 1
    };
    explicit vcfExchange(QString fName,int purpose,QObject *parent = nullptr);
    ~vcfExchange();
    bool exportData(QString,QString,QByteArray);
    bool importData(QString*,QString*,QByteArray*);
    bool dataAvailable() const;
};

#endif // VCFEXCHANGE_H
