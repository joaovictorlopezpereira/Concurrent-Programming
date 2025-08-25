
@echo off
gcc test.c -o test -Wall
gcc create-vector.c -o create-vector -Wall
gcc seq-dotp.c -o seq-dotp -Wall
gcc conc-dotp.c -o conc-dotp -Wall
