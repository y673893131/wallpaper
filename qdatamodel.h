#ifndef QDATAMODEL_H
#define QDATAMODEL_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QVector>
#include <QSqlQuery>
class QDataModel : public QObject
{
    Q_OBJECT
public:
    explicit QDataModel(QObject *parent = nullptr);

    void load();
signals:
    void loadsuccess(const QVector<QStringList>&);
public slots:
    void onAddUrl(const QString& file);
private:
    void reportError(const QSqlQuery&);
private:
    QSqlDatabase m_db;
};

#endif // QDATAMODEL_H
