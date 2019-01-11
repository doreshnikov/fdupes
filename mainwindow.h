#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QMap>
#include <QThread>
#include <QFileInfo>
#include <QFileInfoList>

#include <QtWidgets>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    void interruptWorker();

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void changeCheckMode(int);

    void selectDirectory();
    void startScanning();
    void showAboutDialog();

    void onCounted(int, qint64);

    void recieveProgress(QString const &);
    void recieveDuplicateFile(QString const &, QString const &);

private:

    enum CheckMode {
        Hash,
        HashAndQuadratic,
    };

    std::unique_ptr<Ui::MainWindow> ui;
    QString _dir;
    QMap<QString, QPair<qint64, QTreeWidgetItem *>> _duplicates;
    QThread *_workerThread;

    CheckMode _check_mode;

};

#endif // MAINWINDOW_H
