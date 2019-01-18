#ifndef TESTS_H
#define TESTS_H

#include <initializer_list>
#include <string>

#include <QObject>
#include <QString>
#include <QVector>
#include <QSet>
#include <QThread>

#include "../duplicates_scanner.h"

class test : public QObject {
    Q_OBJECT

public:

    test(QString const &);
    virtual ~test();

    virtual void generate() const = 0;

protected:

    QString _name;
    QDir _dir;

};

class basic_test : public test {

public:

    basic_test(QString const &, std::initializer_list<QString> const &);
    ~basic_test();

    void generate() const override;

private:

    QVector<QString> _files_data;

};

class script_test : public test {

public:

    script_test(QString const &name, std::string const &script_path);
    ~script_test();

    void generate() const override;

private:

    std::string _script_path;

};

class duplicates_scanner_tester : public QObject {
    Q_OBJECT

public:

    duplicates_scanner_tester();
    ~duplicates_scanner_tester();

    void add_test(test const &, std::initializer_list<int> const &, std::initializer_list<QString> const &);
    void run_all();

public slots:

    void receiveDuplicatesBucket(QVector<QString> const &);
    void receiveError(QString const &);

    void checkTest();

private:

    struct full_test {
        test const &t;
        QSet<int> bucket_sizes;
        QSet<QString> errors;

        full_test();
        full_test(test const &t, QSet<int> const &, QSet<QString> const &);
        ~full_test();
    };

    duplicates_scanner *_scanner;
    QThread *_workerThread;

    QVector<full_test> _tests;
    std::size_t _test_id;
    QSet<int> _bucket_sizes;
    QSet<QString> _errors;

    std::size_t _success;

};

#endif // TESTS_H
