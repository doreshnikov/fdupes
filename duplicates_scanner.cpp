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
    QHash<QByteArray, QString> origins;
    QHash<qint64, qint64> count_by_size;

    {
        QDirIterator it(_root.absolutePath(), QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            qint64 file_size = QFile(it.next()).size();
            count_by_size[file_size]++;
        }
    }
    QDirIterator it(_root.absolutePath(), QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        QString file_path = it.next();
        emit onFileProcessed(file_path);

        QFile file(file_path);
        if (file.size() == 0 || count_by_size[file.size()] < 2) {
            continue;
        }
        QCryptographicHash hasher(QCryptographicHash::RealSha3_512);
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
