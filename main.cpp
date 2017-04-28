#include <stdio.h>
#include <iostream>
#include <iomanip>
//#include <ctime>
//#include <string>
#include "conio.h"

#define getch getchar
#define EMPTY_FIELD 'o'
#define SHIP_FIELD '#'
#define HIT_FIELD 'X'
#define MISS_FIELD 'O'
#define MARKED_SHIP_FIELD 'M'
#define MARK_FIELD '0'

#define RESET   "\033[0m"       //*Reset to default*/
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
//#define WHITE   "\033[37m"      /* White */


using namespace std;

const int rows = 10;
const int cols = 10;
const int maxShipsCount[4] = {4, 3, 2, 1};
const int shipsSize[4] = {1, 2, 3, 4};

//--------------------------------------------------------------------------------------------------------------------//
//FUNCTION PROTOTYPES                                                                                                 //
//--------------------------------------------------------------------------------------------------------------------//
//DRAWING FUNCTIONS//
void drawBanner();
void showMenu(string, string*, int);
void playerWon(char[rows][cols], char[rows][cols]);
void playerLose(char[rows][cols], char[rows][cols]);
void drawGrid(char[rows][cols], bool = true, int = -1, int = -1);
void showGrids(char[rows][cols], char[rows][cols], int = -1, int = -1, bool = false);
void drawShip(int, int, int, char[rows][cols], bool);
void drawField(char, bool, bool = false);

//--------------------------------------------------------------------------------------------------------------------//
//GAME MENUS
void mainMenu();
void startGame();
bool shipsPlacementMenu(char[rows][cols]);
bool placeShips(char[rows][cols]);
bool placeShip(int, char[rows][cols]);
int playerFire(char[rows][cols], char[rows][cols], int&, int&);

//--------------------------------------------------------------------------------------------------------------------//
//GAME MAIN FUNCTIONALITY
void generateShips(char[rows][cols]);
void findAvailablePlace(int, char[rows][cols], int&, int&, bool&);
int fire(int, int, char[rows][cols]);
bool markField(int, int, char[rows][cols]);

//--------------------------------------------------------------------------------------------------------------------//
// FIELDS CHECKERS
bool isEmpty(char);
bool isHit(int, int, char[rows][cols]);
bool canAddShips(int *);
bool canAddShip(int *, int);
bool canPlaceShip(int, char[rows][cols], int, int, bool);
bool canFire(int, int, char[rows][cols]);
bool isShipSunk(int, int, char[rows][cols]);
bool assessGameState(char[rows][cols]);

//--------------------------------------------------------------------------------------------------------------------//
// AI FUNCTIONALITY
void resetAiData(char[rows][cols], int&, int&, int&, int&, int[4], int&);
//bool aiFire(char[rows][cols], char[rows][cols], int&, int&, int&, int&, int[4]);
bool canAiFire(int, int, char[rows][cols]);
int determineMoveDirection(int[4]);
bool determineNextShot(char[rows][cols], int&, int&, int);
bool canStepInto(char[rows][cols], int, int);

//--------------------------------------------------------------------------------------------------------------------//
//HELPERS
string centered(int, const string&);
void copyMatrix(char[][cols], char[][cols]);
void clearGrid(char[rows][cols]);

//--------------------------------------------------------------------------------------------------------------------//

int main(int argc, char *argv[]) {
    srand( (unsigned int) time(NULL));
//    int x = 0, y = 0;
//    int i = x, j = y;

//    char m[rows][cols] = {
//            {'#', '#', 'o', 'o', 'o', 'X', 'o', 'o', 'o', 'o'},
//            {'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o'},
//            {'o', 'o', 'o', 'o', '#', 'o', 'o', 'o', 'o', 'o'},
//            {'o', 'o', 'o', 'o', '#', 'o', 'X', 'o', 'o', 'o'},
//            {'o', 'o', 'X', 'o', '#', 'o', 'o', 'o', 'o', 'o'},
//            {'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o'},
//            {'o', 'o', 'o', 'o', 'X', 'o', 'o', 'o', 'o', 'o'},
//            {'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', '#'},
//            {'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o'},
//            {'o', 'o', 'o', 'o', 'o', 'o', '#', '#', '#', '#'}
//    };
//    drawGrid(m);
//    int count = 0;
//    while (!isShipSunk(i, j, m) || count++ < 8) {
//        aiFire(m, x, y);
//    }
//    return 0;
    mainMenu();

    getch();
    return 0;
}

