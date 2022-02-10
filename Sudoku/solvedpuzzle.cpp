#include "solvedpuzzle.h"
#include "ui_solvedpuzzle.h"

SolvedPuzzle::SolvedPuzzle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolvedPuzzle)
{
    ui->setupUi(this);
}

SolvedPuzzle::~SolvedPuzzle()
{
    delete ui;
}
