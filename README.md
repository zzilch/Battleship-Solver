# Battleship-Solver
A OpenMP based battleship solver

## Enviroment
OS: Ubuntu 18.04.3 LTS x86_64  
Kernel: 5.0.0-32-generic  
Compiler: gcc (Ubuntu 6.5.0-2ubuntu1~18.04) 6.5.0   20181026
Build system: cmake version 3.16.0-rc2  
OpenMp: 4.5  

## Build & Run
```bash
mkdir build
cmake -B build
cmake --build build
bash valid.sh > valid.txt
bash test.sh > test.txt
```