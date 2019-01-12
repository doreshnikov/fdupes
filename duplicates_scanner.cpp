#include "duplicates_scanner.h"

#include <QThread>
#include <QDirIterator>
#include <QHash>
#include <QVector>
#include <QCryptographicHash>

#include <QtDebug>

duplicates_scanner::duplicates_scanner(QString const &dir) : _root(dir) {}

duplicates_scanner::~duplicates_scanner() {}

bool duplicates_scanner::files_are_equal(const QString &origin, const QString &other) {
    QFile f1(origin), f2(other);
    if (!f1.open(QFile::ReadOnly)) {
        emit onError(QString("can't open file %1 for comparison with %2").arg(origin).arg(other));
    }
    if (!f2.open(QFile::ReadOnly)) {
        emit onError(QString("can't open file %1 for comparison with %2").arg(other).arg(origin));
    }

    char buffer1[256], buffer2[256];
    while (!f1.atEnd()) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            return false;
        }
        f1.read(buffer1, 256);
        f2.read(buffer2, 256);
        if (!strcmp(buffer1, buffer2)) {
            return false;
        }
    }

    return true;
}

void duplicates_scanner::startScanning() {
    QHash<qint64, QVector<QString>> buckets_by_size;

    {
        QDirIterator it(_root.absolutePath(), QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString file_name = it.next();
            qint64 file_size = QFile(file_name).size();
            if (!buckets_by_size.contains(file_size)) {
                buckets_by_size[file_size] = QVector<QString>();
            }
            buckets_by_size[file_size].append(file_name);
        }
    }

    for (auto &bucket : buckets_by_size) {
        if (bucket.size() < 2) {
            for (QString const &file_name : bucket) {
                emit onFileProcessed(file_name);
            }
            continue;
        }

        QHash<QByteArray, QString> origins;
        QHash<QString, QVector<QString>> duplicates;
        for (auto const &file_name : bucket) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                break;
            }

            QFile file(file_name);
            if (!file.open(QFile::ReadOnly)) {
                emit onError(QString("can't open file %1").arg(file_name));
                emit onFileProcessed(file_name);
                continue;
            }

            QByteArray hash;
            hash.fill(0, 256);
            file.read(hash.begin(), 256);
            file.close();

            if (origins.contains(hash) && files_are_equal(origins[hash], file_name)) {
                duplicates[origins[hash]].append(file_name);
            } else {
                origins.insert(hash, file_name);
                duplicates.insert(file_name, QVector<QString>());
                duplicates[file_name].append(file_name);
            }

            emit onFileProcessed(file_name);
        }

        for (auto const &duplicates_bucket : duplicates) {
            if (duplicates_bucket.size() < 2) {
                continue;
            }
            emit onDuplicatesBucketFound(duplicates_bucket);
        }
    }

    emit onComplete();

    QThread::currentThread()->quit();
}
