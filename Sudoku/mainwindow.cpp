#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupMainWidget();
    setWindowTitle("Sudoku");
    currentFile = "";
    makeConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete sdkgrid;
    delete gl;
}

void MainWindow::setupMainWidget(){

    sdkgrid = new sudokuGrid(50, ui->sudokuGrid);

    gl = new QGridLayout;
    gl->addWidget(sdkgrid);
    ui->sudokuGrid->setLayout(gl);

    drawNumbersGrid();
}

void MainWindow::drawNumbersGrid(){

    auto gLayout = new QGridLayout;

    QString style{"QPushButton {"
          "padding: 0 8px;"
          "background: #dfe4ea;"
          "border: 1px solid red;"
          "border-radius: 5px;"
          "color: #ff6b81;"
          "font: bold 20px;}"
          "QPushButton:pressed {"
          "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);}"};

    for (auto i=0; i<9; ++i){
        auto button = new QPushButton(tr("%0").arg(i+1));
        button->setStyleSheet(style);
        button->setMinimumSize(50,50);

        gLayout->addWidget(button, i/3, i%3);
        connect(button, &QPushButton::clicked, this, &MainWindow::insertNumSlot);
    }
    ui->numbersGrid->setLayout(gLayout);
}

void MainWindow::makeConnections()
{
    connect(ui->action_Load_file, &QAction::triggered, this, &MainWindow::loadSlot);
    connect(ui->action_Save, &QAction::triggered, this, &MainWindow::saveSlot);
}

void MainWindow::insertNumSlot(){
    auto button = qobject_cast<QPushButton*>(sender());

    if (sdkgrid->currentCell){
        sdkgrid->currentCell->setText(button->text());
    }
}

void MainWindow::on_newGame_clicked()
{
    delete sdkgrid;
    auto index = ui->levels->currentIndex();
    auto K = (index==0) ? 50 : ((index==1) ? 35 : 25);

    sdkgrid = new sudokuGrid(K, ui->sudokuGrid);
    gl->addWidget(sdkgrid);

    ui->level->setText(ui->levels->currentText());
    currentFile = "";
}

void MainWindow::on_clearButton_clicked()
{
    sdkgrid->setGrid(sdkgrid->mat);
}

void MainWindow::on_solveButton_clicked()
{
    if (sdkgrid->solve())
        sdkgrid->setGrid(sdkgrid->mat);

    sdkgrid->isCompleted = false;
}

void MainWindow::saveSlot()
{
    if (currentFile.isEmpty()){
        auto filename = QFileDialog::getSaveFileName(this, tr("Save File"), "/", tr("Text File (*.txt)"));
        currentFile = filename;
    }
    save(currentFile);
}

void MainWindow::save(QString filename)
{
    // Pointer to the file
    QFile file(filename);

    // Open the file in Write mode
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        for (auto i=0; i<9*9; ++i){
            auto ledit = sdkgrid->lineEdits[i];
            out << ledit->isReadOnly() << ", " << i << ", " << ledit->text() << Qt::endl;
        }
    }
    // Close the file
    file.close();
}

void MainWindow::loadSlot(){
    auto filename = QFileDialog::getOpenFileName(this, tr("Open File"), "/", tr("Text Files (*.txt)"));
    if (filename != ""){
        currentFile = filename;
        load(filename);
    }
}

void MainWindow::load(QString filename){
    QFile file(filename);
    QRegExpValidator validator(QRegExp("^[1-9]{0,1}$"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream qin(&file);
        QString line;
        QStringList tokens;
        QVector<QVector<int>> grid(9, QVector<int>(9, 0));

        while (!qin.atEnd()) {
            line = qin.readLine();
            tokens = line.split(QChar(','));
            if (tokens[2] == "" || tokens[2] == " ")
                tokens[2] = "0";

            auto isReadOnly = (tokens[0].toInt() == 1);
            auto index = tokens[1].toInt();
            auto value = tokens[2].toInt();

            auto i = index/9;
            auto j = index%9;

            sdkgrid->mat[i][j] = 0;

            if (isReadOnly) {
                sdkgrid->mat[i][j] = value;
                grid[i][j] = value;
            }else{
                grid[i][j] = value;
            }
        }
        sdkgrid->adjustCells();
        sdkgrid->setGrid(grid);
    }
    file.close();
}
