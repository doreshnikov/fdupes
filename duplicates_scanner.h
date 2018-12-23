#ifndef DUPLICATES_SCANNER_H
#define DUPLICATES_SCANNER_H

#include <QObject>
#include <QString>
#include <QDir>

class duplicates_scanner : public QObject {
    Q_OBJECT

public:

    explicit duplicates_scanner(QString const &);
    ~duplicates_scanner();

signals:

    void onFileProcessed(QString const &);
    void onDuplicateFound(QString const &, QString const &);
    void onScanComplete();

public slots:

    void startScanning();

private:

    QDir _root;

};

#endif // DUPLICATES_SCANNER_H
