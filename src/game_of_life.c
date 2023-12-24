#include "game_of_life.h"

int main() {
    int **field;
    int flag = 0;
    int check = initialize(&field);
    FILE *rc = freopen("/dev/tty", "r",
                       stdin);  // Перенаправление стандартного ввода для чтения в терминале
    if (rc == NULL || check) {
        printf("n/a");
        flag = 1;
    }
    run_program(&field);
    return 0;
}

int run_program(int ***field) {
    // Структура для хранения настроек терминала
    struct termios initial_settings, new_settings;
    tcgetattr(0, &initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= (~ICANON & ~ECHO);
    new_settings.c_cc[VTIME] = STEP_DELAY_SECONDS * 10;
    new_settings.c_cc[VMIN] = 0;

    int keep_running = 1, flag = 0, speed = 10;

    while (flag == 0 && keep_running) {
        display(*field);
        int check_error = update_field(&(*field));
        if (check_error) {
            flag = 1;
        }
        if (flag == 0) {
            print_menu();
            char command;
            tcsetattr(0, TCSANOW, &new_settings);
            int r = read(STDIN_FILENO, &command, sizeof(char));
            if (r) {
                if (command == 'a' || command == 'A') {
                    if (speed > 0 && speed < 200)
                        speed--;
                    else
                        speed++;
                } else if (command == 'z' || command == 'Z') {
                    if (speed > 0 && speed < 200)
                        speed++;
                    else
                        speed--;
                } else if (command == 'q' || command == 'Q') {
                    keep_running = 0;  // Exit the loop
                }
            }
            tcsetattr(0, TCSANOW, &initial_settings);
            clear_screen();
            usleep(5000 * speed);
        }
    }
    free(*field);
    return 0;
}

int initialize(int ***grid) {
    *grid = (int **)malloc(RAWS * sizeof(int *) + RAWS * COLS * sizeof(int));
    if (*grid == NULL) return 1;
    int *ptr = (int *)(*grid + RAWS);
    for (int i = 0; i < RAWS; i++) (*grid)[i] = ptr + COLS * i;

    for (int i = 0; i < RAWS; i++) {
        for (int j = 0; j < COLS; j++)
            if (scanf("%d", &(*grid)[i][j]) != 1) return 1;
    }

    return 0;
}

int counter(int **field, int x, int y) {
    int sum = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int raw = (x + i + RAWS) % RAWS;
            int col = (y + j + COLS) % COLS;
            sum += field[raw][col];
        }
    }
    sum -= field[x][y];
    return sum;
}

int update_field(int ***field) {
    int **new_field;
    new_field = (int **)malloc(RAWS * sizeof(int *) + RAWS * COLS * sizeof(int));
    if (new_field == NULL) {
        free(new_field);
        return 1;
    }
    int *ptr = (int *)(new_field + RAWS);
    for (int i = 0; i < RAWS; i++) new_field[i] = ptr + COLS * i;

    for (int i = 0; i < RAWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int neighbors = counter((*field), i, j);

            if ((*field)[i][j] == 1) {
                // живая клетка
                if (neighbors == 2 || neighbors == 3) {
                    new_field[i][j] = 1;  // остается живой
                } else {
                    new_field[i][j] = 0;  // умирает
                }
            } else {
                // мертвая клетка
                if (neighbors == 3) {
                    new_field[i][j] = 1;  // возрождается
                } else {
                    new_field[i][j] = 0;  // остается мертвой
                }
            }
        }
    }

    // проверка на окончание программы
    if (is_game_over(new_field, *field)) {
        printf("Game is over\n");
        free(new_field);
        return 1;
    }

    // присваивание новой матрице старой
    for (int i = 0; i < RAWS; i++) {
        for (int j = 0; j < COLS; j++) {
            (*field)[i][j] = new_field[i][j];
        }
    }

    free(new_field);
    return 0;
}

// отображение поля в терминале
void display(int **field) {
    for (int i = 0; i < RAWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (field[i][j] == 0) {
                printf("░");  // пустая клетка
            } else {
                printf("▓");  // заполненная клетка
            }
        }
        printf("\n");  // переход на новую строку после отображения каждой строки поля
    }
}

// очистка терминала
void clear_screen() { printf("\033[H\033[J"); }

// проверка на окончание программы
int is_game_over(int **field, int **prev_field) {
    int is_same = 1;

    for (int i = 0; i < RAWS && is_same; i++) {
        for (int j = 0; j < COLS && is_same; j++) {
            if (field[i][j] != prev_field[i][j]) {
                is_same = 0;
            }
        }
    }

    return is_same;
}

// вывод меню
void print_menu() {
    printf(
        "1.txt - устойчивые фигуры (пасека, би блоки, пекарня, длинна баржа, "
        "змейка и другие)\n"
        "2.txt - долгожители\n"
        "3.txt - ружье\n"
        "4.txt - периодические фигуры (галактика Коко, французский поцелуй, "
        "крест, звезда, пентадекатлон)\n"
        "5.txt - двигающиеся фигуры (паровоз + 2 маленьких корабля и 1 большой)\n"
        "\n"
        "a - увеличение скорости\n"
        "z - уменьшение скорости\n"
        "q - выход из игры\n");
}
