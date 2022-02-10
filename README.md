# Sudoku
Sudoku game based on Qt framework

--------

## Index
- [Components](https://github.com/ohidaoui/Sudoku#components)
- [Puzzle Widget](https://github.com/ohidaoui/Sudoku#building-the-sudoku-board-widget)
    * [Drawing the board](https://github.com/ohidaoui/Sudoku#drawing-the-grid-)
    * [the Generator](https://github.com/ohidaoui/Sudoku#generating-the-puzzle-)
    * [the Checker](https://github.com/ohidaoui/Sudoku#checking-the-moves-)
    * [the Solver](https://github.com/ohidaoui/Sudoku#implementing-a-sudoku-solver-)
- [Control Widget](https://github.com/ohidaoui/Sudoku#control-widget)
    * [Drawing the numpad](https://github.com/ohidaoui/Sudoku#control-widget)
    * [Connecting components](https://github.com/ohidaoui/Sudoku#slots)
    * [Saving and loading puzzles](https://github.com/ohidaoui/Sudoku#saving-and-loading-puzzles)
- [Final Result](https://github.com/ohidaoui/Sudoku#final-result)
- [Conclusion and future enhancements](https://github.com/ohidaoui/Sudoku#conclusion--future-enhancements-)
  
  
## Components  
The main components of this Sudoku app are :
  - **The central widget**
    * **sudokugrid** : represents the puzzle.
    * **controlWidget** : contains the control buttons and the numpad.
  - **A menubar** : contains the *File menu* that has two actions *load* and *save*.
  
## Building the sudoku board Widget
  
**`sudokuGrid`** class inherits ***QWidget*** and represents the puzzle widget with specific difficulty level.
  
### Drawing the grid :
  
**`QPainter`** class was used to draw the grid lines and to fill the containing rectangle.
  
- **painterEvent** :  `void paintEvent(QPaintEvent *e) override;`
```cpp
void sudokuGrid::paintEvent(QPaintEvent *e){
    Q_UNUSED(e);

    auto painter = new QPainter(this);
    drawSudokuGrid(painter);
}
```
- **Drawing method** :  `void drawSudokuGrid(QPainter *painter = nullptr);`
```cpp
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
```
### Adding the cells :

For this reason ths class has the following attributes:
  * `QLineEdit* lineEdits[81];` an array of pointers of type ***QLineEdit***.
  * `QVector<QVector<int>> mat;` a two dimentional array of integers storing the generated puzzle.
  
#### Generating the puzzle :

The following algorithm is based on *[this article](https://www.geeksforgeeks.org/program-sudoku-generator/)*.

Knowing that all 3x3 diadonal matricies are independent of other 3 x 3 adjacent matrices initially, as others are empty. Only box check is required. In Addition to that, while filling the rest of the non-diagonal elements, we can use a recursive backtracking (discussed after : solver) by checking 1 to 9 instead of randomly generate a test number. Once matrix is fully filled, K elements will be removed randomly (K depends on the difficulty).  

* `void generatePuzzle();`
```cpp
void sudokuGrid::generatePuzzle(){
    // Initializing mat
    mat = QVector<QVector<int>>(9, QVector<int>(9,0));
    // Filling mat
    fillDiagonal();  // Fill the diagonal 3x3 matrices
    fillRemainingCells(0, 3);  // Fill remaining blocks
    cluesOnly();  // lets only the clues and remove randomly the other digits
}
```
* `int randomGenerator(int num);` : Uses `ctime` time-related random generator to generate a number between 1 and the input
```cpp
int sudokuGrid::randomGenerator(int num){
    return rand()%num + 1;
}
```
* `void fillDiagonal();` : Fills the diagonal 3x3 matrices
```cpp
void sudokuGrid::fillDiagonal(){
    for (int i = 0; i<9; i=i+3){
        fillBox(i, i);
    }
}
```
* `void fillBox(int row,int col);` Fills a 3x3 matrix with randomly generated distinct numbers from 1 to 9.
```cpp
void sudokuGrid::fillBox(int row,int col){
    int num;
    for (int j=0; j<3; j++){
        for (int k=0; k<3; k++){
            do{
                num = randomGenerator(9);
            } while (!unUsedInBox(mat, row, col, num));
            
            mat[row+j][col+k] = num;
        }
    }
}
```
* `bool fillRemainingCells(int i, int j);`  
  
Uses a recursive backtracking to fill the remaining blocks by checking 1 to 9 and backtracks if the number does not lead to a valid result.
```cpp
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
```
* `void cluesOnly();` : Rmoves all digits except the clues
```cpp
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
```
* `bool CheckIfSafe(QVector<QVector<int>> & grid, int i,int j,int num);` : Chech if it is safe to insert *num* to *grid* in the position *(i, j)*

It is safe to insert a number between 1 and 9 in the puzzle if :
- It is not used in any other cell of the same row.
- It is not used in any other cell of the same column.
- It is not used in any other cell of the same 3x3 box.

```cpp
bool sudokuGrid::CheckIfSafe(QVector<QVector<int>> & grid, int i,int j,int num){
    return (unUsedInRow(grid, i, num) &&
            unUsedInCol(grid, j, num) &&
            unUsedInBox(grid, i-i%3, j-j%3, num));
}
```
* `bool unUsedInRow(QVector<QVector<int>> & grid, int i,int num);`
```cpp
bool sudokuGrid::unUsedInRow(QVector<QVector<int>> & grid, int i,int num){
    for (int j = 0; j<9; j++)
       if (grid[i][j] == num)
            return false;
    return true;
}
```
* `bool unUsedInCol(QVector<QVector<int>> & grid, int j,int num);`
```cpp
bool sudokuGrid::unUsedInCol(QVector<QVector<int>> & grid, int j,int num){
    for (int i = 0; i<9; i++)
        if (grid[i][j] == num)
            return false;
    return true;
}
```
* `bool unUsedInBox(QVector<QVector<int>> & grid, int rowStart, int colStart, int num);`
```cpp
bool sudokuGrid::unUsedInBox(QVector<QVector<int>> & grid, int rowStart, int colStart, int num){
    for (int i = 0; i<3; i++)
        for (int j = 0; j<3; j++)
            if (grid[rowStart+i][colStart+j]==num)
                return false;
    return true;
}
```
#### Creating and adjusting the cells
* `void addCells();`
```cpp
void sudokuGrid::addCells() {

    generatePuzzle();
    
    for(auto i=0; i<9*9; ++i){
        lineEdits[i] = new QLineEdit();
        auto ledit = lineEdits[i];
        ledit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ledit->setMinimumSize(40,40);
        ledit->setAlignment(Qt::AlignCenter);
        glayout->addWidget(ledit, i/9, i%9,  Qt::AlignCenter);
    }
    adjustCells();
}
```
* `void adjustCells();`
```cpp
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
```
### Setting the current cell
`currentCell` is a pointer of type ***QLineEdit*** that stores pointes to the current active cell. It is called when using the numpad.  
* `void setCurrentCell();` A ***slot*** that updates the value of `currentCell`
```cpp
void sudokuGrid::setCurrentCell()
{
    auto lineEdit = qobject_cast<QLineEdit*>(sender());
    currentCell = lineEdit;
}
```
### Checking the moves :
The following slot is called whenever a cell content is changed.  
It alarms the player of the unsafe entries by colloring them in red.  
If all entries are correct and the board is fully filled, the sudoku puzzle is solved and a ***SolvedPuzzle*** **Dialog** will pops up.  
> the test prevents lunching the dialog when the solver is used.
* `void checkMove();`
```cpp
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
```
### Setting and getting board content
* `void setGrid(QVector<QVector<int>> &grid);` : Sets the board content to ***grid*** values.
```cpp
void sudokuGrid::setGrid(QVector<QVector<int>> &grid)
{
    for (auto i=0; i<9*9; ++i) {
        if (grid[i/9][i%9] != 0)
            lineEdits[i]->setText(QString::number(grid[i/9][i%9]));
        else
            lineEdits[i]->setText("");
    }
}
```
* `QVector<QVector<int>> getGrid();` : Returns a 2D vector containing the board content.
```cpp
QVector<QVector<int>> sudokuGrid::getGrid()
{
    QVector<QVector<int>> grid(9, QVector<int>(9,0));
    for (auto i=0; i<9*9; ++i) {
        if(!lineEdits[i]->text().isEmpty())
            grid[i/9][i%9] = lineEdits[i]->text().toInt();
    }
    return grid;
}
```
### Implementing a Sudoku solver :
* **Backtracking**  

Backtracking is a general algorithm for finding all (or some) solutions to a problem that incrementally builds candidates to the solution. As soon as it determines that a candidate can not possibly be the solution to the problem, it abandons it *(“backtracks”)*.
When the algorithm abandons a candidate, it will typically return to a previous stage in the problem-solving process. This is the key to the algorithm and also where it gets its name.  
This principle of backtracking is used to implement the following algorithm.

* **The Algorithm**  

The goal is to fill in the numbers from 1–9 exactly once in each row, column, and 3x3 region.

  - Termination Condition :  

Since the generated puzzles are valid, they must have a solution so if the puzzle is filled, then, the solution is found and the algorithm terminates.

  - Steps :

Starting with an incomplete board :
1. Find the first empty cell
2. Attempt to place the digits 1-9 in that cell
3. Check if that digit is valid in the current cell based on the current board  
  a. If the digit is valid, recursively attempt to fill the board using steps 1-3.  
  b. If it is not valid, reset the square you just filled and go back to the previous step.  
  
Once the board is full by the definition of this algorithm we have found a solution.

* `int findEmptyCell(QVector<QVector<int>> &grid);` : Returns the ***Id*** of the first empty cell, if the grid is filled it returns -1
```cpp
int sudokuGrid::findEmptyCell(QVector<QVector<int> > &grid)
{
    for (auto i=0; i<9*9; ++i)
        if (grid[i/9][i%9] == 0)
            return i;
    return -1;
}
```
* `bool solve();` : Solves the puzzle
```cpp
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
```
  
  
## Control Widget

This widget comes with the following :
- `newGame` : a ***QPushButton*** to start a new puzzle.
- `clearButton` : a ***QPushButton*** to clear all entries.
- `solveButton` : a ***QPushButton*** to solve the puzzle.
- `levels` : a ***QComboBox*** specifying the puzzle's level of complexity.
- `numbersGrid` : the numpad.
  
  
* `void drawNumbersGrid();` : Draws the numpad.
```cpp
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
```
### Slots
* `void insertNumSlot();` : Sets the current cell content to the clicked number.
```cpp
void MainWindow::insertNumSlot(){
    auto button = qobject_cast<QPushButton*>(sender());

    if (sdkgrid->currentCell){
        sdkgrid->currentCell->setText(button->text());
    }
}
```
* `void on_newGame_clicked();`
```cpp
void MainWindow::on_newGame_clicked()
{
    delete sdkgrid;
    auto index = ui->levels->currentIndex();
    auto K = (index==0) ? 50 : ((index==1) ? 35 : 25);

    sdkgrid = new sudokuGrid(K, ui->sudokuGrid);
    gl->addWidget(sdkgrid);

    ui->level->setText(ui->levels->currentText());
}
```
* `void on_clearButton_clicked();`
```cpp
void MainWindow::on_clearButton_clicked()
{
    sdkgrid->setGrid(sdkgrid->mat);
}
```
* `void on_solveButton_clicked();`
```cpp
void MainWindow::on_solveButton_clicked()
{
    if (sdkgrid->solve())
        sdkgrid->setGrid(sdkgrid->mat);

    sdkgrid->isCompleted = false;
}
```

### Saving and loading puzzles

* `void saveSlot();` : A slot that lunches a ***getSaveFileName*** Dialog to save the current puzzle.
```cpp
void MainWindow::saveSlot()
{
    if (currentFile.isEmpty()){
        auto filename = QFileDialog::getSaveFileName(this, tr("Save File"), "/", tr("Text File (*.txt)"));
        currentFile = filename;
    }
    save(currentFile);
}
```
* `void save(QString filename);` : Saves the current puzzle
```cpp
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
```
* `void saveSlot();` : A slot that lunches a ***getOpenFileName*** Dialog to open a puzzle.
```cpp
void MainWindow::loadSlot(){
    auto filename = QFileDialog::getOpenFileName(this, tr("Open File"), "/", tr("Text Files (*.txt)"));
    if (filename != ""){
        currentFile = filename;
        load(filename);
    }
}
```
* `void load(QString filename);` : Loads a puzzle
```cpp
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
```
## Final Result

![demo](https://user-images.githubusercontent.com/92833778/153335614-356b49c8-c1e3-47cd-bd2e-4736aeb8af46.gif)
  
  
<p align="center"><img width="80%" src="https://user-images.githubusercontent.com/92833778/153344349-0ed846c8-e3a7-49b8-9181-90d11c0e1ba7.PNG"></p>
  
> Here I cheated by generating a puzzle with only few cells empty.

## Conclusion & Future enhancements :
  
In this repository I was presenting the process of making a basic C++ Sudoku using Qt framework.  
The folowing points could improve the app :
- Implement a unique-solution puzzle generator, hence, the solver will not be needed.
- Implement some advanced mathematical concepts related to puzzling to reduce the number of checks of the correctness of a Sudoku solution. [starting point](https://mathoverflow.net/questions/129143/verifying-the-correctness-of-a-sudoku-solution)
- Specifying the complexity of a sudoku puzzle considering only the number of clues or/and the number of backtracks is not enough, knowing that the human approach of solving puzzles is different, so introducing other factors as the number of logical combinations will get to a more precise generator algorithm.

