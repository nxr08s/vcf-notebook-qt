#include "vcfexchange.h"

vcfExchange::vcfExchange(QString fname,int purposeIn,QObject *parent) : QObject(parent){
    file.setFileName(fname);
    if (purposeIn == VCF_EXPORT)
        file.open(QIODevice::WriteOnly);
    else
        file.open(QIODevice::ReadOnly);
    fstream.setDevice(&file);
}

vcfExchange::~vcfExchange(){
    file.close();
}

bool vcfExchange::exportData(QString name,QString phone,QByteArray photo){
    if (!file.isOpen())
        return false;
    fstream << "BEGIN:VCARD" << "\r\n";
    fstream <<"VERSION:2.1"<<"\r\n";
    fstream << "FN;CHARSET=UTF-8:" << name << "\r\n";
    fstream << "TEL;CELL:" << phone << "\r\n";
    if (photo.length() != 0)
        fstream << "PHOTO;ENCODING=BASE64;TYPE=IMAGE/JPEG:" << photo.toBase64() <<"\r\n";
    fstream <<"END:VCARD" << "\r\n";
    return true;
}

bool vcfExchange::importData(QString *outName,QString *outPhone,QByteArray *outPhoto){
    if (!file.isOpen())
        return false;

    bool    inPhoto = false,
            inPhone = false,
            inName = false,
            isQuotedPrintable = false,
            beginFlag,
            versionFlag,
            nameFlag;

    QByteArray photo;
    QString name,
            phone;

    QRegExp reBegin("^BEGIN"),
            reVersion("^VERSION"),
            reName("^FN"),
            reQuotedPrintable("ENCODING=QUOTED-PRINTABLE"),
            reTel("^TEL"),
            rePhoto("^PHOTO"),
            reEnd("^END");

    while(!fstream.atEnd()){

        QString     tempStr;
        tempStr = fstream.readLine();

        if(tempStr.contains(reBegin)){
            beginFlag = true;
            continue;
        }

        if(tempStr.contains(reVersion)){
            versionFlag = true;
            continue;
        }

        if(tempStr.contains(reName)){
            inPhoto = false;
            inName = true;
            inPhone = false;
            nameFlag = true;
            QStringRef clearStr(&tempStr,tempStr.indexOf(':') + 1,tempStr.length() - tempStr.indexOf(':') - 1);
            name = clearStr.toString();
            if (tempStr.contains(reQuotedPrintable)){
                isQuotedPrintable = true;
            }
            continue;
        }

        if(tempStr.contains(reTel)){
            inPhoto = false;
            inName = false;
            inPhone = true;
            QStringRef clearStr(&tempStr,tempStr.indexOf(':') + 1,tempStr.length() - tempStr.indexOf(':') - 1);
            phone = clearStr.toString();
            continue;
        }

        if(tempStr.contains(rePhoto)){
            inPhoto = true;
            inName = false;
            inPhone = false;
            QStringRef clearStr(&tempStr,tempStr.indexOf(':') + 1,tempStr.length() - tempStr.indexOf(':') - 1);
            photo.append(clearStr.toString());
            continue;
        }

        if(tempStr.contains(reEnd)){
            if (beginFlag && versionFlag && nameFlag){
                if (isQuotedPrintable)
                    name = QByteArray::fromHex(name.toLocal8Bit());
                *outName = name;
                *outPhone = phone;
                *outPhoto = photo;
                return true;
            }
            else
                return false;
        }
        if(inPhoto && !tempStr.contains(":")) photo.append(tempStr);
        if(inName && !tempStr.contains(":")) name += tempStr;
        if(inPhone && !tempStr.contains(":")) phone += tempStr;
    }
    return false;
}

bool vcfExchange::dataAvailable() const{
    return !fstream.atEnd();
}

