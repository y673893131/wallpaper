#include "qdatamodel.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
QDataModel::QDataModel(QObject *parent) : QObject(parent)
{
    qDebug() << "db:" << QSqlDatabase::connectionNames();
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("./data.tmp");
    if(!m_db.open()){
        QMessageBox::warning(nullptr, tr("error"), tr("cannot load data."));
    }
    else{
        QSqlQuery query;
        auto bCreate = query.exec("CREATE TABLE background_info ("
                   "name VARCHAR(40) NOT NULL, "
                   "url VARCHAR(256) NOT NULL, "
                   "access_time TimeStamp NOT NULL DEFAULT(datetime('now','localtime')), PRIMARY KEY (name, url)) ");
        if(!bCreate){
            auto error = query.lastError();
            qDebug() << error.text();
            if(!error.text().contains("already exists"))
                QMessageBox::warning(nullptr, tr("warning"), error.text() + "("+ error.nativeErrorCode() + ")");
        }
        query.clear();
    }
}

void QDataModel::load()
{
    QVector<QStringList> data;
    QSqlQuery query;
    auto bquery = query.exec("select name, url from background_info order by access_time desc limit 0, 10; ");
    if(bquery){
        auto record = query.record();
        auto count = record.count();
        while(query.next())
        {
            auto name = query.value(0).toString();
            auto url = query.value(1).toString();
            QStringList s;
            s << name << url;
            data.push_back(s);
        }
    }

    query.clear();
    emit loadsuccess(data);
}

void QDataModel::onAddUrl(const QString &file)
{
    auto name = file.mid(file.lastIndexOf('/') + 1);
    QSqlQuery query;
    auto sql = QString("replace into background_info ('name', 'url') values ('%1', '%2');").arg(name).arg(file);
//    qDebug() << sql;
    auto bquery = query.exec(sql);
    if(!bquery) {
        reportError(query);
        return;
    }

    bquery = query.exec(QString("delete from background_info where url not in (select url from background_info order by access_time desc limit 0, 10);"));
    if(!bquery) reportError(query);
    load();
    query.clear();
}

void QDataModel::reportError(const QSqlQuery & query)
{
    auto error = query.lastError();
    QMessageBox::warning(nullptr, tr("warning"), error.text() + "("+ error.nativeErrorCode() + ")");
}
