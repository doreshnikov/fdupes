#ifndef DUPLICATES_RECIEVER_H
#define DUPLICATES_RECIEVER_H

#include <memory>
#include <QObject>
#include <QString>
#include <QMap>

#include <QTreeWidget>
#include <QPlainTextEdit>
#include <QProgressBar>

class duplicates_reciever : public QObject {
    Q_OBJECT

public:

    duplicates_reciever(QTreeWidget *, QPlainTextEdit *, QProgressBar *);

signals:

public slots:

    void recieveDuplicateFile(QString const &, QString const &);
    void recieveProgress(QString const &);

    void clear();

private:

    QTreeWidget *_widget;
    QPlainTextEdit *_text;
    QProgressBar *_bar;

    QMap<QString, std::shared_ptr<QTreeWidgetItem>> _duplicates;
    QMap<QString, qint64> _duplicates_count;

};

#endif // DUPLICATES_RECIEVER_H
