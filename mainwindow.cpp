#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "huffman.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::string toCompressDecompressFile;


void MainWindow::on_browseCompressFile_clicked()
{

    QString filePath = QFileDialog::getOpenFileName(this, "Select a File to Compress or Decompress (.txt/.bin)", QString(), "All Files (*);;Text Files (*.txt);;Bin Files (*.bin)");
    if (!filePath.isEmpty()) {
        // Check if the selected file has the correct extension (e.g., .txt).
        QString extension = QFileInfo(filePath).suffix().toLower();
        if (extension == "txt" || extension == "bin") {
            // The selected file has the correct extension.
            qDebug() << "Selected file path: " << filePath;
            ui->labelBrowse->setText(QFileInfo(filePath).fileName());
            toCompressDecompressFile = filePath.toStdString();
        } else {
            // The selected file does not have the correct extension.
            qDebug() << "Invalid file extension. Please select a .txt file.";
            ui->labelBrowse->setText("Invalid File Type");
        }
    }

}
void MainWindow::on_btnCompress_clicked()
{
    qDebug() << "Button Compress Clicked";
    QString qstr = QString::fromStdString(toCompressDecompressFile);
    QString qstrData = QString::fromStdString("output.bin");
    compressFile(qstr,qstrData);
}


void MainWindow::on_btnDecompress_clicked()
{
    qDebug() << "Button Decompress Clicked";
    QString qstr = QString::fromStdString(toCompressDecompressFile);
    QString qstrData = QString::fromStdString("output.txt");
    decompressFile(qstr,qstrData);
}

