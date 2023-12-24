#include "game_of_life.h"

int main() {
    int **field; // pointer to 2D array for field
    int flag = 0; // flag for program exit
    int check = initialize(&field); // initialize field
    FILE *rc = freopen("/dev/tty", "r", stdin); // open terminal for input
    if (rc == NULL || check) {
        printf("n/a");
        flag = 1; // set flag if file or initialization fails
    }
    run_program(&field); // run program
    return 0;
}

int run_program(int ***field) {
    // initialize the new terminal settings
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
            tcsetattr(0, TCSANOW, &new_settings); // change old settings to new ones
            int r = read(STDIN_FILENO, &command, sizeof(char));
            if (r) {
                if (command == 'a' || command == 'A') { // command to increase the speed of the game
                    if (speed > 0 && speed < 200) // checking the speed limits
                        speed--;
                    else
                        speed++;
                } else if (command == 'z' || command == 'Z') { // command to reduce the speed of the game
                    if (speed > 0 && speed < 200) // checking the speed limits
                        speed++;
                    else
                        speed--;
                } else if (command == 'q' || command == 'Q') {
                    keep_running = 0;  // Exit the loop
                }
            }
            tcsetattr(0, TCSANOW, &initial_settings); // change new settings to old ones
            clear_screen();
            usleep(5000 * speed); // suspend the execution of the current thread for a specified number of microseconds
        }
    }
    free(*field);
    return 0;
}

int initialize(int ***grid) {
    // Allocate memory for the grid
    *grid = (int **)malloc(RAWS * sizeof(int *) + RAWS * COLS * sizeof(int));
    if (*grid == NULL) return 1; // Check for successful allocation

    // Initialize pointers for the grid
    int *ptr = (int *)(*grid + RAWS);
    for (int i = 0; i < RAWS; i++)
        (*grid)[i] = ptr + COLS * i;

    // Read the grid values from the input
    for (int i = 0; i < RAWS; i++) {
        for (int j = 0; j < COLS; j++)
            if (scanf("%d", &(*grid)[i][j]) != 1) return 1; // Check for successful read
    }

    return 0;
}

int counter(int **field, int x, int y) {
    int sum = 0; // Initialize sum variable
    for (int i = -1; i < 2; i++) { // Loop through -1, 0, 1
        for (int j = -1; j < 2; j++) { // Loop through -1, 0, 1
            int raw = (x + i + RAWS) % RAWS; // Calculate row index
            int col = (y + j + COLS) % COLS; // Calculate column index
            sum += field[raw][col]; // Add the cell value to sum
        }
    }
    sum -= field[x][y]; // Subtract the cell itself from sum
    return sum; // Return the calculated sum
}

int update_field(int ***field) {
    int **new_field;
    // Allocate memory for the new grid
    new_field = (int **)malloc(RAWS * sizeof(int *) + RAWS * COLS * sizeof(int));
    if (new_field == NULL) { // Check for successful allocation
        free(new_field);
        return 1;
    }
    // Initialize pointers for the grid
    int *ptr = (int *)(new_field + RAWS);
    for (int i = 0; i < RAWS; i++) new_field[i] = ptr + COLS * i;

    // Copy old data into the new array and calculate each cell's next state
    for (int i = 0; i < RAWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Count the number of neighbours for each ceil
            int neighbors = counter((*field), i, j);
            if ((*field)[i][j] == 1) {
                // If the current cell is alive and has two or three live neighbours then it will stay alive
                if (neighbors == 2 || neighbors == 3) {
                    new_field[i][j] = 1;
                } else {
                    new_field[i][j] = 0;
                }
            } else {
                // If the current cell is dead and has exactly three live neighbours then it will be born
                if (neighbors == 3) {
                    new_field[i][j] = 1;
                } else {
                    new_field[i][j] = 0;
                }
            }
        }
    }

    // If last state of the grid is exactly the same as new state - program will stop
    if (is_game_over(new_field, *field)) {
        printf("Game is over\n");
        free(new_field);
        return 1;
    }

    // Copy the new grid into our current grid
    for (int i = 0; i < RAWS; i++) {
        for (int j = 0; j < COLS; j++) {
            (*field)[i][j] = new_field[i][j];
        }
    }

    // Freeing up the memory of the auxiliary grid
    free(new_field);
    return 0;
}

void display(int **field) {
    // Loop through each row
    for (int i = 0; i < RAWS; i++) {
        // Loop through each column
        for (int j = 0; j < COLS; j++) {
            // Display character based on cell value
            if (field[i][j] == 0) {
                printf("░"); // Display empty cell
            } else {
                printf("▓"); // Display filled cell
            }
        }
        printf("\n"); // Move to next row
    }
}

void clear_screen() { 
    printf("\033[H\033[J"); // Clear screen by printing escape sequences
}

int is_game_over(int **field, int **prev_field) {
    int is_same = 1;
    // Loop through rows
    for (int i = 0; i < RAWS && is_same; i++) {
        // Loop through columns
        for (int j = 0; j < COLS && is_same; j++) {
            // Compare elements
            if (field[i][j] != prev_field[i][j]) {
                is_same = 0;
            }
        }
    }
    return is_same;
}

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
