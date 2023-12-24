#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define COLS 80
#define RAWS 25

#define STEP_DELAY_SECONDS 0.01

int initialize(int ***field);
void display(int **field);
void clear_screen();
int update_field(int ***field);
int counter(int **field, int x, int y);
int is_game_over(int **field, int **prev_field);
void print_menu();
int run_program(int ***field);

#endif // GAME_OF_LIFE_H