void mainMenu() {
    char action;
    string title = "MAIN MENU";
    string menuItems[] = {
            "(1) START GAME",
            "(2) QUIT"
    };

    while (true) {
        clear();

        drawBanner();
        showMenu(title, menuItems, 2);

        action = getch();
        switch (action) {
            case '1':
                startGame();
                break;
            case '2':
                return;
            default:
                break;
        }
    }
}

void startGame() {
    char playerGrid[rows][cols];
    char enemyGrid[rows][cols];

    if (!shipsPlacementMenu(playerGrid)) {
        return;
    }

    generateShips(enemyGrid);
    int gameOver;
    int pLastX = 0, pLastY = 0,
        aiX = -1, aiY = -1,
        aiLastHitX = -1, aiLastHitY = -1,
        directions[4] = {0, 0, 0, 0},
        aiFireDirection = -1;
    bool aiHit, aiShipSunk;

    do {
//        gameOver = playerFire(playerGrid, enemyGrid, pLastX, pLastY);
//        if (gameOver == 1) {
//            playerWon(playerGrid, enemyGrid);
//            return;
//        } else if (gameOver == -1) {
//            return;
//        }

        if (aiFireDirection < 0) {
            resetAiData(playerGrid, aiX, aiY, aiLastHitX, aiLastHitY, directions, aiFireDirection);
        }

        clear();
        drawBanner();
        do {
            aiHit = fire(aiX, aiY, playerGrid) == 1;

            showGrids(playerGrid, enemyGrid, aiX, aiY, true);
//            getch();

            aiShipSunk = aiHit && isShipSunk(aiX, aiY, playerGrid);

            if (aiHit) {
                if (aiShipSunk) {
                    if (assessGameState(playerGrid)) {
                        playerLose(playerGrid, enemyGrid);
                        return;
                    }
                    resetAiData(playerGrid, aiX, aiY, aiLastHitX, aiLastHitY, directions, aiFireDirection);
                } else {
                    if (aiFireDirection < 0) {
                        aiLastHitX = aiX;
                        aiLastHitY = aiY;
                    } else {
                        directions[aiFireDirection] = 1;
                    }
                }
            } else if (aiFireDirection > 0) {
                directions[aiFireDirection] = directions[aiFireDirection] == 1 ? 2 : -1;
            }

            if ((aiHit && !aiShipSunk) || (!aiHit && aiFireDirection > 0)) {
                if (aiFireDirection > 0) {
                    aiX = aiLastHitX;
                    aiY = aiLastHitY;
                }
                bool shotDetected;
                do {
                    aiFireDirection = determineMoveDirection(directions);
                    shotDetected = determineNextShot(playerGrid, aiX, aiY, aiFireDirection);
                    if (!shotDetected) {
                        directions[aiFireDirection] = directions[aiFireDirection] == 1 ? 2 : -1;
                    }
                } while (!shotDetected);
            }
        } while (aiHit);
    } while (true);
}

bool shipsPlacementMenu(char grid[rows][cols]) {
    char action;
    string title = "Do you want to place your ships manually?";
    string menuItems[] = {
            "(1) Place ships automatically",
            "(2) Place ships manually",
            "(Q) Return to main menu"
    };

    while (true) {
        clear();
        drawBanner();
        showMenu(title, menuItems, 2);

        action = getch();
        switch (action) {
            case '1':
                generateShips(grid);
                return true;
            case '2':
                if (placeShips(grid)) {
                    return true;
                }
                break;
            case 'q':
            case 'Q':
                return false;
            default:
                break;
        }
    }
}

