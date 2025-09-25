
#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <numeric> // needed for accumulate in compute_score
#include <random>

using namespace std;
const int ROW_LENGTH = 4;
const int COL_LENGTH = 4;
void collectEmptyCells(vector<pair<int,int>>& emptyCells, const vector<vector<int>>& board);
void compress(vector<int>& compressed, const vector<int>& row);
void merge(vector<int>& compressed);

void write_board_csv(const vector<vector<int>>& board, bool first) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void print_board(const vector<vector<int>>& board, bool first) {
    for (const auto &row : board) {
        for (auto val : row) {
            if (val == 0) cout << ".\t";
            else cout << val << "\t";
        }
        cout << "\n";
    }
    cout << endl;
    write_board_csv(board, first);
}

// TODO: use algorithms to spawn properly
void spawn_tile(vector<vector<int>>& board) {
    vector<pair<int,int>> emptyCells;
    collectEmptyCells(emptyCells, board);
    // AI Declaration
    // I used AI to use the random number generation
    // (This was additionally explicitly permitted in the lab spec)

    // Generate random seed
    std::random_device rd;
    std::mt19937 gen(rd());

    // End of AI Declaration

    // Select from list
    std::uniform_int_distribution<int> dist(0, emptyCells.size()-1);


    int index = dist(gen);

    int row = emptyCells[index].first;
    int col = emptyCells[index].second;

    // AI Declaration
    // Used AI to figure out how to do a coinflip, it suggested using bernoli
    std::bernoulli_distribution coin_flip(0.9);
    int val;
    if (coin_flip(gen)) {
        // End of AI Declaration
        val = 2;
    } else {
        val = 4;
    }

    board[row][col] = val;
}
// Merges a row or column
void merge(vector<int>& compressed) {
    for (int i = 0; i < ROW_LENGTH - 1; i++) {
        //cout << compressed[i] << "vs" << compressed[i + 1] << "\n";
        if (compressed[i] == compressed[i + 1]) {
            compressed[i] = compressed[i]*2;
            compressed[i+1]=0;
            i++;
        }
    }
}
// Compresses a row or column
void compress(vector<int>& compressed, const vector<int>& row) {
    copy_if(row.begin(), row.end(), back_inserter(compressed), [](int i){return i!=0;});
    compressed.resize(ROW_LENGTH, 0);
}

void collectEmptyCells(vector<pair<int,int>>& emptyCells, const vector<vector<int>>& board) {
    for (int r=0; r< ROW_LENGTH; r++) {
        for (int c=0; c< COL_LENGTH; c++) {
            if (board[r][c] == 0) {
                emptyCells.push_back(make_pair(r,c));
            }
        }
    }
}

void compress_row(vector<int>& compressed, const vector<int>& row, bool reverseCompress) {
    vector<int> temp = row;
    if (reverseCompress) {
        reverse(temp.begin(), temp.end());
    }

    compress(compressed, temp);
    merge(compressed);
    compress(compressed, compressed);

    if (reverseCompress) {
        reverse(compressed.begin(), compressed.end());
    }
}

bool move_left(vector<vector<int>>& board) {
    bool modified = false;

    for (auto &row: board) {
        std::vector<int> compressed;
        vector<int> prev_row = row;

        compress_row(compressed, row, false);

        if (compressed != prev_row) {
            modified = true;
            row.assign(compressed.begin(), compressed.end());
        }

    }
    return true;
}
bool move_right(vector<vector<int>>& board){
    bool modified = false;
    for (auto &row: board) {
        vector<int> prev_row = row;

        std::vector<int> compressed;
        compress_row(compressed, row, true);

        if (compressed != prev_row) {
            modified = true;
            row.assign(compressed.begin(), compressed.end());
        }
    }

    return modified;
}


bool move_column(vector<vector<int>>& board, bool reverseCompress) {
    bool modified = false;
    for (int j=0; j<COL_LENGTH; j++) {
        std::vector<int> column;
        for (int i=0; i<ROW_LENGTH; i++) {
            column.push_back(board[i][j]);
        }
        vector<int> prev_column = column;

        std::vector<int> compressed;
        compress_row(compressed, column, reverseCompress);
        if (compressed != prev_column) {
            modified = true;
            for (int i=0; i<ROW_LENGTH; i++) {
                board[i][j] = compressed[i];
            }
        }
    }
    return modified;
}

bool move_up(vector<vector<int>>& board){
    return move_column(board,false);;
}
bool move_down(vector<vector<int>>& board){
    return move_column(board,true);
;}

int compute_score(const vector<vector<int>>& board) {
    int score = 0;
    for (int r=0; r<ROW_LENGTH; r++) {
        for (int c=0; c<COL_LENGTH; c++) {
            score += board[r][c];
        }
    }
    return score;
}

int main(){
    srand(time(nullptr));
    vector<vector<int>> board(4, vector<int>(4,0));
    spawn_tile(board);
    spawn_tile(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board, first);
        first=false;
        cout << "Score: " << compute_score(board) << "\n";
        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char cmd;
        if (!(cin>>cmd)) break;
        if (cmd=='q') break;

        if (cmd=='u') {
            if (!history.empty()) {
                board = history.top();
                history.pop();
            }
            //print_board(board,false);
            continue;
        }

        vector<vector<int>> prev = board;
        bool moved=false;
        if (cmd=='a') moved=move_left(board);
        else if (cmd=='d') moved=move_right(board);
        else if (cmd=='w') moved=move_up(board);
        else if (cmd=='s') moved=move_down(board);

        if (moved) {
            history.push(prev);
            spawn_tile(board);
        }
    }
    return 0;
}
