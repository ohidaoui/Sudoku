#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sudokugrid.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void setupMainWidget();
    void drawNumbersGrid();

    void makeConnections();
    void save(QString filename);
    void load(QString filename);


private slots:
    void on_newGame_clicked();
    void insertNumSlot();
    void on_clearButton_clicked();
    void on_solveButton_clicked();
    void loadSlot();
    void saveSlot();

private:
    Ui::MainWindow *ui;
    sudokuGrid *sdkgrid;
    QGridLayout *gl;
    QString currentFile;
};
#endif // MAINWINDOW_H
