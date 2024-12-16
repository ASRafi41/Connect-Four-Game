#include<bits/stdc++.h>
using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
auto my_rand(long long l, long long r) {
    return uniform_int_distribution<long long>(l, r)(rng);
}

const int Rows = 6, Cols = 7, WinSz = 4, Points = 10000;
const char You = 'O', AI = 'X';

void printBoard(const vector<vector<char>> &board) {
    for(int r = 0; r < Rows; r++) {
        for(int c = 0; c < Cols; c++) {
            cout << board[r][c] << " \n"[c + 1 == Cols];
        }
    }
    cout << "1 2 3 4 5 6 7\n"; // Rows indexing
}

bool isValidMove(const vector<vector<char>> &board, int col) {
    return col >= 0 && col < Cols && board[0][col] == '.';
}

int makeMove(vector<vector<char>> &board, int col, char player) {
    for(int r = Rows - 1; r >= 0; r--) {
        if(board[r][col] == '.') {
            board[r][col] = player;
            return r;
        }
    }
    return -1; // Invalid case
}

void undoMove(vector<vector<char>> &board, int row, int col) {
    if(row >= 0 && row < Rows) board[row][col] = '.';
}

bool isBoardFull(const vector<vector<char>> &board) {
    for(int c = 0; c < Cols; c++) {
        if(board[0][c] == '.') return false;
    }
    return true;
}

bool isWinningMove(const vector<vector<char>> &board, int row, int col, char ch) {
    auto isValid = [&](int r, int c) {
        return r >= 0 && r < Rows && c >= 0 && c < Cols && board[r][c] == ch;
    };

    // Directions=> vertical, horizontal, diagonals
    vector<pair<int, int>> directions = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    
    for(auto &[dr, dc] : directions) {
        int cnt = 1;
        // Check forward direction
        for(int i = 1; i < WinSz && cnt < WinSz; i++) {
            if (isValid(row + i * dr, col + i * dc)) cnt++;
            else break;
        }
        // Check backward direction
        for(int i = 1; i < WinSz && cnt < WinSz; i++) {
            if (isValid(row - i * dr, col - i * dc)) cnt++;
            else break;
        }
        if(cnt >= WinSz) return true;
    }
    return false;
}

int findWinningMove(const vector<vector<char>> &board) { // +Points for AI win, -Points for Human win, 0 otherwise
    for(int r = 0; r < Rows; r++) {
        for(int c = 0; c < Cols; c++) {
            if(board[r][c] != '.' && isWinningMove(board, r, c, board[r][c])) {
                return board[r][c] == AI ? +Points : -Points;
            }
        }
    }
    return 0;
}

int minimax(vector<vector<char>> &board, int depth, int alpha, int beta, bool isMaximizingPlayer) {
    int score = findWinningMove(board);
    if(abs(score) == Points or depth == 0 or isBoardFull(board)) return score;
    if(isMaximizingPlayer) {
        int mx = INT_MIN;
        for(int c = 0; c < Cols; c++) {
            if(isValidMove(board, c)) {
                int r = makeMove(board, c, AI);
                int x = minimax(board, depth - 1, alpha, beta, 0);
                undoMove(board, r, c);
                mx = max(mx, x);
                alpha = max(alpha, x);
                if(beta <= alpha) break;
            }
        }
        return mx;
    }
    else {
        int mn = INT_MAX;
        for(int c = 0; c < Cols; c++) {
            if(isValidMove(board, c)) {
                int r = makeMove(board, c, You);
                int x = minimax(board, depth - 1, alpha, beta, 1);
                undoMove(board, r, c);
                mn = min(mn, x);
                beta = min(beta, mn);
                if(beta <= alpha) break;
            }
        }
        return mn;
    }
    assert(0); // Invalid case
}

int findBestMove(vector<vector<char>> &board, int depth) {
    map<int, vector<int>> scores;
    for(int c = 0; c < Cols; c++) {
        if(isValidMove(board, c)) {
            // cout << c << " ";
            int r = makeMove(board, c, AI);
            int x = minimax(board, depth - 1, INT_MIN, INT_MAX, 0);
            undoMove(board, r, c);
            if(isWinningMove(board, r, c, AI)) return c;
            scores[x].push_back(c);
        }
    }
    auto &bestMoves = prev(scores.end())->second;
    // for(auto &i: bestMoves) cout << i << " "; cout << endl;
    return bestMoves[my_rand(0, int(bestMoves.size()) - 1)];
}

void SetColor(int textColor) { // Text Color
    cout << "\033[" << textColor << "m";
}
void ResetColor() { 
    cout << "\033[0m"; 
}
void printAns(const vector<vector<char>> &board, int color) {
    for(int r = 0; r < Rows; r++) {
        for(int c = 0; c < Cols; c++) {
            if(board[r][c] != '.' && isWinningMove(board, r, c, board[r][c])) {
                SetColor(color);
                cout << board[r][c] << " ";
                ResetColor();
            }
            else {
                cout << board[r][c] << " ";
            }
        }
        cout << endl;
    }
}

int main() {
    vector<vector<char>> board(Rows, vector<char>(Cols, '.'));
    bool isPlayerTurn = my_rand(0, 1);
    cout << "--- Welcome to Connect Four! ---\n";
    cout << (isPlayerTurn ? "Your turn first!\n" : "AI starts first!\n");

    while(true) {
        printBoard(board);
        if(isPlayerTurn) {
            int col;
            cout << "Enter your move (1-7): ";
            cin >> col;
            if(col < 1 or col > Cols or !isValidMove(board, col - 1)) {
                cout << "Invalid move. Try again.\n";
                continue;
            }
            int row = makeMove(board, col - 1, You);
            if(isWinningMove(board, row, col - 1, You)) {
                // printBoard(board);
                cout << endl;
                printAns(board, 32);

                SetColor(35);
                cout << "==> Congratulations, You Win! <==\n\n";
                ResetColor();
                break;
            }
            else cout << endl;
        } 
        else {
            cout << "AI is making its move...\n";
            int col = findBestMove(board, 6);
            int row = makeMove(board, col, AI);
            cout << "AI chose column: " << col + 1 << "\n";
            if(isWinningMove(board, row, col, AI)) {
                // printBoard(board);
                cout << endl;
                printAns(board, 31);

                SetColor(35);
                cout << "==> AI Wins! Better luck next time. <==\n\n";
                ResetColor();
                break;
            }
            else cout << endl;
        }
        if(isBoardFull(board)) {
            printBoard(board);
            cout << "It's a draw!\n";
            break;
        }
        isPlayerTurn ^= 1;
    }
    return 0;
}
