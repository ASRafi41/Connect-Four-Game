#include<bits/stdc++.h>
using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count()); // mt19937_64 (long long)
auto my_rand(long long l, long long r) {
    return uniform_int_distribution<long long>(l, r)(rng);
}

const int rows = 6, cols = 7, winSz = 4, points = 10000;
const char You = 'O', AI = 'X';

void print(const vector<vector<char>> &board) {
    for(int r = 0; r < rows; r++) {
        for(int c = 0; c < cols; c++) {
            cout << board[r][c] << " \n"[c + 1 == cols];
        }
    }
    cout << "1 2 3 4 5 6 7\n"; // rows indexing
}

bool isValidMove(const vector<vector<char>> &board, int col) {
    return col >= 0 && col < cols && board[0][col] == '.';
}

int makeMove(vector<vector<char>> &board, int col, char player) {
    for(int r = rows - 1; r >= 0; r--) {
        if(board[r][col] == '.') {
            board[r][col] = player;
            return r;
        }
    }
    return -1; // Invalid case
}

void undoMove(vector<vector<char>> &board, int row, int col) {
    if(row >= 0 && row < rows) board[row][col] = '.';
}

bool isBoardFull(const vector<vector<char>> &board) {
    for(int c = 0; c < cols; c++) {
        if(board[0][c] == '.') return 0;
    }
    return 1;
}

bool isWinningMove(const vector<vector<char>> &board, int row, int col, char ch) {
    int cnt1, cnt2, r, c; 
    // Vertical
    cnt1 = 0, r = row + 1;
    while(r < rows && board[r][col] == ch && cnt1 + 1 < winSz) ++r, ++cnt1;
    if(cnt1 + 1 >= winSz) return 1;

    // Down Left Corner
    cnt1 = 0, cnt2 = 0, r = row + 1, c = col - 1;
    while(r < rows && c >= 0 && board[r][c] == ch && cnt1 + 1 < winSz) ++r, --c, ++cnt1;
    r = row - 1, c = col + 1;
    while(r >= 0 && c < cols && board[r][c] == ch && cnt1 + cnt2 + 1 < winSz) --r, ++c, ++cnt2;
    if(cnt1 + cnt2 + 1 >= winSz) return 1;

    // Down Right Corner
    cnt1 = 0, cnt2 = 0, r = row - 1, c = col - 1;
    while(r >= 0 && c >= 0 && board[r][c] == ch && cnt1 + 1 < winSz) --r, --c, ++cnt1;
    r = row + 1, c = col + 1;
    while(r < rows && c < cols && board[r][c] == ch && cnt1 + cnt2 + 1 < winSz) ++r, ++c, ++cnt2;
    if(cnt1 + cnt2 + 1 >= winSz) return 1;
    
    // Horizontal
    cnt1 = 0, cnt2 = 0, c = col;
    while(--c >= 0 && ch == board[row][c] && cnt1 + 1 < winSz) ++cnt1; // Left
    c = col;
    while(++c < cols && ch == board[row][c] && cnt1 + cnt2 + 1 < winSz) ++cnt2; // Right
    if(cnt1 + cnt2 + 1 >= winSz) return 1;

    return 0;
}

int findWinningMove(const vector<vector<char>> &board) { // +points for AI win, -points for Human win, 0 otherwise
    for(int r = 0; r < rows; r++) {
        for(int c = 0; c < cols; c++) {
            if(board[r][c] != '.' && isWinningMove(board, r, c, board[r][c])) {
                return board[r][c] == AI ? +points : -points;
            }
        }
    }
    return 0;
}

int minimax(vector<vector<char>> &board, int depth, int alpha, int beta, bool isMaximizingPlayer) {
    int point = findWinningMove(board);
    if(abs(point) == points or depth == 0 or isBoardFull(board)) return point;
    if(isMaximizingPlayer) {
        int mx = INT_MIN;
        for(int c = 0; c < cols; c++) {
            if(isValidMove(board, c)) {
                int r = makeMove(board, c, AI);
                int x = minimax(board, depth - 1, alpha, beta, 0);
                undoMove(board, r, c);
                mx = max(mx, x);
                alpha = max(alpha, x);
                if(alpha >= beta) return mx;
            }
        }
        return mx;
    }
    else {
        int mn = INT_MAX;
        for(int c = 0; c < cols; c++) {
            if(isValidMove(board, c)) {
                int r = makeMove(board, c, You);
                int x = minimax(board, depth - 1, alpha, beta, 1);
                undoMove(board, r, c);
                mn = min(mn, x);
                beta = min(beta, mn);
                if(alpha >= beta) return mn;
            }
        }
        return mn;
    }
    assert(0); // Invalid case
}

int findBestMove(vector<vector<char>> &board, int depth) {
    map<int, vector<int>> mp;
    for(int c = 0; c < cols; c++) {
        // cout << c << " ";
        if(isValidMove(board, c)) {
            int r = makeMove(board, c, AI);
            int x = minimax(board, depth - 1, INT_MIN, INT_MAX, 0);
            undoMove(board, r, c);
            mp[x].push_back(c);
        }
    }
    auto &vec = prev(mp.end())->second;
    // for(auto &i: vec) cout << i << " "; cout << endl;
    return vec[my_rand(0, int(vec.size()) - 1)];
}

int main() {
    vector<vector<char>> board(rows, vector<char> (cols, '.'));
    bool turn = my_rand(0, 1); // 0 => Your Turn, 1 => AI Turn
    cout << "--- Welcome to Connect Four! ---\n";
    if(turn == 0) {
        cout << "=> First turn is Yours\n";
        print(board);
    }
    else cout << "First turn is AI\n";

    int row, col;
    while(true) {
        if(turn == 0) { // Your turn
            cout << "Enter your move (1-7): ";
            cin >> col; --col; // 0'base
            
            if(!isValidMove(board, col)) {
                cout << "Invalid move. Try again. \2\n";
                continue;
            }

            row = makeMove(board, col, You);
            // cout << "=> " << row << " " << col << " = " << isWinningMove(board, row, col, You) << "\n";//<====
            if(isWinningMove(board, row, col, You)) {
                print(board);
                cout << "==> Congratulations, You Win! <==\n";
                return 0;
            }
        }
        else { // AI
            cout << "AI is making its move...\n";
            col = findBestMove(board, 6);
            // cout << "=> " << col << "\n";
            row = makeMove(board, col, AI);
            cout << "AI chose column: " << col + 1 << "\n";
            if(isWinningMove(board, row, col, AI)) {
                print(board);
                cout << "==> Bad Luck, AI Win! <==\n";
                return 0;
            }
        }
        cout << "\n";
        print(board);
        // cout << "\n";
        if(isBoardFull(board)) {
            cout << "It's a draw!\n";
            return 0;
        }
        turn ^= 1;
    }
    return 0;
}