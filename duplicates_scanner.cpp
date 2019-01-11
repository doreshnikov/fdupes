#include "duplicates_scanner.h"

#include <QThread>
#include <QDirIterator>
#include <QHash>
#include <QVector>
#include <QCryptographicHash>

#include <QtDebug>

duplicates_scanner::duplicates_scanner(QString const &dir) : _root(dir) {}

duplicates_scanner::~duplicates_scanner() {}

void duplicates_scanner::startScanning() {
    QDirIterator it(_root.absolutePath(), QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);
    QHash<QByteArray, QString> origins;

    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        QString file_path = it.next();
        emit onFileProcessed(file_path);

        QFile file(file_path);
        if (file.size() == 0) {
            continue;
        }
        QCryptographicHash hasher(QCryptographicHash::Md5);
        hasher.reset();
        if (!file.open(QFile::ReadOnly)) {
            continue;
        }
        hasher.addData(&file);
        QByteArray hash = hasher.result();
        if (origins.contains(hash)) {
            emit onDuplicateFound(origins[hash], file_path);
        } else {
            origins.insert(hash, file_path);
        }
    }

    emit onComplete();
    QThread::currentThread()->quit();
}
