#ifndef DUPLICATES_SCANNER_H
#define DUPLICATES_SCANNER_H

#include <QObject>
#include <QString>
#include <QDir>

class duplicates_scanner : public QObject {
    Q_OBJECT

public:

    static const quint32 BUFFER_SIZE = 1024 * 1024;

    explicit duplicates_scanner(QString const &);
    ~duplicates_scanner();

    bool files_are_equal(QString const &, QString const &);

signals:

    void onFileProcessed(QString const &);
    void onDuplicatesBucketFound(QVector<QString> const &);

    void onError(QString);

    void onComplete();

public slots:

    void startScanning();

private:

    QDir _root;

};

#endif // DUPLICATES_SCANNER_H
