#include <iostream>

#include <QCoreApplication>
#include <QtDebug>

#include "tests.h"

namespace {

QString
    DIR = "./tmp",

    empty = "",
    a1 = "a",
    b1 = "b",
    zero1 = "\0",

    a256 = QString("a").repeated(256),
    b256 = QString("b").repeated(256),
    zero256 = QString("\0").repeated(256),

    a256b = a256 + "b",
    a256c = a256 + "c",

    rnd1 = "abacabadabacaba",
    rnd2 = "adaafalxknlnlxnn.kn.snldnflsnlknsldnfk;f;aj;fj;aj;wj;aj;wf\0\1afwfa",
    rnd3 = QString("abacabadabacaba").repeated(100),
    rnd4 = QString("abacabadabacaba").repeated(50) + "xyzcv";

}

test::test(QString const &name) : _name(name), _dir(QDir::currentPath()) {
    _dir.mkdir(DIR);
    _dir = QDir(_dir.filePath(DIR));
}

test::~test() {
    QDir(_dir.filePath("..")).rmdir(DIR);
}

basic_test::basic_test(QString const &name, std::initializer_list<QString> const &list) : test(name), _files_data(list) {}

basic_test::~basic_test() {}

void basic_test::generate() const {
    qDebug() << QString("[Generating  %1 ...]").arg(_name);

    QDir::current().mkdir(DIR);
    std::size_t id = 0;
    for (auto const &file_data : _files_data) {
        QFile file(_dir.filePath(QString::number(id++)));
        if (file.open(QFile::ReadWrite)) {
            file.write(file_data.toUtf8());
        }
    }
}

script_test::script_test(QString const &name, std::string const &script_path) : test(name), _script_path(script_path) {}

script_test::~script_test() {}

void script_test::generate() const {
    qDebug() << QString("[Generating %1 ...]").arg(_name);
    system(_script_path.data());
}

duplicates_scanner_tester::duplicates_scanner_tester() : _scanner(nullptr), _workerThread(nullptr), _test_id(0),  _bucket_sizes(), _errors(), _success(0) {
    std::cout << "weeeee\n";
    _scanner = new duplicates_scanner(DIR);
    _workerThread = new QThread();
    _scanner->moveToThread(_workerThread);

    connect(_scanner, &duplicates_scanner::onDuplicatesBucketFound,
            this, &duplicates_scanner_tester::receiveDuplicatesBucket);
    connect(_scanner, &duplicates_scanner::onError,
            this, &duplicates_scanner_tester::receiveError);
    connect(_scanner, &duplicates_scanner::onComplete,
            this, &duplicates_scanner_tester::checkTest);

    _workerThread->start();
}

duplicates_scanner_tester::~duplicates_scanner_tester() {
    _workerThread->requestInterruption();
    _workerThread->quit();
    delete _workerThread;
    delete _scanner;
}

duplicates_scanner_tester::full_test::full_test() : t(std::move(basic_test("empty", {}))), bucket_sizes(), errors() {}

duplicates_scanner_tester::full_test::full_test(const test &t, QSet<int> const &bucket_sizes, QSet<QString> const &errors) : t(t), bucket_sizes(bucket_sizes), errors(errors) {}

duplicates_scanner_tester::full_test::~full_test() {}

void duplicates_scanner_tester::add_test(const test &t, std::initializer_list<int> const &expect, std::initializer_list<QString> const &error) {
    QSet<int> bucket_sizes;
    for (int size : expect) {
        bucket_sizes.insert(size);
    }
    QSet<QString> errors;
    for (auto const &e : error) {
        errors.insert(e);
    }
    _tests.append(full_test(t, bucket_sizes, errors));
}

void duplicates_scanner_tester::run_all() {
    _test_id = -1;
    checkTest();
}

void duplicates_scanner_tester::receiveDuplicatesBucket(const QVector<QString> &bucket) {
    _bucket_sizes.insert(bucket.size());
}

void duplicates_scanner_tester::receiveError(const QString &error) {
    _errors.insert(error);
}

void duplicates_scanner_tester::checkTest() {
    if (0 <= _test_id && _test_id < _tests.size()) {
        if ((_bucket_sizes == _tests[_test_id].bucket_sizes) && (_errors == _tests[_test_id].errors)) {
            qDebug() << "[+] Test passed\n";
            _success++;
        } else {
            qDebug() << "[-] Test failed\n";
        }
    }
    _test_id++;
    if (_test_id < _tests.size()) {
        _tests[_test_id].t.generate();
        _scanner->startScanning();
    } else {
        qDebug() << QString("=").repeated(15) << QString("Passed: %1/%2").arg(QString::number(_success)).arg(QString::number(_tests.size()));
    }
}

int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);
    duplicates_scanner_tester tester;
    tester.add_test(
        basic_test("Empty x2", {empty, empty}),
        {2},
        {}
    );

    return a.exec();

}
