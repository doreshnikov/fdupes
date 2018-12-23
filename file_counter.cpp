#include "file_counter.h"

#include <QDirIterator>
#include <QThread>

file_counter::file_counter(QString const &dir) : _root(dir) {}

file_counter::~file_counter() {}

void file_counter::startCounting() {
    QDirIterator it(_root.absolutePath(), QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);

    int amount = 0;
    qint64 size = 0;
    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        amount++;
        size += it.fileInfo().size();
        it.next();
    }

    emit onComplete(amount, size);
    QThread::currentThread()->quit();
}