bool placeShips(char grid[rows][cols]) {
    int shipsCount[4] = {0, 0, 0, 0};

    clearGrid(grid);

    while (true) {
        int shipType;
        bool addShip;
        char action;

        if (!canAddShips(shipsCount)) {
            break;
        }
        clear();
        drawBanner();
        drawGrid(grid);

        cout << "Place ships" << endl;
        for (int i = 0; i < 4; i++) {
            if (canAddShip(shipsCount, i + 1)) {
                cout << i + 1 << " - " << setfill(SHIP_FIELD) << setw(maxShipsCount[i]) << "" << endl;
            }
        }
        cout << "Q - Return to previous menu" << endl;

        action = getch();
        switch (action) {
            case '1':
                shipType = shipsSize[0];
                addShip = canAddShip(shipsCount, shipType);
                break;
            case '2':
                shipType = shipsSize[1];
                addShip = canAddShip(shipsCount, shipType);
                break;
            case '3':
                shipType = shipsSize[2];
                addShip = canAddShip(shipsCount, shipType);
                break;
            case '4':
                shipType = shipsSize[3];
                addShip = canAddShip(shipsCount, shipType);
                break;
            case 'q':
            case 'Q':
                clearGrid(grid);
                return false;
            default:
                shipType = -1;
                addShip = false;
        }
        if (addShip && shipType > 0 && shipType < 5) {
            if (placeShip(shipType, grid)) {
                shipsCount[shipType - 1] += 1;
            }
        }
    }

    return true;
}

bool canAddShips(int *ships) {
    for (int i = 0; i < 4; i++) {
        if (canAddShip(ships, i + 1)) {
            return true;
        }
    }
    return false;
}

bool canAddShip(int *ships, int ship) {
    return ships[ship - 1] < maxShipsCount[ship - 1];
}

bool placeShip(int shipSize, char battlefield[rows][cols]) {
    char battlefieldClone[rows][cols];
    int x = 0, y = 0;
    bool vertical;
    char action;

    findAvailablePlace(shipSize, battlefield, x, y, vertical);
    while (true) {
        copyMatrix(battlefield, battlefieldClone);
        drawShip(x, y, shipSize, battlefieldClone, vertical);

        clear();
        drawBanner();
        cout << "Place ship:" << endl;
        drawGrid(battlefieldClone);
        cout << "W - ↑" << endl;
        cout << "S - ↓" << endl;
        cout << "A - ←" << endl;
        cout << "D - →" << endl;
        cout << "F - Flip" << endl;
        cout << "E - Accept" << endl;
        cout << "Q - Cancel" << endl;

        action = getch();
        switch (action) {
            case 'w':
            case 'W':
                x = (x > 0) ? x - 1 : x;
                break;
            case 's':
            case 'S':
                if (vertical) {
                    x = (x + shipSize < rows) ? x + 1 : x;
                } else {
                    x = (x + 1 < rows) ? x + 1 : x;
                }
                break;
            case 'a':
            case 'A':
                y = (y > 0) ? y - 1 : y;
                break;
            case 'd':
            case 'D':
                if (!vertical) {
                    y = (y + shipSize < cols) ? y + 1 : y;
                } else {
                    y = (y + 1 < cols) ? y + 1 : y;
                }
                break;
            case 'f':
            case 'F':
                vertical = !vertical;
                if (vertical && x + shipSize - 1 >= rows) {
                    x += rows - (x + shipSize);
                }
                if (!vertical && y + shipSize - 1 >= cols) {
                    y += cols - (y + shipSize);
                }
                break;
            case 'e':
            case 'E':
                if (canPlaceShip(shipSize, battlefield, x, y, vertical)) {
                    drawShip(x, y, shipSize, battlefield, vertical);
                    return true;
                }
                break;
            case 'q':
            case 'Q':
                return false;
            default:
                break;
        }
    }
}

bool canPlaceShip(int shipSize, char battlefield[rows][cols], int x, int y, bool vertical) {
    int start_x = x, end_x = x, start_y = y, end_y = y;

    if (x < 0 || y < 0 || x >= rows || y >= cols) {
        return false;
    }

    if (vertical && x + shipSize - 1 >= rows) {
        return false;
    } else if (!vertical && y + shipSize - 1 >= cols) {
        return false;
    }

    if (x > 0) {
        start_x = x - 1;
    }
    if (y > 0) {
        start_y = y - 1;
    }

    if (vertical) {
        if (x + shipSize < rows) {
            end_x = x + shipSize;
        } else {
            end_x = x + shipSize - 1;
        }
        if (y + 1 < cols ) {
            end_y = y + 1;
        }
    } else {
        if (y + shipSize < rows) {
            end_y = y + shipSize;
        } else {
            end_y = y + shipSize - 1;
        }
        if (x + 1 < rows) {
            end_x = x + 1;
        }
    }


    for (int i = start_x; i <= end_x; i++) {
        for (int j = start_y; j <= end_y; j++) {
            if (!isEmpty(battlefield[i][j])) {
                return false;
            }
        }
    }

    return true;
}

