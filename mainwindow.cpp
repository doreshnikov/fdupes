#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "file_counter.h"
#include "duplicates_scanner.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QThread>
#include <QtWidgets>

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _dir(), _duplicates(), _workerThread(nullptr), _check_mode(CheckMode::Hash) {
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    ui->plainTextEdit->setReadOnly(true);
    ui->progressBar->reset();
    this->statusBar()->setSizeGripEnabled(false);

    QCommonStyle style;
    ui->action_About->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
//    ui->treeWidget->setColumnWidth(1, 100);
//    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
//    ui->treeWidget->setAnimated(false);

    connect(ui->action_Select_Directory, &QAction::triggered,
            this, &MainWindow::selectDirectory);
    connect(ui->action_Exit, &QAction::triggered,
            this, &MainWindow::close);
    connect(ui->action_About, &QAction::triggered,
            this, &MainWindow::showAboutDialog);
}

void MainWindow::interruptWorker() {
    if (_workerThread != nullptr) {
        _workerThread->requestInterruption();
        _workerThread->wait();
        _workerThread->deleteLater();
    }
    delete _workerThread;
}

void MainWindow::selectDirectory() {
    _dir = QFileDialog::getExistingDirectory(this, "Please select a directory", QString(), QFileDialog::ShowDirsOnly);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(QString("Counting files in directory: "));
    ui->treeWidget->clear();
    ui->progressBar->reset();

    interruptWorker();
    _workerThread = new QThread();

    file_counter *counter = new file_counter(_dir);
    counter->moveToThread(_workerThread);
    connect(_workerThread, &QThread::started,
            counter, &file_counter::startCounting);
    connect(counter, &file_counter::onComplete,
            this, &MainWindow::onCounted);
    connect(_workerThread, &QThread::finished,
            counter, &QObject::deleteLater);

    _workerThread->start();
}

void MainWindow::showAboutDialog() {
    QMessageBox::about(this, "fdupes", "Files duplicates finder utility");
}

void MainWindow::changeCheckMode(int tick) {
    if (tick) {
        _check_mode = CheckMode::HashAndQuadratic;
    } else {
        _check_mode = CheckMode::Hash;
    }
}

void MainWindow::onCounted(int amount, qint64 size) {
    ui->plainTextEdit->appendPlainText(QString("%1\nActual size:\n%2B").arg(amount).arg(size));
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(amount);
    ui->button_Start_Scanning->setDisabled(false);

    interruptWorker();
    _workerThread = new QThread();

    duplicates_scanner *scanner = new duplicates_scanner(_dir);
    scanner->moveToThread(_workerThread);
    connect(ui->button_Start_Scanning, &QPushButton::clicked,
            this, &MainWindow::startScanning);
    connect(_workerThread, &QThread::started,
            scanner, &duplicates_scanner::startScanning);
    connect(scanner, &duplicates_scanner::onFileProcessed,
            this, &MainWindow::recieveProgress);
    connect(scanner, &duplicates_scanner::onDuplicateFound,
            this, &MainWindow::recieveDuplicateFile);
    connect(_workerThread, &QThread::finished,
            scanner, &QObject::deleteLater);
}

void MainWindow::startScanning() {
    _workerThread->start();
}

void MainWindow::recieveProgress(const QString &file_name) {
    ui->plainTextEdit->appendPlainText(file_name);
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

void MainWindow::recieveDuplicateFile(QString const &origin, QString const &duplicate) {
    if (_duplicates.count(origin) == 0) {
        QTreeWidgetItem *new_item = new QTreeWidgetItem(ui->treeWidget);
        new_item->setExpanded(true);
        new_item->setText(0, origin);
        new_item->setText(1, "1");
        _duplicates[origin] = qMakePair(1, new_item);
        ui->treeWidget->addTopLevelItem(new_item);
    }

    auto &origin_item = _duplicates[origin];

    QTreeWidgetItem *item = new QTreeWidgetItem(origin_item.second);
    item->setText(0, duplicate);
    origin_item.second->setText(1, QString::number(++origin_item.first));
}

MainWindow::~MainWindow() {
    interruptWorker();
}
