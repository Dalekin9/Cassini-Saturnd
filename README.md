# SY5-projet-2021-2022

Le projet du cours de systèmes d'exploitation (L3), 2021-2022

## Compile and run
The Makefile compiles the project into one or two files at the root of this repo :
- `make cassini` compiles the client into a `cassini` file
- `make saturnd` compiles the daemon into a `saturnd` file
- `make all` compiles both

To clean up all the .o files and the executables, run `make clean`.

Run `./cassini` to launch the client and `./saturnd` to launch the daemon.
