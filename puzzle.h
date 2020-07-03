#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

struct Puzzle
{
    int col;
    int row;
    vector<int> ships;
    vector<int> rowsums;
    vector<int> colsums;
};

struct Ship
{
    int length;
    int row;
    int col;
    int ori;

    int hash()
    {
        return ori + col * 1e2 + row * 1e4 + length * 1e6;
    }
};

struct State
{
    Puzzle *puzzle;
    vector<Ship> ships;
    vector<vector<int>> grid;
    vector<int> rowsums;
    vector<int> colsums;

    State() {}

    State(Puzzle &puzzle) : puzzle(&puzzle),
                                  rowsums(vector<int>(puzzle.row, 0)),
                                  colsums(vector<int>(puzzle.col, 0)),
                                  grid(puzzle.row, vector<int>(puzzle.col, 0)) {}

    State(const State &state) : puzzle(state.puzzle), ships(state.ships), rowsums(state.rowsums), colsums(state.colsums), grid(state.grid) {}


    bool is_hash_legal(Ship &ship)
    {
        if (ships.size() > 0)
            return ships.back().hash() > ship.hash();
        return true;
    }

    bool is_row_legal(Ship &ship)
    {
        // the ship is horizontal or not exceed max row
        return ship.ori == 0 ||
               (ship.row + ship.length <= puzzle->row);
    }

    bool is_col_legal(Ship &ship)
    {
        // the ship is vertical or not exceed max col
        return ship.ori == 1 ||
               (ship.col + ship.length <= puzzle->col);
    }

    bool is_rowsums_legal(Ship &ship)
    {
        if (ship.ori == 0)
            return rowsums[ship.row] + ship.length <= puzzle->rowsums[ship.row];
        else
        {
            for (int row = ship.row; row < ship.row + ship.length; row++)
                if (rowsums[row] + 1 > puzzle->rowsums[row])
                    return false;
            return true;
        }
    }
    bool is_colsums_legal(Ship &ship)
    {
        if (ship.ori == 1)
            return colsums[ship.col] + ship.length <= puzzle->colsums[ship.col];
        else
        {
            for (int col = ship.col; col < ship.col + ship.length; col++)
                if (colsums[col] + 1 > puzzle->colsums[col])
                    return false;
            return true;
        }
    }

    bool is_grid_legal(Ship &ship)
    {
        if (ship.ori == 1)
        {
            for (int row = max(ship.row - 1, 0); row < min(ship.row + ship.length + 1, puzzle->row); row++)
            {
                for (int col = max(ship.col - 1, 0); col < min(ship.col + 2, puzzle->col); col++)
                {
                    if (grid[row][col] == 1)
                        return false;
                }
            }
            return true;
        }
        else
        {
            for (int row = max(ship.row - 1, 0); row < min(ship.row + 2, puzzle->row); row++)
            {
                for (int col = max(ship.col - 1, 0); col < min(ship.col + ship.length + 1, puzzle->col); col++)
                {
                    if (grid[row][col] == 1)
                        return false;
                }
            }
            return true;
        }
    }

    bool is_legal(Ship &ship)
    {
        if (!is_hash_legal(ship) || !is_row_legal(ship) || !is_col_legal(ship))
            return false;
        if (!is_rowsums_legal(ship) || !is_colsums_legal(ship))
            return false;
        if (!is_grid_legal(ship))
            return false;
        return true;
    }

    void push(Ship &ship)
    {
        ships.push_back(ship);

        if (ship.ori == 0)
        {
            rowsums[ship.row] += ship.length;
            for (int col = ship.col; col < ship.col + ship.length; col++)
            {
                colsums[col] += 1;
                grid[ship.row][col] = 1;
            }
        }
        else
        {
            colsums[ship.col] += ship.length;
            for (int row = ship.row; row < ship.row + ship.length; row++)
            {
                rowsums[row] += 1;
                grid[row][ship.col] = 1;
            }
        }
    }

    Ship pop()
    {
        Ship ship = ships.back();
        ships.pop_back();

        if (ship.ori == 0)
        {
            rowsums[ship.row] -= ship.length;
            for (int col = ship.col; col < ship.col + ship.length; col++)
            {
                colsums[col] -= 1;
                grid[ship.row][col] = 0;
            }
        }
        else
        {
            colsums[ship.col] -= ship.length;
            for (int row = ship.row; row < ship.row + ship.length; row++)
            {
                rowsums[row] -= 1;
                grid[row][ship.col] = 0;
            }
        }
        return ship;
    }
};

vector<Puzzle> read_data(string filename)
{
    vector<Puzzle> puzzles;

    ifstream f;
    f.open(filename);
    int npuzzle;
    f>>npuzzle;
    for(int i=0;i<npuzzle;i++)
    {
        int row, col, nship;
        f>>row>>col>>nship;
        vector<int> ships(nship),rowsums(row),colsums(col);
        for(int j=0;j<nship;j++)
            f>>ships[j];
        for(int j=0;j<row;j++)
            f>>rowsums[j];
        for(int j=0;j<col;j++)
            f>>colsums[j];
        puzzles.push_back({row,col,ships,rowsums,colsums});
    }
    f.close();
    return puzzles;
}

void print_ship(Ship &ship)
{
    cout << "(" << ship.length << "," << ship.row << "," << ship.col << "," << ship.ori << ")" << endl;
}

void print_grid(Puzzle &puzzle, vector<vector<int>> &grid)
{
    cout << "  ";
    for (int i = 0; i < puzzle.col; i++)
    {
        cout << puzzle.colsums[i] << " ";
    }
    cout << endl;
    for (int i = 0; i < puzzle.row; i++)
    {
        cout << puzzle.rowsums[i] << " ";
        for (int j = 0; j < puzzle.col; j++)
        {
            cout << (grid[i][j] == 0 ? "-" : "O") << " ";
        }
    }
    cout << endl;
}

void print_state(State &state)
{
    print_grid(*state.puzzle, state.grid);
    for (int i = 0; i < state.ships.size(); i++)
        print_ship(state.ships[i]);
}

void print_solution(Puzzle &puzzle, vector<Ship> &solution)
{
    int nship = solution.size();

    vector<vector<int>> grid(puzzle.row, vector<int>(puzzle.col, 0));
    for (int i = 0; i < nship; i++)
    {
        Ship &ship = solution[i];
        if (ship.ori == 0)
        {
            for (int col = ship.col; col < ship.col + ship.length; col++)
            {
                grid[ship.row][col] = 1;
            }
        }
        else
        {
            for (int row = ship.row; row < ship.row + ship.length; row++)
            {
                grid[row][ship.col] = 1;
            }
        }
    }

    cout << "  ";
    for (int i = 0; i < puzzle.col; i++)
    {
        cout << puzzle.colsums[i] << " ";
    }
    cout << endl;
    for (int i = 0; i < puzzle.row; i++)
    {
        cout << puzzle.rowsums[i] << " ";
        for (int j = 0; j < puzzle.col; j++)
        {
            cout << (grid[i][j] == 0 ? "-" : "O") << " ";
        }
        cout << endl;
    }
}