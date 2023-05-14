#include <stdio.h>

void drawBoard(char board[3][3]) {
    printf("    0   1   2  \n");
    printf("  -------------\n");
    for (int i = 0; i < 3; i++) {
        printf("%i | %c | %c | %c |\n", i, board[i][0], board[i][1], board[i][2]);
        printf("  -------------\n");
    }
}


// 2 -> no winner yet
// 1 -> computer won (X)
// -1 -> player won (O)
// 0 -> it's a draw
int checkWin(char board[3][3]) {
    // rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != ' ' && board[i][1] == board[i][0] && board[i][2] == board[i][0])
            return (board[i][0] == 'X') ? 1 : -1;
    }

    // columns
    for (int i = 0; i < 3; i++) {
        if (board[0][i] != ' ' && board[1][i] == board[0][i] && board[2][i] == board[0][i])
            return (board[0][i] == 'X') ? 1 : -1;
    }

    // diagonals
    if (board[0][0] != ' ' && board[1][1] == board[0][0] && board[2][2] == board[0][0])
        return (board[0][0] == 'X') ? 1 : -1;

    if (board[0][2] != ' ' && board[1][1] == board[0][2] && board[2][0] == board[0][2])
        return (board[0][2] == 'X') ? 1 : -1;
    
    // draw
    int isDraw = 1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                isDraw = 0;
                break;
            }
        }
        if (!isDraw)
            break;
    }
    if (isDraw)
        return 0;

    return 2;
}

int minimax(char board[3][3], int n_moves, char player) {
    int score = checkWin(board);

    if (score != 2) { // the game is finished
        return score*10 - score*n_moves;
    }

    int bestScore;

    if (player == 'X') { // we want to maximize the score
        bestScore = -20;

        for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
                if (board[i][j] != ' ')
                    continue;
                board[i][j] = 'X';
                score = minimax(board, n_moves + 1, 'O');
                board[i][j] = ' ';
                if (score > bestScore)
                    bestScore = score;
            }
        }
    } else { // we want to minimize the score
        bestScore = 20;

        for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
                if (board[i][j] != ' ')
                    continue;
                board[i][j] = 'O';
                score = minimax(board, n_moves + 1, 'X');
                board[i][j] = ' ';
                if (score < bestScore)
                    bestScore = score;
            }
        }
    }
    return bestScore;
}

void computerMove(char board[3][3]) {
    int score;
    int bestScore = -20;
    int bestRow = -1;
    int bestCol = -1;

    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = 'X';
                score = minimax(board, 0, 'O');
                if (score > bestScore) {
                    bestRow = i;
                    bestCol = j;
                    bestScore = score;
                }
                board[i][j] = ' '; // reset the board for next score computation
            }
        }
    }
    board[bestRow][bestCol] = 'X';
}

void playGame() {
    char board[3][3] = { { ' ', ' ', ' ' }, { ' ', ' ', ' ' }, { ' ', ' ', ' ' } };
    int row, col, win, scan;
    char currentPlayer = 'X';

    printf("Tic-Tac-Toe Game\n");
    printf("Player  : X\n");
    printf("Computer: O\n");
    printf("Enter the position to place your symbol (row column).\n");

    while (1) {
        drawBoard(board);

        if (currentPlayer == 'O') {
            printf("Your turn: ");
            scan = scanf("%d %d", &row, &col);
            printf("%d %d", row, col);
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF) {} // empty the buffer

            // Check if the move is valid
            if (row < 0 || row > 2 || col < 0 || col > 2 || board[row][col] != ' ') {
                printf("Invalid move! Try again.\n");
                continue;
            }
            // Place the symbol on the board
            board[row][col] = currentPlayer;

        } else {
            computerMove(board);
        }

        
        // Check for a win
        win = checkWin(board);
        if (win == 1) {
            drawBoard(board);
            printf("Computer wins!\n");
            break;
        } else if (win == -1) {
            drawBoard(board);
            printf("You win!\n");
            break;
        } else if (win == 0) {
            drawBoard(board);
            printf("It's a draw!\n");
            break;
        }

        // Switch players
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }
}

int main() {
    playGame();
    return 0;
}
