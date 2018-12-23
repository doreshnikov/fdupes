#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_status.h"

#include <memory>
#include <QMainWindow>
#include <QMap>
#include <QThread>
#include <QFileInfo>
#include <QFileInfoList>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void changeCheckMode(int);

    void selectDirectory();
    void showAboutDialog();

    void onCounted(int, qint64);

    void recieveProgress(QString const &);
    void recieveDuplicateFile(QString const &, QString const &);

private:

    enum CheckMode {
        Hash,
        HashAndQuadratic,
    };

    enum Status {
        Init,
        Count,
        Scan,
        Ready,
        Interrupt,
    };

    std::unique_ptr<Ui::MainWindow> ui;
    QString _dir;
    QMap<QFileInfo, QFileInfoList> _duplicates;
    QThread _worker;

    CheckMode _check_mode;
    Status _status;

};

#endif // MAINWINDOW_H
