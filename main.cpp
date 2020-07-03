#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include <cmath>
#include <queue>
#include <queue>
#include "puzzle.h"
using namespace std;

vector<vector<Ship>> solutions;
queue<State *> q;

void dfs_step(State &state)
{
    if (state.ships.size() == state.puzzle->ships.size())
    {
#pragma omp critical
        {
            solutions.push_back(state.ships);
        }
    }
    else
    {
        int length = state.puzzle->ships[state.ships.size()];
        for (int row = 0; row < state.puzzle->row; row++)
        {
            for (int col = 0; col < state.puzzle->col; col++)
            {
                for (int ori = 0; ori < (length != 1) + 1; ori++)
                {
                    Ship ship = {length, row, col, ori};
                    if (state.is_legal(ship))
                    {
                        state.push(ship);
                        dfs_step(state);
                        state.pop();
                    }
                }
            }
        }
    }
}

void dfs_serial(Puzzle &puzzle)
{
    State state(puzzle);
    dfs_step(state);
}

void bfs_step()
{
    State *state = nullptr;
#pragma omp critical
    {
        state = q.front();
        q.pop();
    }
    if (state != nullptr)
    {
        if (state->ships.size() == state->puzzle->ships.size())
        {
#pragma omp critical
            solutions.push_back(state->ships);
        }
        else
        {
            int length = state->puzzle->ships[state->ships.size()];
            for (int row = 0; row < state->puzzle->row; row++)
            {
                for (int col = 0; col < state->puzzle->col; col++)
                {
                    for (int ori = 0; ori < (length != 1) + 1; ori++)
                    {
                        Ship ship = {length, row, col, ori};
                        if (state->is_legal(ship))
                        {
                            State *new_state = new State(*state);
                            new_state->push(ship);
#pragma omp critical
                            q.push(new_state);
                        }
                    }
                }
            }
        }
        delete state;
    }
}

void bfs_serial(Puzzle puzzle)
{
    State *state = new State(puzzle);
    q.push(state);
    ntask = 0;
    while (!q.empty())
    {
        int qsize = q.size();
        for (int i = 0; i < qsize; i++)
        {
            bfs_step();
        }
    }
}

void bfs_parallel(Puzzle puzzle)
{

    State *state;
    state = new State(puzzle);
    q.push(state);
    while (!q.empty())
    {
        int qsize = q.size();
#pragma omp parallel for
        for (int i = 0; i < qsize; i++)
        {
            bfs_step();
        }
    }
}

void dfs_parallel_step(State &state)
{
    int length = state.puzzle->ships[state.ships.size()];
    for (int row = 0; row < state.puzzle->row; row++)
    {
        for (int col = 0; col < state.puzzle->col; col++)
        {
            for (int ori = 0; ori < (length != 1) + 1; ori++)
            {
                Ship ship = {length, row, col, ori};
                if (state.is_legal(ship))
                {
                    state.push(ship);
                    if (state.ships.size() < log2(state.puzzle->ships.size()))
                    {
#pragma omp task
                        dfs_parallel_step(state);
                    }
                    else
                    {
                        dfs_step(state);
                    }
                    state.pop();
                }
            }
        }
    }
    //#pragma omp taskwait
}

void dfs_parallel(Puzzle puzzle)
{
    State state(puzzle);
#pragma omp parallel
    {
#pragma omp single
        dfs_parallel_step(state);
    }
}

