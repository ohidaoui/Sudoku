#include "sudokugrid.h"
#include "solvedpuzzle.h"
#include <QtMath>
#include <ctime>
#include <QRegExp>
#include <QRegExpValidator>

sudokuGrid::sudokuGrid(int difficulty, QWidget *parent) : QWidget(parent)
{
    this->difficulty = difficulty;
    this->isCompleted = true;

    srand(time(0));
    glayout = new QGridLayout;
    addCells();
    setLayout(glayout);
    glayout->setHorizontalSpacing(19);
    glayout->setVerticalSpacing(19);
    setMinimumSize(510, 510);
}

sudokuGrid::~sudokuGrid()
{
    delete glayout;
    delete currentCell;
}

void sudokuGrid::paintEvent(QPaintEvent *e){
    Q_UNUSED(e);

    auto painter = new QPainter(this);
    drawSudokuGrid(painter);
}

void sudokuGrid::drawSudokuGrid(QPainter *painter){
    QPen pen(Qt::SolidLine);

    auto h = height();
    auto w = width();

    auto palette = QColor::colorNames();
    painter->fillRect(0,0,w,h, tr("#dfe4ea"));

    for (auto i=0; i<10; ++i){
        if (i % 3 == 0){
            pen.setColor(Qt::black);
            pen.setWidth(5);
        }
        else{
            pen.setColor(Qt::gray);
            pen.setWidth(2);
        }

        painter->setPen(pen);
        painter->drawLine(0, qMax(i*h/9-1,0), w, qMax(i*h/9-1,0));  // rows
        painter->drawLine(qMax(i*w/9-1,0), 0, qMax(i*w/9-1,0), h);  // columns
    }
    painter->end();
}

void sudokuGrid::addCells(){

    generatePuzzle();

    for(auto i=0; i<9*9; ++i){
        lineEdits[i] = new QLineEdit();
        auto ledit = lineEdits[i];
        ledit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ledit->setMinimumSize(40,40);
        ledit->setAlignment(Qt::AlignCenter);
//        ledit->resize(10, 10);
        glayout->addWidget(ledit, i/9, i%9,  Qt::AlignCenter);
    }

    adjustCells();
}

void sudokuGrid::adjustCells()
{
    window()->setStyleSheet("QLineEdit { qproperty-frame: false }");

    QRegExp exp("^[1-9]{0,1}$");
    QRegExpValidator *validator = new QRegExpValidator(exp);

    for(auto i=0; i<9*9; ++i){

        auto ledit = lineEdits[i];

        QFont font("Times", 10, QFont::Bold);
        ledit->setFont(font);

        if (mat[i/9][i%9] == 0) {
            ledit->setReadOnly(false);
            ledit->setText("");
            ledit->setValidator(validator);
            ledit->setStyleSheet("color: #6e45e1; background-color: #f5f7fa;");
            connect(ledit, &QLineEdit::editingFinished, this, &sudokuGrid::setCurrentCell);
            connect(ledit, &QLineEdit::textChanged, this, &sudokuGrid::checkMove);
        }
        else {
            ledit->setReadOnly(true);
            ledit->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
            ledit->setText(QString::number(mat[i/9][i%9]));
            ledit->disconnect();
        }
    }
}