int playerFire(char playerGrid[rows][cols], char enemyGrid[rows][cols], int &x, int &y) {
    char action;
    int hit = -1;

    while (hit != 0) {
        hit = -1;
        clear();
        drawBanner();

        if (isShipSunk(x, y, enemyGrid)) {
            cout << "==================================================" << endl;
            cout << "==" << RED << centered(46, "SHIP IS SUNK") << RESET << "==" << endl;
            cout << "==================================================" << endl;
        }
        cout << "Select cell to fire:" << endl;
        showGrids(playerGrid, enemyGrid, x, y);
        cout << "W - ↑" << endl;
        cout << "S - ↓" << endl;
        cout << "A - ←" << endl;
        cout << "D - →" << endl;
        cout << "E - Fire" << endl;
        cout << "F - Mark field as empty" << endl;
        cout << "Q - Return to main menu" << endl;

        action = getch();
        switch (action) {
            case 'w':
            case 'W':
                x = (x > 0) ? x - 1 : x;
                break;
            case 's':
            case 'S':
                x = (x + 1 < rows) ? x + 1 : x;
                break;
            case 'a':
            case 'A':
                y = (y > 0) ? y - 1 : y;
                break;
            case 'd':
            case 'D':
                y = (y + 1 < cols) ? y + 1 : y;
                break;
            case 'e':
            case 'E':
                hit = fire(x, y, enemyGrid);
                break;
            case 'f':
            case 'F':
                markField(x, y, enemyGrid);
                break;
            case 'q':
            case 'Q':
                return -1;
            default:
                break;
        }

        if (hit == 1 && assessGameState(enemyGrid)) {
            return true;
        }
    }

    return false;
}


void generateShips(char battlefield[rows][cols]) {
    clearGrid(battlefield);
    for (int i = 4; i > 0; i--) {
        for (int j = 0; j < 5 - i; j++) {
            bool added = false;
            int tries = 0;
            do {
                int x = rand() % rows;
                int y = rand() % cols;
                bool vertical = rand() % 2 == 1;

                tries++;
                if (canPlaceShip(i, battlefield, x, y, vertical)) {
                    added = true;
                    drawShip(x, y, i, battlefield, vertical);
                }
            } while (!added && tries < 1000);

            if (!added) {
                int x, y;
                bool vertical;
                findAvailablePlace(i, battlefield, x, y, vertical);

                drawShip(x, y, i, battlefield, vertical);
            }
        }
    }
}

void findAvailablePlace(int shipSize, char grid[rows][cols], int &x, int &y, bool &vertical) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (canPlaceShip(shipSize, grid, i, j, true)) {
                x = i;
                y = j;
                vertical = true;
                return;
            } else if (canPlaceShip(shipSize, grid, i, j, false)) {
                x = i;
                y = j;
                vertical = false;
                return;
            }
        }
    }
}

int fire(int x, int y, char grid[rows][cols]) {
    if (canFire(x, y, grid)) {
        if (isHit(x, y, grid)) {
            grid[x][y] = HIT_FIELD;
            return 1;
        } else if (isEmpty(grid[x][y])) {
            grid[x][y] = MISS_FIELD;
            return 0;
        }
    }
    return -1;
}

bool markField(int x, int y, char grid[rows][cols]) {
    if (grid[x][y] == EMPTY_FIELD) {
        grid[x][y] = MARK_FIELD;
        return true;
    } else if (grid[x][y] == SHIP_FIELD) {
        grid[x][y] = MARKED_SHIP_FIELD;
        return true;
    } else if (grid[x][y] == MARK_FIELD) {
        grid[x][y] = EMPTY_FIELD;
        return false;
    } else if (grid[x][y] == MARKED_SHIP_FIELD) {
        grid[x][y] = SHIP_FIELD;
        return false;
    }
    return false;
}

