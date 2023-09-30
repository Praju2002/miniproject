#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_browseCompressFile_clicked();

    void on_btnCompress_clicked();


    void on_btnDecompress_clicked();

    void on_compressFileOpen_clicked();

    void on_decompressFileOpen_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