void sudokuGrid::generatePuzzle(){ // K is the number of missing digits

    mat = QVector<QVector<int>>(9, QVector<int>(9,0));

    fillDiagonal();  // Fill the diagonal of 3 x 3 matrices
    fillRemainingCells(0, 3);  // Fill remaining blocks
    cluesOnly();  // Remove Randomly K digits to make game
}
void sudokuGrid::fillDiagonal(){
    // Fill the diagonal of 3 x 3 matrices
    for (int i = 0; i<9; i+=3){
        fillBox(i, i);
    }
}
void sudokuGrid::fillBox(int row,int col){
    int num;
    for (int j=0; j<3; j++)
    {
        for (int k=0; k<3; k++)
        {
            do{
                num = randomGenerator(9);
            } while (!unUsedInBox(mat, row, col, num));

            mat[row+j][col+k] = num;
        }
    }
}
bool sudokuGrid::fillRemainingCells(int i, int j){

    if (j>=9 && i<9-1){
        i = i + 1; j = 0;
    }
    if (i>=9 && j>=9)
        return true;

    if (i < 3){
        if (j < 3)
            j = 3;
    }
    else if (i < 9-3){
        if (j==(int)(i/3)*3)
            j =  j + 3;
    }
    else{
        if (j == 9-3){
            i = i + 1;
            j = 0;
            if (i>=9)
                return true;
        }
    }
    for (int num = 1; num<=9; num++){
        if (CheckIfSafe(mat, i, j, num)){
            mat[i][j] = num;
            if (fillRemainingCells(i, j+1))
                return true;
            mat[i][j] = 0;  // backtracks
        }
    }
    return false;
}
void sudokuGrid::cluesOnly() {
    auto count = 81 - this->difficulty;
    while (count != 0)
    {
        int cellId = randomGenerator(9*9)-1;
        int i = cellId/9;
        int j = cellId%9;

        if (mat[i][j] != 0)
        {
            count--;
            mat[i][j] = 0;
        }
    }
}
int sudokuGrid::randomGenerator(int num){
    return rand()%num + 1;
}
bool sudokuGrid::CheckIfSafe(QVector<QVector<int>> & grid, int i,int j,int num){
    return (unUsedInRow(grid, i, num) &&
            unUsedInCol(grid, j, num) &&
            unUsedInBox(grid, i-i%3, j-j%3, num));
}
bool sudokuGrid::unUsedInRow(QVector<QVector<int>> & grid, int i,int num){
    for (int j = 0; j<9; j++)
       if (grid[i][j] == num)
            return false;
    return true;
}
bool sudokuGrid::unUsedInCol(QVector<QVector<int>> & grid, int j,int num){
    for (int i = 0; i<9; i++)
        if (grid[i][j] == num)
            return false;
    return true;
}
bool sudokuGrid::unUsedInBox(QVector<QVector<int>> & grid, int rowStart, int colStart, int num){
    for (int i = 0; i<3; i++)
        for (int j = 0; j<3; j++)
            if (grid[rowStart+i][colStart+j]==num)
                return false;
    return true;
}

bool sudokuGrid::solve()
{
    auto emptyCell = findEmptyCell(mat);
    if (emptyCell == -1)
        return true;

    auto i = emptyCell/9;
    auto j = emptyCell%9;

    for (auto n=1; n<10; ++n) {
        if (CheckIfSafe(mat, i, j, n)){
            mat[i][j] = n;
            if (solve())
                return true;
            mat[i][j] = 0;  // backtracks
        }
    }
    return false;
}

void sudokuGrid::setGrid(QVector<QVector<int>> &grid)
{
    for (auto i=0; i<9*9; ++i) {
        if (grid[i/9][i%9] != 0)
            lineEdits[i]->setText(QString::number(grid[i/9][i%9]));
        else
            lineEdits[i]->setText("");
    }
}

QVector<QVector<int>> sudokuGrid::getGrid()
{
    QVector<QVector<int>> grid(9, QVector<int>(9,0));
    for (auto i=0; i<9*9; ++i) {
        if(!lineEdits[i]->text().isEmpty())
            grid[i/9][i%9] = lineEdits[i]->text().toInt();
    }
    return grid;
}

int sudokuGrid::findEmptyCell(QVector<QVector<int> > &grid)
{
    for (auto i=0; i<9*9; ++i)
        if (grid[i/9][i%9] == 0)
            return i;
    return -1;
}

void sudokuGrid::setCurrentCell()
{
    auto lineEdit = qobject_cast<QLineEdit*>(sender());
    currentCell = lineEdit;
}

void sudokuGrid::checkMove()
{
    auto grid = getGrid();
    isCompleted = isCompleted && (findEmptyCell(grid) == -1);

    for (auto i=0; i<9*9; ++i) {
        auto num = lineEdits[i]->text().toInt();
        if (!lineEdits[i]->isReadOnly() && num != 0){
            grid[i/9][i%9] = 0;

            if (CheckIfSafe(grid, i/9, i%9, num))
                lineEdits[i]->setStyleSheet("color: #6e45e1; background-color: #f5f7fa;");

            else {
                lineEdits[i]->setStyleSheet("color: red; background-color: #f5f7fa;");
                isCompleted = false;
            }
        }
    }
    if (isCompleted){
        SolvedPuzzle S(this);
        S.exec();
    }
}