bool isEmpty(char field) {
    return field == EMPTY_FIELD;
}

bool isHit(int x, int y, char battlefied[rows][cols]) {
    return battlefied[x][y] == SHIP_FIELD;
}

bool canFire(int x, int y, char battlefied[rows][cols]) {
    return x >= 0 && y >= 0 && x < rows && y < cols &&
            (battlefied[x][y] == SHIP_FIELD || battlefied[x][y] == EMPTY_FIELD);
}

bool isShipSunk(int x, int y, char grid[rows][cols]) {
    char field = grid[x][y];
    int start_x = x;
    int start_y = y;

    if (field != HIT_FIELD) {
        return false;
    }

    while (field == HIT_FIELD && x > 0) {
        field = grid[--x][y];
        if (field == SHIP_FIELD) {
            return false;
        }
    }

    x = start_x;
    field = HIT_FIELD;
    while (field == HIT_FIELD && x + 1 < rows) {
        field = grid[++x][y];
        if (field == SHIP_FIELD) {
            return false;
        }
    }

    x = start_x;
    field = HIT_FIELD;
    while (field == HIT_FIELD && y > 0) {
        field = grid[x][--y];
        if (field == SHIP_FIELD) {
            return false;
        }
    }

    y = start_y;
    field = HIT_FIELD;
    while (field == HIT_FIELD && y + 1 < cols) {
        field = grid[x][++y];
        if (field == SHIP_FIELD) {
            return false;
        }
    }

    return true;
}

bool assessGameState(char grid[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == SHIP_FIELD || grid[i][j] == MARKED_SHIP_FIELD) {
                return false;
            }
        }
    }
    return true;
}

//void debugAiFire(char grid[rows][cols], int &x, int &y, int directions[4]) {
//    int tempX = x, tempY = y;
//    char dgrid[rows][cols];
//    copyMatrix(grid, dgrid);
//
//    if (x < 0 && y < 0) {
//        determineNextShot(dgrid, x, y, directions);
//        tempX = x;
//        tempY = y;
//    }
//
//    determineNextShot(dgrid, tempX, tempY, directions);
//    drawGrid(grid, true, tempX, tempY);
//}

void resetAiData(char grid[rows][cols], int &x, int &y, int &lastX, int &lastY, int directions[4], int &direction) {
    while (!canAiFire(x, y, grid)) {
        x = rand() % rows;
        y = rand() % cols;
    }
    for (int i = 0; i < 4; ++i) {
        directions[i] = 0;
    }
    direction = -1;
    lastX = -1;
    lastY = -1;
}


//bool aiFire(char grid[rows][cols], char grid1[rows][cols], int &x, int &y, int &lastX, int &lastY, int directions[4]) {
//    bool hit;
//    bool isSunk;
//    bool gameOver = false;
//    int direction;
//
//    do {
//        clear();
//        drawBanner();
//
//        hit = fire(x, y, grid) == 1;
//
//        showGrids(grid, grid1, x, y, true);
//        getch();
//
//        isSunk = hit && isShipSunk(x, y, grid);
//
//        if (hit && !isSunk) {
//            directions[direction] = 1;
//            if (aiLastX < 0) {
//                aiLastX = x;
//                aiLastY = y;
//            }
//
//            x = lastX;
//            y = lastY;
//
//            bool shotDetected;
//            do {
//
//                direction = determineMoveDirection(directions);
//                shotDetected = determineNextShot(grid, x, y, direction);
//                if (!shotDetected) {
//                    directions[direction] = directions[direction] == 1 ? 2 : -1;
//                }
//            } while (!shotDetected);
//        } else if (isSunk || lastX < 0) {
//            if (isSunk && assessGameState(grid)) {
//                gameOver = true;
//            } else {
//                resetAiData(grid, x, y, lastX, lastY, directions);
//            }
//        } else if ( !hit && lastX > 0 && lastY > 0) {
//            directions[direction] = directions[direction] == 1 ? 2 : -1;
//        }
//    } while (hit && !gameOver);
//
//    return hit && gameOver;
//}

