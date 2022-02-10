#ifndef SOLVEDPUZZLE_H
#define SOLVEDPUZZLE_H

#include <QDialog>

namespace Ui {
class SolvedPuzzle;
}

class SolvedPuzzle : public QDialog
{
    Q_OBJECT

public:
    explicit SolvedPuzzle(QWidget *parent = nullptr);
    ~SolvedPuzzle();

private:
    Ui::SolvedPuzzle *ui;
};

#endif // SOLVEDPUZZLE_H
