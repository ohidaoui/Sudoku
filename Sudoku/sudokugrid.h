#ifndef SUDOKUGRID_H
#define SUDOKUGRID_H

#include <QWidget>
#include <QGridLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QLineEdit>
#include <QVector>

using namespace std;

class sudokuGrid : public QWidget
{
    Q_OBJECT
public:
    explicit sudokuGrid(int difficulty = 50, QWidget *parent = nullptr);
    ~sudokuGrid();

protected:
    void paintEvent(QPaintEvent *e) override;
    void drawSudokuGrid(QPainter *painter = nullptr);
    void addCells();
    void generatePuzzle();
    int difficulty;

private:
    QGridLayout *glayout;

    int randomGenerator(int num);
    void fillDiagonal();
    void fillBox(int row,int col);
    bool fillRemainingCells(int i, int j);
    bool unUsedInBox(QVector<QVector<int>> & grid, int rowStart, int colStart, int num);
    bool CheckIfSafe(QVector<QVector<int>> & grid, int i,int j,int num);
    bool unUsedInRow(QVector<QVector<int>> & grid, int i,int num);
    bool unUsedInCol(QVector<QVector<int>> & grid, int j,int num);
    void cluesOnly();

public:
    QLineEdit* lineEdits[81];
    QLineEdit *currentCell;
    QVector<QVector<int>> mat;
    bool solve();
    void setGrid(QVector<QVector<int>> &grid);
    QVector<QVector<int>> getGrid();
    int findEmptyCell(QVector<QVector<int>> &grid);
    bool isCompleted;
    void adjustCells();

public slots:
    void setCurrentCell();
    void checkMove();

signals:

};

#endif // SUDOKUGRID_H