int determineMoveDirection(int directions[4]) {
    for (int i = 0; i < 4; ++i) {
        if (directions[i] == 1) {
            return i;
        }
    }

    int axis = -1;

    for (int i = 0; i < 2; ++i) {
        if (directions[i*2] == -1 && directions[i*2+1] == -1) {
            axis = !i;
            break;
        } else if (directions[i*2] == 2 || directions[i*2 + 1] == 2) {
            axis = i;
            break;
        }
    }

    if (axis == -1) {
        axis = rand() % 2;
    }

    int randDir = rand() % 2;
    int directionState = directions[axis * 2 + randDir];

    return axis * 2 + (directionState == 0 == randDir);
}

bool canStepInto(char grid[rows][cols], int x, int y) {
    return x >= 0 && y >= 0 && x < rows && y < cols && grid[x][y] != MISS_FIELD;
}

bool determineNextShot(char grid[rows][cols], int &x, int &y, int direction) {
    int nextX = x;
    int nextY = y;

    int axis = direction < 2 ? 0 : 1;
    direction = direction % 2 == 0 ? -1 : 1;

    while (canStepInto(grid, nextX, nextY)) {
        if (canAiFire(nextX, nextY, grid)) {
            x = nextX;
            y = nextY;
            return true;
        }
        if (!axis) {
            nextX += direction;
        } else {
            nextY += direction;
        }
    }

    return false;
}

bool canAiFire(int x, int y, char battlefied[rows][cols]) {
    if (!canFire(x, y, battlefied)) {
        return false;
    }

    int start_x = x, end_x = x, start_y = y, end_y = y;
    if (x > 0) {
        start_x = x - 1;
    }
    if (x < rows - 1) {
        end_x = x + 1;
    }
    if (y > 0) {
        start_y = y - 1;
    }
    if (y < cols - 1) {
        end_y = y + 1;
    }

    for (int i = start_x; i <= end_x; ++i) {
        for (int j = start_y; j <= end_y; ++j) {
            if (battlefied[i][j] == HIT_FIELD && isShipSunk(i, j, battlefied)) {
                return false;
            }
        }
    }
    return true;
}

void drawBanner() {
    cout << RED << "***************************************************************************************************"
         << RESET << "\n";
    cout << CYAN
         << ".______       ___   .___________.___________. __       _______     _______. __    __   __  .______ \n";
    cout << "|   _  \\     /   \\  |           |           ||  |     |   ____|   /       ||  |  |  | |  | |   _  \\\n";
    cout << "|  |_)  |   /  ^  \\ `---|  |----`---|  |----`|  |     |  |__     |   (----`|  |__|  | |  | |  |_)  |\n";
    cout << "|   _  <   /  /_\\  \\    |  |        |  |     |  |     |   __|     \\   \\    |   __   | |  | |   ___/\n";
    cout << "|  |_)  | /  _____  \\   |  |        |  |     |  `----.|  |____.----)   |   |  |  |  | |  | |  |    \n";
    cout << "|______/ /__/     \\__\\  |__|        |__|     |_______||_______|_______/    |__|  |__| |__| | _|    \n";
    cout << RESET << "\n";
    cout << RED
         << "****************************************************************************************************"
         << RESET << "\n";
    cout << CYAN
         << "°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸°º¤ø,¸¸,ø¤º°`°º¤ø\n";
    cout << BLUE
         << "°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸°º¤ø,¸¸,ø¤º°`°º¤ø\n"
         << RESET;
}

void showMenu(string title, string *items, int countItems) {
    cout << endl;
    cout << setw(100) << setfill('*') << "" << endl;
    cout << setw(100) << setfill('*') << "" << endl;
    cout << "**" << setw(96) << setfill(' ') << "" << "**" << endl;
    cout << "**" << centered(96, title) << "**" << endl;

    cout << "**" << setw(96) << setfill(' ') << "" << "**" << endl;

    for (int i = 0; i < countItems; i++) {
        cout << "**" << centered(96, items[i]) << "**\n";
    }

    cout << "**" << setw(96) << setfill(' ') << "" << "**" << endl;
    cout << setw(100) << setfill('*') << "" << endl;
    cout << setw(100) << setfill('*') << "" << endl;
}

