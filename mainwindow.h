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

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void changeCheckMode(int);

    void selectDirectory();
    void deleteSelected();
    void startScanning();
    void stopScanning();

    void showAboutDialog();

    void onCounted(int, qint64);

    void recieveProgress(QString const &);
    void recieveDuplicateFile(QString const &, QString const &);

    void interruptWorker();

private:

    enum CheckMode {
        Hash,
        HashAndQuadratic,
    };

    std::unique_ptr<Ui::MainWindow> ui;
    QString _dir;
    QMap<QString, QTreeWidgetItem *> _duplicates;
    QMap<QString, qint64> _duplicates_count;

    QThread *_workerThread;

    CheckMode _check_mode;

};

#endif // MAINWINDOW_H
