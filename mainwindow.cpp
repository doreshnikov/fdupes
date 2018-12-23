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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _dir(), _duplicates(), _worker(), _check_mode(CheckMode::Hash) {
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    ui->plainTextEdit->setReadOnly(true);
    ui->progressBar->reset();

    QCommonStyle style;
    ui->action_About->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->action_Select_Directory, &QAction::triggered,
            this, &MainWindow::selectDirectory);
    connect(ui->action_Exit, &QAction::triggered,
            this, &MainWindow::close);
    connect(ui->action_About, &QAction::triggered,
            this, &MainWindow::showAboutDialog);
}

void MainWindow::selectDirectory() {
    _dir = QFileDialog::getExistingDirectory(this, "Please select a directory", QString(), QFileDialog::ShowDirsOnly);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(QString("Counting files in directory: "));
    ui->treeWidget->clear();
    ui->progressBar->reset();

    _worker.requestInterruption();
    _worker.quit();

    file_counter *counter = new file_counter(_dir);
    counter->moveToThread(&_worker);
    connect(&_worker, &QThread::started,
            counter, &file_counter::startCounting);
    connect(counter, &file_counter::onComplete,
            this, &MainWindow::onCounted);
    connect(&_worker, &QThread::finished,
            counter, &QObject::deleteLater);

    _worker.start();
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
    ui->plainTextEdit->appendPlainText(QString("%1\nActual size:\n%2").arg(amount).arg(size));
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(amount);
    ui->button_Start_Scanning->setDisabled(false);

    _worker.quit();

    duplicates_scanner *scanner = new duplicates_scanner(_dir);
    scanner->moveToThread(&_worker);
    _worker.start();
    connect(ui->button_Start_Scanning, &QPushButton::clicked,
            scanner, &duplicates_scanner::startScanning);
    connect(scanner, &duplicates_scanner::onFileProcessed,
            this, &MainWindow::recieveProgress);
    connect(scanner, &duplicates_scanner::onDuplicateFound,
            this, &MainWindow::recieveDuplicateFile);
    connect(&_worker, &QThread::finished,
            scanner, &QObject::deleteLater);

    _worker.start();
}

void MainWindow::recieveProgress(const QString &file_name) {
    ui->plainTextEdit->appendPlainText(file_name);
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

void MainWindow::recieveDuplicateFile(QString const &origin, QString const &duplicate) {
    qDebug() << origin << ' ' << duplicate << '\n';
    QTreeWidgetItem *new_item = new QTreeWidgetItem(ui->treeWidget);
    new_item->setExpanded(true);
    new_item->setText(0, "found");
    new_item->setText(1, "size");

    QTreeWidgetItem *item = new QTreeWidgetItem(new_item);
    item->setText(0, origin);
    item->setText(1, duplicate);
    ui->treeWidget->addTopLevelItem(item);
}

MainWindow::~MainWindow() {}