void drawGrid(char grid[rows][cols], bool own, int mark_x, int mark_y) {
    cout << "   ";
    for (char i = 'A'; i < 'A' + rows; i++) {
        cout << i << " ";
    }
    cout << endl;
    for (int i = 0; i < rows; i++) {
        cout << setfill(' ') << setw(3) << left << i + 1;
        for (int j = 0; j < cols; j++) {
            drawField(grid[i][j], own, mark_x == i && mark_y == j);
        }
        cout << endl;
    }
}


void showGrids(char ownGrid[rows][cols], char enemyGrid[rows][cols], int mark_x, int mark_y, bool markEnemy) {
    cout << "   ";
    for (char i = 'A'; i < 'A' + rows; i++) {
        cout << i << " ";
    }
    cout << "    ";
    for (char i = 'A'; i < 'A' + rows; i++) {
        cout << i << " ";
    }
    cout << endl;
    for (int i = 0; i < rows; i++) {
        cout << setfill(' ') << setw(3) << left << i + 1;
        for (int j = 0; j < cols; j++) {
            drawField(ownGrid[i][j], true, markEnemy && i == mark_x && j == mark_y);
        }

        cout << " ";

        cout << setfill(' ') << setw(3) << left << i + 1;
        for (int j = 0; j < cols; j++) {
            drawField(enemyGrid[i][j], false, !markEnemy && i == mark_x && j == mark_y);
        }
        cout << endl;
    }
}

void drawField(char field, bool own = true, bool isCursor) {
    if (!own) {
        if (field == SHIP_FIELD) {
            field = EMPTY_FIELD;
        } else if (field == MARKED_SHIP_FIELD) {
            field = MARK_FIELD;
        }
    }

    if (isCursor) {
        if (!own) {
            if (field == HIT_FIELD || field == MISS_FIELD || field == MARK_FIELD) {
                cout << RED;
            } else {
                cout << GREEN;
            }
        } else {
            cout << MAGENTA;
        }
    } else {
        if (field == HIT_FIELD) {
            cout << (own ? RED : GREEN);
        } else if (field == MISS_FIELD) {
            cout << CYAN;
        } else if (field == EMPTY_FIELD) {
            cout << BLUE;
        } else if (field == MARK_FIELD || field == MARKED_SHIP_FIELD) {
            cout << YELLOW;
        }
    }

    cout << field << RESET << ' ';
}

void drawShip(int x, int y, int shipSize, char grid[rows][cols], bool vertical = true) {
    for (int i = 0; i < shipSize; i++) {
        if (vertical) {
            grid[x + i][y] = SHIP_FIELD;
        } else {
            grid[x][y + i] = SHIP_FIELD;
        }
    }
}

void playerWon(char grid1[rows][cols], char grid2[rows][cols]) {
    clear();
    drawBanner();
    showGrids(grid1, grid2);
    cout << endl;
    cout << "YOU WON!";
    cout << endl;
    getch();
}

void playerLose(char grid1[rows][cols], char grid2[rows][cols]) {
    clear();
    drawBanner();
    showGrids(grid1, grid2);
    cout << endl;
    cout << "YOU LOSE!";
    cout << endl;
    getch();
}

//void showCollisions(int shipSize, int x, int y, bool vertical, char battlefield[rows][cols]) {
//    cout << "   ";
//    for (char i = 'A'; i < 'A' + rows; i++) {
//        cout << i << " ";
//    }
//    cout << endl;
//    for (int i = 0; i < rows; i++) {
//        cout << setfill(' ') << setw(3) << left << i + 1;
//        for (int j = 0; j < cols; j++) {
//            cout << battlefield[i][j] << " ";
//        }
//        cout << endl;
//    }
//}

void clearGrid(char battlefield[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            battlefield[i][j] = EMPTY_FIELD;
        }
    }
}

void copyMatrix(char src[][cols], char dest[][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

string centered(int width, const string &str) {
    unsigned long len = str.length();
    if (width < len) { return str; }

    unsigned long diff = width - len;
    unsigned long pad1 = diff / 2;
    unsigned long pad2 = diff - pad1;
    return string(pad1, ' ') + str + string(pad2, ' ');
}