int main(int argc, char *argv[])
{
    vector<Puzzle> puzzles;
    int nsolution = 0;
    int idx = 2;
    int num_threads = 1;
    auto start = chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();
    auto time = (end - start).count();
    if (argc < 3)
    {
        cout << "solver algorithm num_threads [filename]" << endl;
        cout << "algorithm" << endl;
        cout << "  0: bfs_serial" << endl;
        cout << "  1: bfs_parallel" << endl;
        cout << "  2: dfs_serail" << endl;
        cout << "  3: dfs_parallel" << endl;
        exit(0);
    }
    else
    {
        idx = stoi(argv[1]);
        num_threads = stoi(argv[2]);
        omp_set_num_threads(num_threads);

        if (argc == 3)
        {
            puzzles.push_back(
                {
                    10,
                    10,
                    {4, 3, 3, 2, 2, 2, 1, 1, 1, 1},
                    {2, 4, 3, 3, 2, 4, 1, 1, 0, 0},
                    {0, 5, 0, 2, 2, 3, 1, 3, 2, 2},
                });
            // puzzles.push_back({4, 4, {2, 1, 1}, {1, 1, 1, 1}, {1, 0, 0, 3}});
            // puzzles.push_back({
            //     7,
            //     7,
            //     {3, 2, 2, 1, 1, 1},
            //     {0, 2, 1, 1, 2, 0, 4},
            //     {2, 0, 2, 1, 3, 2, 0},
            // });
        }
        else
        {
            puzzles = read_data(argv[3]);
            cout << "read " << puzzles.size() << " data" << endl;
        }
    }

    if (idx == 0)
    {
        time = 0;
        nsolution = 0;
        for (int i = 0; i < puzzles.size(); i++)
        {
            Puzzle puzzle(puzzles[i]);
            solutions.clear();
            start = chrono::high_resolution_clock::now();
            bfs_serial(puzzle);
            end = chrono::high_resolution_clock::now();
            time += (end - start).count();
            nsolution += solutions.size();
            //time /= puzzles.size();
            cout << "bfs_serial time(ms):" << time / 1e6 << endl;
            cout << "  num solutions:" << nsolution << endl;
        }
    }
    else if (idx == 1)
    {
        time = 0;
        nsolution = 0;
        for (int i = 0; i < puzzles.size(); i++)
        {
            Puzzle puzzle(puzzles[i]);
            solutions.clear();
            start = chrono::high_resolution_clock::now();

            bfs_parallel(puzzle);

            end = chrono::high_resolution_clock::now();
            time += (end - start).count();
            nsolution += solutions.size();
        }
        //time /= puzzles.size();
        cout << "bfs_parallel(" << num_threads << ") time(ms):" << time / 1e6 << endl;
        cout << "  num solutions:" << nsolution << endl;
    }
    else if (idx == 2)
    {
        time = 0;
        nsolution = 0;
        for (int i = 0; i < puzzles.size(); i++)
        {
            Puzzle puzzle(puzzles[i]);
            solutions.clear();
            start = chrono::high_resolution_clock::now();
            dfs_serial(puzzle);
            end = chrono::high_resolution_clock::now();
            time += (end - start).count();
            nsolution += solutions.size();
        }
        //time /= puzzles.size();
        cout << "dfs_serial time(ms):" << time / 1e6 << endl;
        cout << "  num solutions:" << nsolution << endl;
    }
    else if (idx == 3)
    {
        time = 0;
        nsolution = 0;
        for (int i = 0; i < puzzles.size(); i++)
        {
            Puzzle puzzle(puzzles[i]);
            solutions.clear();
            start = chrono::high_resolution_clock::now();
            dfs_parallel(puzzle);
            end = chrono::high_resolution_clock::now();
            time += (end - start).count();
            nsolution += solutions.size();
        }
        //time /= puzzles.size();
        cout << "dfs_parallel(" << num_threads << ") time(ms):" << time / 1e6 << endl;
        cout << "  num solutions:" << nsolution << endl;
    }

    if (argc == 3)
    {
        solutions.clear();
        switch (idx)
        {
        case 0:
            bfs_serial(puzzles[0]);
            break;
        case 1:
            bfs_parallel(puzzles[0]);
            break;
        case 2:
            dfs_serial(puzzles[0]);
            break;
        case 3:
            dfs_parallel(puzzles[0]);
            break;
        }
        print_solution(puzzles[0], solutions[0]);
    }
}