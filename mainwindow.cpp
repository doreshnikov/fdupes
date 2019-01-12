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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _dir(),
                                          _duplicates(), _duplicates_count(),
                                          _workerThread(nullptr) {
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit_Error->setReadOnly(true);
    ui->plainTextEdit_Error->setStyleSheet("QPlainTextEdit {"
                                           "    color: red;"
                                           "    backgroud-color: black;"
                                           "    font-weight: bold;"
                                           "}");
    ui->progressBar->reset();
    ui->button_Start_Scanning->setDisabled(true);

    this->statusBar()->setSizeGripEnabled(false);

    QCommonStyle style;
    ui->action_About->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeWidget->setUniformRowHeights(true);

    connect(ui->action_Select_Directory, &QAction::triggered,
            this, &MainWindow::selectDirectory);
    connect(ui->action_Delete_Selected, &QAction::triggered,
            this, &MainWindow::deleteSelected);
    connect(ui->action_Exit, &QAction::triggered,
            this, &MainWindow::close);
    connect(ui->action_About, &QAction::triggered,
            this, &MainWindow::showAboutDialog);
}

void MainWindow::interruptWorker() {
    if (_workerThread != nullptr) {
        if (_workerThread->isRunning()) {
            _workerThread->requestInterruption();
            _workerThread->wait();
        }
    }
    delete _workerThread;
    _workerThread = nullptr;
}

void MainWindow::selectDirectory() {
    _dir = QFileDialog::getExistingDirectory(this, "Please select a directory", QString(), QFileDialog::ShowDirsOnly);
    _duplicates.clear();
    _duplicates_count.clear();

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

void MainWindow::onCounted(int amount, qint64 size) {
    ui->plainTextEdit->appendPlainText(QString("%1\nActual size:\n%2B").arg(amount).arg(size));
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(amount);

    interruptWorker();

    ui->button_Start_Scanning->setDisabled(false);
    ui->button_Start_Scanning->repaint();

    disconnect(ui->button_Start_Scanning, &QPushButton::clicked,
               this, &MainWindow::stopScanning);
    connect(ui->button_Start_Scanning, &QPushButton::clicked,
            this, &MainWindow::startScanning);
}

void MainWindow::startScanning() {
    _workerThread = new QThread();

    duplicates_scanner *scanner = new duplicates_scanner(_dir);
    scanner->moveToThread(_workerThread);
    connect(_workerThread, &QThread::started,
            scanner, &duplicates_scanner::startScanning);
    connect(scanner, &duplicates_scanner::onFileProcessed,
            this, &MainWindow::receiveProgress);
    connect(scanner, &duplicates_scanner::onDuplicateFound,
            this, &MainWindow::receiveDuplicateFile);
    connect(_workerThread, &QThread::finished,
            scanner, &QObject::deleteLater);

    connect(_workerThread, &QThread::finished,
            this, &MainWindow::stopScanning);

    _workerThread->start();

    ui->button_Start_Scanning->setText("Stop scanning");

    disconnect(ui->button_Start_Scanning, &QPushButton::clicked,
               this, &MainWindow::startScanning);
    connect(ui->button_Start_Scanning, &QPushButton::clicked,
            this, &MainWindow::stopScanning);
}

void MainWindow::stopScanning() {
    interruptWorker();

    ui->button_Start_Scanning->setText("Start scanning");
    ui->button_Start_Scanning->setDisabled(true);
    ui->button_Start_Scanning->repaint();
}

void MainWindow::receiveProgress(const QString &file_name) {
    ui->plainTextEdit->appendPlainText(file_name);
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

void MainWindow::receiveDuplicateFile(QString const &origin, QString const &duplicate) {
    if (_duplicates.count(origin) == 0) {
        QTreeWidgetItem *new_item = new QTreeWidgetItem(ui->treeWidget);
        new_item->setExpanded(true);
        new_item->setText(0, QString("%1 duplicates").arg(1));
        new_item->setText(1, QString::number(QFile(origin).size()) + "B");

        _duplicates[origin] = new_item;
        _duplicates_count[origin] = 1;
        QTreeWidgetItem *item = new QTreeWidgetItem(new_item);
        item->setText(0, origin);
    }

    auto origin_item = _duplicates[origin];

    QTreeWidgetItem *item = new QTreeWidgetItem(origin_item);
    item->setText(0, duplicate);
    origin_item->setText(0, QString("%1 duplicates").arg(++_duplicates_count[origin]));
}

void MainWindow::receiveError(QString const &error) {

}

bool MainWindow::deleteOneSelected(QString const &file_name, bool &request_confirmation) {
    bool yes = false || !request_confirmation;
    if (request_confirmation) {
        auto confirmation = QMessageBox::warning(this, "Delete", QString("Delete file %1?").arg(file_name), QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::No);
        if (confirmation == QMessageBox::YesToAll) {
            request_confirmation = false;
            yes = true;
        } else if (confirmation == QMessageBox::Yes) {
            yes = true;
        }
    }

    return yes && QFile(file_name).remove();
}

void MainWindow::deleteSelected() {
    bool request_confirmation = true;

    auto selected_items = ui->treeWidget->selectedItems();
    for (auto &item : selected_items) {
        if (!item->isDisabled()) {
            if (item->childCount() != 0) {
                for (int c = 1; c < item->childCount(); c++) {
                    if (deleteOneSelected(item->child(c)->text(0), request_confirmation)) {
                        item->child(c)->setDisabled(true);
                    }
                }
            } else {
                if (deleteOneSelected(item->text(0), request_confirmation)) {
                    item->setDisabled(true);
                }
            }
        }
        item->setSelected(false);
    }

    ui->treeWidget->clearSelection();
}

MainWindow::~MainWindow() {
    interruptWorker();
}
