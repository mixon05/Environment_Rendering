# GKOM_Environment_Rendering

## Uwagi

1. Używać float zamiast double
2. Fajna książka: https://learnopengl.com/

# Ruch

W/S - przód/tył
A/D - lewo/prawo
Spacja (+shift) - góra/dół

Strzałki - obrót

## Linux

Instalacja GLM (wymagane do poprawnego funkcjonowania projektu)
$ sudo apt update
$ sudo apt install libglm-dev

$ cd gkom_environment_rendering
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./gkom_environment_rendering

## Windows

Trzeba sprawdzić czy działa.

## TODO

woda - góry otoczone jeziorem - poziom wody w pliku konfiguracyjnym
(można użyc normal mapy)
(czas uniform - zmiana wsp tekstury)
