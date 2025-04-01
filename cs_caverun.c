// cs_caverun.c
// Written by Tony Chen z5687393 on <30/03/2024>
//
// Description: Assignment 1/CS Caverun game 

// Provided Libraries
#include <stdio.h>


// Add your own #include statements below this line
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Provided constants
#define COLS 10
#define ROWS 10
#define INVALID_ROW -1
#define INVALID_COL -1
#define INITIAL_LIVES 3

// Add your own #defines constants below this line
#define TRUE 1
#define FALSE 0
#define DEAD 2
#define GAMEOFLAVA 3
#define LAVASEEDS 4
// Points multiplier for lava modes
#define LAVA_MODE_SCORE_MULTIPLIER 10 
// Input buffer size for code detection
#define KONAMISIZE 1000               
// Unique respawn failure code
#define BURNT 451                     
#define SENTINEL -1

// Provided Enums

// Enum for features on the game board
enum entity {
    EMPTY,
    DIRT,
    WALL,
    BOULDER, 
    GEM,
    EXIT_LOCKED,
    EXIT_UNLOCKED,
    HIDDEN,
    LAVA,
};

// Add your own enums below this line

// Represents a tile/cell on the game board
struct tile {
    enum entity entity;
    enum entity original_entity;
    enum entity spare_entity;
    enum entity lava_entity;
};

// Add your own structs below this line
struct pos {
    int pos_row;
    int pos_col;
    int dash_flag;
};


// Provided Function Prototypes
void initialise_board(struct tile board[ROWS][COLS]);
void print_board(
    struct tile board[ROWS][COLS], 
    int player_row, 
    int player_col,
    int lives_remaining
);
void print_board_line(void);
void print_board_header(int lives);
void print_map_statistics(
    int number_of_dirt_tiles,
    int number_of_gem_tiles,
    int number_of_boulder_tiles,
    double completion_percentage,
    int maximum_points_remaining
);

// Add your function prototypes below this line

//Stage 1 
struct pos scan_starting_pos(void);

void map_features (struct tile board[ROWS][COLS], struct pos starting_pos);
void add_entity (struct tile board[ROWS][COLS], char extra_entity, int entity_pos_row, int entity_pos_col);
void add_multiple_walls(struct tile board[ROWS][COLS], struct pos starting_pos);
int check_multiple_walls(struct tile board[ROWS][COLS], struct pos starting_pos, int start_row, int start_col, int end_row, int end_col);

int check_off_map(int pos_row, int pos_col);
int check_valid_location(int pos_row, int pos_col, struct tile board[ROWS][COLS]);

//Stage 2
void set_initial_pos_empty(struct tile board[ROWS][COLS], struct pos starting_pos);

int check_collision(int pos_row, int pos_col, struct tile board[ROWS][COLS]);

int add_points(struct tile board[ROWS][COLS], struct pos player_pos, int is_game_of_lava, int is_lava_seeds);
void print_statistics(struct tile board[ROWS][COLS], int initial_collectibles_count, int collected, int is_game_of_lava, int is_lava_seeds);
int collectibles_collected(struct tile board[ROWS][COLS], struct pos player_pos);
int initial_collectibles_count(struct tile board[ROWS][COLS]);
int maximum_points_remaining(struct tile board[ROWS][COLS], int is_game_of_lava, int is_lava_seeds);

int player_win(struct tile board[ROWS][COLS], struct pos player_pos, int player_score, int lives);

void check_and_unlock_exit(struct tile board [ROWS][COLS]);
void replace_gem_empty(struct tile board[ROWS][COLS], struct pos player_pos);
void locked_to_unlocked(struct tile board[ROWS][COLS]);

struct pos player_movement(struct tile board[ROWS][COLS], struct pos player_pos, char command);
void single_move(struct tile board[ROWS][COLS], struct pos *player_pos, char command, int radius, int *dash_cooldown);
void command_print_stats(struct tile board[ROWS][COLS], char command, int player_score, int initial_collectibles_count, int num_collected, int is_game_of_lava, int is_lava_seeds);

void print_quit_game(struct tile board[ROWS][COLS], struct pos player_pos, int lives, int death_flag, char command);
void dirt_to_empty(struct tile board[ROWS][COLS], struct pos player_pos);

//Stage 3
void dash_first_move(struct tile board[ROWS][COLS], struct pos *player_pos, char command, int *dash_cooldown, int *num_collected, int *player_score, int *is_game_of_lava, int *is_lava_seeds);
void dash_first_move_up(struct tile board[ROWS][COLS], struct pos *player_pos);
void dash_first_move_left(struct tile board[ROWS][COLS], struct pos *player_pos);
void dash_first_move_down(struct tile board[ROWS][COLS], struct pos *player_pos);
void dash_first_move_right(struct tile board[ROWS][COLS], struct pos *player_pos);

void dash_second_move(struct tile board[ROWS][COLS], struct pos *player_pos, char command, int radius);
struct pos second_dash(struct tile board[ROWS][COLS], struct pos player_pos);
int out_of_breath(struct tile board[ROWS][COLS], struct pos player_pos, int dash_cooldown);
int dash_first_move_flag(struct tile board[ROWS][COLS], struct pos player_pos, char command);

void boulder_turn(struct tile board[ROWS][COLS], struct pos player_pos, char gravity_dir, int radius);
void run_game_mechanics(struct tile board[ROWS][COLS], struct pos player_pos, char gravity_dir, int radius, int *num_collected, int *player_score, int *is_game_of_lava, int *is_lava_seeds);
void check_and_move_boulder(struct tile board[ROWS][COLS], struct pos player_pos, int row, int col, char gravity_dir);
void boulder_move(struct tile board[ROWS][COLS], struct pos player_pos, int boulder_row, int boulder_col, char gravity_dir);

int respawn(struct tile board[ROWS][COLS], struct pos player_pos, struct pos starting_pos, int radius, int lives);
void respawn_blocked(struct tile board[ROWS][COLS], struct pos player_pos, int radius, int *death_flag, int player_score);
int player_death_check_respawn(struct tile board[ROWS][COLS], struct pos player_pos, struct pos starting_pos, int player_score);
int player_squashed_by_boulder(struct tile board[ROWS][COLS], struct pos player_pos);

int illumination(struct tile board[ROWS][COLS], struct pos player_pos);
void illuminate(struct tile board[ROWS][COLS], struct pos player_pos, int radius);

int single_dash_command(char command);
int double_dash_command(char command);
int dash_on_cooldown(char command, int dash_cooldown);
int gameplay_command(char command);

char gravity_shift(struct tile board[ROWS][COLS], struct pos player_pos, char gravity_dir);

void hide_and_save_tiles(struct tile board[ROWS][COLS], int row, int col);
void restore_original_entity(struct tile board[ROWS][COLS], int row, int col);

//Stage 4
void lava_turn(struct tile board[ROWS][COLS], struct pos *player_pos, struct pos starting_pos, int *lives, int *death_flag, int player_score, int radius);
int can_respawn_lava(struct tile board[ROWS][COLS], struct pos player_pos, struct pos starting_pos, int player_score);
int lava_death(struct tile board[ROWS][COLS], struct pos player_pos);
int lava_code(struct tile board[ROWS][COLS], struct pos player_pos, char command, int round_counter, char konami_code[KONAMISIZE]);

void game_of_lava(struct tile board[ROWS][COLS]);
void apply_game_of_lava_rules(struct tile board[ROWS][COLS], enum entity next_lava[ROWS][COLS], int row, int col, int alive_neighbors);

void lava_seeds(struct tile board[ROWS][COLS]);
int count_lava_neighbors(struct tile board[ROWS][COLS], int row, int col);
void apply_lava_seeds_rules(struct tile board[ROWS][COLS], enum entity next_lava[ROWS][COLS], int row, int col, int alive_neighbors);
void change_lava_mode(int lava_mode, int *is_game_of_lava, int *is_lava_seeds, int *round_counter);

void shadow_mode_activation(struct tile board[ROWS][COLS], struct pos *player_pos, int *lives, int *shadow_mode);
void shadow_tiles(struct tile board[ROWS][COLS], struct pos player_pos);
void is_tile_shadowed(
    struct tile board[ROWS][COLS], 
    double sample_y,
    double sample_x,
    double epsilon,
    int sample_y_cord, 
    int sample_x_cord,
    int *tangent_from_above,
    int *tangent_from_below,
    int *blocked
);

void ray_sampling(
    struct pos player_pos,
    double horizontal_dist,
    double vertical_dist,
    struct tile board[ROWS][COLS], 
    double epsilon,
    int *tangent_from_above,
    int *tangent_from_below,
    int *blocked,
    int row,
    int col
);
int is_opaque(enum entity entity);

//Functions encountered over multiple stages 
void gameplay(struct tile board[ROWS][COLS], struct pos starting_pos, int initial_collectibles_count);
void gameplay_loop(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos, 
    int initial_collectibles_count, 
    int *player_score, 
    struct pos starting_pos, 
    int *num_collected, 
    char command, 
    int *dash_cooldown, 
    int *lives, 
    int *death_flag, 
    int *radius, 
    char *gravity_dir, 
    int *round_counter,
    char konami_code[KONAMISIZE],
    int *is_game_of_lava,
    int *is_lava_seeds,
    int *shadow_mode                                       
);

void game_lost(struct tile board[ROWS][COLS], struct pos player_pos, int radius, int lives, int player_score);

void commands(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos,         
    int *player_score,             
    int *num_collected,             
    char command,
    int *dash_cooldown,            
    int *lives,      
    int *radius,                  
    char *gravity_dir,
    int *is_game_of_lava,
    int *is_lava_seeds,
    int *shadow_mode    
);
    
void respawn_and_lava_turns(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos, 
    struct pos starting_pos,         
    int *player_score,             
    int *lives,   
    int *death_flag,                              
    int *radius,                  
    int *is_game_of_lava,
    int *is_lava_seeds
);


//Main function
int main(void) {

    printf("Welcome to CS Caverun!\n\n");
    printf("--- Game Setup Phase ---\n");

    // Scan valid starting position
    struct pos starting_pos = scan_starting_pos();
    struct tile board[ROWS][COLS];
    initialise_board(board);
    print_board(board, starting_pos.pos_row, 
        starting_pos.pos_col, INITIAL_LIVES);

    // Map features
    map_features(board, starting_pos);
    int num_collectibles = initial_collectibles_count(board);
    print_board(board, starting_pos.pos_row, 
        starting_pos.pos_col, INITIAL_LIVES);

    // Gameplay
    gameplay(board, starting_pos, num_collectibles);

    return 0;
}

// Add your function definitions below this line

// Initialises player position
struct pos scan_starting_pos(void) {
    struct pos starting_pos;
    printf("Enter the player's starting position: ");
    scanf("%d %d", &starting_pos.pos_row, &starting_pos.pos_col);
    while (check_off_map(starting_pos.pos_row, starting_pos.pos_col) == 1) {
        printf("Position %d %d is invalid!\n", 
            starting_pos.pos_row, starting_pos.pos_col);
        printf("Enter the player's starting position: ");
        scanf("%d %d", &starting_pos.pos_row, &starting_pos.pos_col);
    }

    return starting_pos;
}

// Initialises map features
void map_features(struct tile board[ROWS][COLS], struct pos starting_pos) {
    printf("Enter map features:\n");
    char extra_entity;
    int entity_pos_row;
    int entity_pos_col;

    while (scanf(" %c", &extra_entity) == 1 && extra_entity != 's') {
        // Skips over non-valid inputs
        if (extra_entity != 'w' && 
            extra_entity != 'b' &&
            extra_entity != 'g' &&
            extra_entity != 'e' &&
            extra_entity != 'W' &&
            extra_entity != 'l') {

        } else if (extra_entity == 'W') {
            add_multiple_walls(board, starting_pos);
        } else {
            // Scans in coordinates for the entity added
            scanf(" %d %d", &entity_pos_row, &entity_pos_col);
            // Only add if valid 
            if (check_off_map(entity_pos_row, entity_pos_col) == TRUE) {
                printf("Invalid location: position is not on map!\n");
            } else if ((check_valid_location(entity_pos_row, 
                entity_pos_col, board) == TRUE) || 
            (starting_pos.pos_row == entity_pos_row && 
            starting_pos.pos_col == entity_pos_col)) {
                printf("Invalid location: tile is occupied!\n");
            } else {
                add_entity(board, extra_entity, entity_pos_row, entity_pos_col);
            }
        }
    }
    set_initial_pos_empty(board, starting_pos);
    check_and_unlock_exit(board);
}

// Check if tile is occupied
int check_valid_location(
    int pos_row, 
    int pos_col, 
    struct tile board[ROWS][COLS]
) {
    return board[pos_row][pos_col].entity != DIRT;
}

// Check if placed entity/object in grid
int check_off_map(int pos_row, int pos_col) {
    return ((pos_row <= INVALID_ROW) ||
        (pos_col <= INVALID_COL) ||
        (pos_row >= ROWS) ||
        (pos_col >= COLS));
}

// Allows user to add map features
void add_entity (
    struct tile board[ROWS][COLS], 
    char extra_entity, 
    int entity_pos_row,
    int entity_pos_col
) {
    if (extra_entity == 'w') {
        board[entity_pos_row][entity_pos_col].entity = WALL;
    } else if (extra_entity == 'b') {
        board[entity_pos_row][entity_pos_col].entity = BOULDER;
    } else if (extra_entity == 'g') {
        board[entity_pos_row][entity_pos_col].entity = GEM;
    } else if (extra_entity == 'e') {
        board[entity_pos_row][entity_pos_col].entity = EXIT_LOCKED;
    } else if (extra_entity == 'l') {
        board[entity_pos_row][entity_pos_col].lava_entity = LAVA;
    }
}

// Add multiple walls with upper case W and
// start and end coordinates
void add_multiple_walls(
    struct tile board[ROWS][COLS], 
    struct pos starting_pos
) {
    int start_row;
    int start_col;
    int end_row;
    int end_col;
    // scans starting and ending coordinates
    scanf(" %d %d %d %d", &start_row, &start_col, &end_row, &end_col);
    int valid = TRUE;
    int valid_feature = TRUE;
    // Check for invalid positions
    if (check_off_map(start_row, start_col) == TRUE || 
    check_off_map(end_row, end_col) == TRUE) {
        printf("Invalid location: feature cannot be placed here!\n");
        valid = FALSE;
    } else { 
        if (check_multiple_walls(board, starting_pos, 
            start_row, start_col, end_row, end_col)) {
            valid_feature = FALSE;
            valid = FALSE;
        }
    } 
    if (valid_feature == FALSE) {
        printf("Invalid location: feature cannot be placed here!\n");
    }
    // Loops through every tile in between start and end coordinates
    // and places a wall there
    if (valid == TRUE) {
        int i = 0;
        int j = 0;
        while (i <= end_row - start_row) {
            while (j <= end_col - start_col) {
                (board[start_row + i][start_col + j].entity = WALL);
                j++;
            }
            j = 0;
            i++;
        }
    }
}

// Check if multiple walls can be placed
int check_multiple_walls(
    struct tile board[ROWS][COLS], 
    struct pos starting_pos,
    int start_row,
    int start_col,
    int end_row, 
    int end_col
) {
    int valid = TRUE;
    int i = 0;
    int j = 0;
    // Loop through all the tiles which wants a wall to be placed
    while (i <= end_row - start_row) {
        while (j <= end_col - start_col) {
            if (board[start_row + i][start_col + j].entity != DIRT 
                || (starting_pos.pos_row == start_row + i 
                && starting_pos.pos_col == start_col + j)) {
                valid = FALSE;
            }
            j++;
        }
        j = 0;
        i++;
    }
    // If even 1 wall is invalid all walls cannot be placed
    if (valid == FALSE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

//Changes the intial player position from dirt to empty
void set_initial_pos_empty(
    struct tile board[ROWS][COLS], 
    struct pos starting_pos
) {
    board[starting_pos.pos_row][starting_pos.pos_col].entity = EMPTY;
}

//Main gameplay phase
void gameplay(
    struct tile board[ROWS][COLS], 
    struct pos starting_pos, 
    int initial_collectibles_count
) {
    // Initialises variables here to save lines
    struct pos player_pos = starting_pos;
    int player_score = 0;
    int num_collected = 0;
    char command = 'a';
    int dash_cooldown = 0;
    int lives = INITIAL_LIVES;
    int death_flag = FALSE;
    int radius = KONAMISIZE;
    char gravity_dir = 's';
    int round_counter = 0;
    char konami_code[KONAMISIZE] = {0}; 
    int is_game_of_lava = FALSE;
    int is_lava_seeds = FALSE;
    int shadow_mode = FALSE;

    player_pos.dash_flag = FALSE;
    printf("--- Gameplay Phase ---\n");

    gameplay_loop(
        board, 
        &player_pos,                    
        initial_collectibles_count,
        &player_score,                  
        starting_pos, 
        &num_collected,                 
        command,
        &dash_cooldown,                 
        &lives,                         
        &death_flag,                    
        &radius,                       
        &gravity_dir,
        &round_counter,
        konami_code,
        &is_game_of_lava,
        &is_lava_seeds,
        &shadow_mode                   
    );
}

//Players turn (including printing points and statistics)
void gameplay_loop(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos,         
    int initial_collectibles_count,
    int *player_score,             
    struct pos starting_pos, 
    int *num_collected,             
    char command,
    int *dash_cooldown,            
    int *lives,                   
    int *death_flag,                
    int *radius,                  
    char *gravity_dir,
    int *round_counter,
    char konami_code[KONAMISIZE],
    int *is_game_of_lava,
    int *is_lava_seeds,
    int *shadow_mode                                                 
) {
    while (scanf(" %c", &command) == 1 && command != 'q' &&
           board[player_pos->pos_row][player_pos->pos_col].entity 
           != EXIT_UNLOCKED && 
           *death_flag == FALSE && *lives > 0) {
        //Set initial position empty
        dirt_to_empty(board, *player_pos);
        //Check if konami code is entered and if yes, change mode
        int lava_mode = lava_code(board, *player_pos, command, 
            *round_counter, konami_code);
        change_lava_mode(lava_mode, is_game_of_lava, is_lava_seeds, 
            round_counter);

        commands(board, player_pos, player_score, num_collected, command, 
            dash_cooldown, lives, radius, gravity_dir, is_game_of_lava, 
            is_lava_seeds, shadow_mode);

        // Doesn't apply lava games until next command 
        // scanned in to avoid overcounting
        if (command == 'L') {
            continue;
        }

        // Don't run game mechanics if player's already won
        if (player_win(board, *player_pos, *player_score, *lives)) {

        } else if (gameplay_command(command)) {
            run_game_mechanics(board, *player_pos, *gravity_dir, *radius, 
                num_collected, player_score, is_game_of_lava, is_lava_seeds);
            respawn_and_lava_turns(board, player_pos, starting_pos, 
                player_score, lives, death_flag, radius, is_game_of_lava, 
                is_lava_seeds);
            if (*shadow_mode == TRUE) {
                shadow_tiles(board, *player_pos);
            }
            
            if (*lives > 0) {
                print_board(board, player_pos->pos_row, 
                    player_pos->pos_col, *lives);
            }
        }

        // Only prints if no lives left
        game_lost(board, *player_pos, *radius, *lives, *player_score);
        
        command_print_stats(board, command, 
            *player_score, initial_collectibles_count, 
            *num_collected, *is_game_of_lava, *is_lava_seeds);
    }

    print_quit_game(board, *player_pos, *lives, *death_flag, command);
}

int single_dash_command(char command) {
    return (command == 'w' || command == 'a' || 
        command == 's' || command == 'd' || command == 'r');
}
    
int double_dash_command(char command) {
    return (command == 'W' || command == 'A' || 
        command == 'S' || command == 'D');
}

int dash_on_cooldown(char command, int dash_cooldown) {
    return (command != 'p' && command != 'm' && command != 'L' &&
        command != 'i' && command != 'u' && dash_cooldown == 1);
}

int gameplay_command(char command) {
    return (command != 'p' && command != 'm' && 
        command != 'L' && command != 'i' && command != 'u');
}

// Prints statistics when 'm' is entered
void print_statistics(
    struct tile board[ROWS][COLS], 
    int initial_collectibles_count, 
    int collected,
    int is_game_of_lava,
    int is_lava_seeds
) {
    int dirt_left = 0;
    int gems_left = 0;
    int boulders_left = 0;
    // Completion as a percentage
    double collectible_completion = (
        (100.0 * collected) / initial_collectibles_count);

    int points_remaining = maximum_points_remaining(board, 
        is_game_of_lava, is_lava_seeds);
    
    // Checks for remaining dirt, gems and boulders
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < ROWS; col++) {
            if (board[row][col].entity == DIRT || 
                (board[row][col].entity == HIDDEN && 
                    board[row][col].original_entity == DIRT)) {
                dirt_left += 1;
            } else if (board[row][col].entity == GEM || 
                (board[row][col].entity == HIDDEN && 
                    board[row][col].original_entity == GEM)) {
                gems_left += 1;
            } else if (board[row][col].entity == BOULDER || 
                (board[row][col].entity == HIDDEN && 
                    board[row][col].original_entity == BOULDER)) {
                boulders_left += 1;
            }
        }
    }
    print_map_statistics(dirt_left, gems_left, boulders_left, 
        collectible_completion, points_remaining);
}

// As game progresses how many points are left
int maximum_points_remaining(
    struct tile board[ROWS][COLS],
    int is_game_of_lava, 
    int is_lava_seeds
) {
    int points = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < ROWS; col++) {
            if (board[row][col].entity == DIRT || 
                (board[row][col].entity == HIDDEN && 
                    board[row][col].original_entity == DIRT)) {
                points += 1;
            } else if (board[row][col].entity == GEM || 
                (board[row][col].entity == HIDDEN && 
                    board[row][col].original_entity == GEM)) {
                points += 20;
            } 
        }
    }

    // 10x multiplier on for konami games   
    if (is_game_of_lava == TRUE || is_lava_seeds == TRUE) {
        return (points * LAVA_MODE_SCORE_MULTIPLIER);
    } else {
        return points;
    }
}

// Add points for collected gems and dirts
int add_points(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    int is_game_of_lava, 
    int is_lava_seeds
) {
    int player_score = 0;
    if (board[player_pos.pos_row][player_pos.pos_col].entity == GEM) {
        player_score += 20;
    } else if (board[player_pos.pos_row][player_pos.pos_col].entity == DIRT) {
        player_score += 1;
    }

    // 10x multiplier on for konami games
    if (is_game_of_lava == TRUE || is_lava_seeds == TRUE) {
        return (player_score * LAVA_MODE_SCORE_MULTIPLIER);
    } else {
        return player_score;
    }
}

// As game progresses how many collectibles has the player collected
int collectibles_collected(
    struct tile board[ROWS][COLS], 
    struct pos player_pos
) {
    int collectibles_collected = 0;
    if (board[player_pos.pos_row][player_pos.pos_col].entity == GEM) {
        collectibles_collected += 1;
    } else if (board[player_pos.pos_row][player_pos.pos_col].entity == DIRT) {
        collectibles_collected += 1;
    }
        
    return collectibles_collected;
}

//Initially how many collectible objects there are
int initial_collectibles_count(struct tile board[ROWS][COLS]) {
    int num_collected = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < ROWS; col++) {
            if (board[row][col].entity == GEM) {
                num_collected += 1;
            } else if (board[row][col].entity == DIRT) {
                num_collected += 1;
            }
        }
    }
    
    return num_collected;
}

//Counts how many gems and dirt is left for map stats
int collectibles_remaining(struct tile board[ROWS][COLS]) {
    int num_collectibles = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < ROWS; col++) {
            if (board[row][col].entity == DIRT) {
                num_collectibles += 1;
            } else if (board[row][col].entity == GEM) {
                num_collectibles += 1;
            } 
        }
    }

    return num_collectibles;
}

//Check overlap with entity
int check_collision(
    int pos_row, 
    int pos_col, 
    struct tile board[ROWS][COLS]
) {
    return (board[pos_row][pos_col].entity == WALL || 
        board[pos_row][pos_col].entity == BOULDER) ||
        board[pos_row][pos_col].entity == EXIT_LOCKED;
}

// Checks if all gems are collected before unlocking exit
void check_and_unlock_exit(struct tile board [ROWS][COLS]) {
    int unlocked = TRUE;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < ROWS; col++) {
            if (board[row][col].entity == GEM || 
                (board[row][col].entity == HIDDEN && 
                board[row][col].original_entity == GEM)) {
                unlocked = FALSE;
            } 
        }
    }
    if (unlocked == TRUE) {
        locked_to_unlocked(board);
    }
}

// Unlocks exits, even if hidden
// Function runs only after all gems collected
void locked_to_unlocked(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == EXIT_LOCKED) {
                board[row][col].entity = EXIT_UNLOCKED;
            } else if (board[row][col].entity == HIDDEN && 
                       board[row][col].original_entity == EXIT_LOCKED) {
                board[row][col].original_entity = EXIT_UNLOCKED;
            }
        }
    }
}

// Prints comment for a player win
int player_win(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    int player_score,
    int lives
) {
    if (board[player_pos.pos_row][player_pos.pos_col].entity == EXIT_UNLOCKED) {
        print_board(board, player_pos.pos_row, 
            player_pos.pos_col, lives);      
        printf("You Win! Final Score: %d point(s)!\n", player_score); 
        return TRUE;
    } else {
        return FALSE;
    }
}

// Function runs after points have been collected so correct points are added
void replace_gem_empty(struct tile board[ROWS][COLS], struct pos player_pos) {
    if (board[player_pos.pos_row][player_pos.pos_col].entity == GEM) {
        board[player_pos.pos_row][player_pos.pos_col].entity = EMPTY;
    } 
}

// Single movement commands
// NOTE: 'wasd' not #define because they are universal gaming commands
struct pos player_movement(
    struct tile board[ROWS][COLS],
    struct pos player_pos, 
    char command
) {
    if (command == 'w') {
        player_pos.pos_row -= 1;
        if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
            player_pos.pos_row += 1;
        } else if (check_collision(player_pos.pos_row, 
            player_pos.pos_col, board)) {
            player_pos.pos_row += 1;
        }
    } else if (command == 'a') {
        player_pos.pos_col -= 1;
        if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
            player_pos.pos_col += 1;
        } else if (check_collision(player_pos.pos_row, 
            player_pos.pos_col, board)) {
            player_pos.pos_col += 1;
        }
    } else if (command == 's') {
        player_pos.pos_row += 1;
        if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
            player_pos.pos_row -= 1;
        } else if (check_collision(player_pos.pos_row, 
            player_pos.pos_col, board)) {
            player_pos.pos_row -= 1;
        }
    } else if (command == 'd') {
        player_pos.pos_col += 1;
        if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
            player_pos.pos_col -= 1;
        } else if (check_collision(player_pos.pos_row, 
            player_pos.pos_col, board)) {
            player_pos.pos_col -= 1;
        }
    } else if (command == 'r') {

    }

    return player_pos;
}

// Applies (if allowed) the first move of the dash
void dash_first_move(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos, 
    char command,
    int *dash_cooldown,
    int *num_collected,
    int *player_score,
    int *is_game_of_lava,
    int *is_lava_seeds
) {
    *dash_cooldown = 1;
    if (command == 'W') {
        dash_first_move_up(board, player_pos);
    } else if (command == 'A') {
        dash_first_move_left(board, player_pos);
    } else if (command == 'S') {
        dash_first_move_down(board, player_pos);
    } else if (command == 'D') {
        dash_first_move_right(board, player_pos);
    } 
    // Collects points before second dash
    *num_collected += collectibles_collected(board, *player_pos);
    *player_score += add_points(board, *player_pos, 
        *is_game_of_lava, *is_lava_seeds); 
}

// First move of dash is up
void dash_first_move_up(
    struct tile board[ROWS][COLS],
    struct pos *player_pos
) {
    player_pos->pos_row -= 1;
    if (check_off_map(player_pos->pos_row, player_pos->pos_col)) {
        player_pos->pos_row += 1;
        player_pos->dash_flag = TRUE;
    } else if (check_collision(player_pos->pos_row, 
        player_pos->pos_col, board)) {
        player_pos->pos_row += 1;
        player_pos->dash_flag = TRUE;
    } else {
        player_pos->dash_flag = FALSE;
    }
}

// First move of dash is left
void dash_first_move_left(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos
) {
    player_pos->pos_col -= 1;
    if (check_off_map(player_pos->pos_row, player_pos->pos_col)) {
        player_pos->pos_col += 1;
        player_pos->dash_flag = TRUE;
    } else if (check_collision(player_pos->pos_row, 
        player_pos->pos_col, board)) {
        player_pos->pos_col += 1;
        player_pos->dash_flag = TRUE;
    } else {
        player_pos->dash_flag = FALSE;
    }
}

// First move of dash is down
void dash_first_move_down(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos
) {
    player_pos->pos_row += 1;
    if (check_off_map(player_pos->pos_row, player_pos->pos_col)) {
        player_pos->pos_row -= 1;
        player_pos->dash_flag = TRUE;
    } else if (check_collision(player_pos->pos_row, 
        player_pos->pos_col, board)) {
        player_pos->pos_row -= 1;
        player_pos->dash_flag = TRUE;
    } else {
        player_pos->dash_flag = FALSE;
    }
}

// First move of dash is right
void dash_first_move_right(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos
) {
    player_pos->pos_col += 1;
    if (check_off_map(player_pos->pos_row, player_pos->pos_col)) {
        player_pos->pos_col -= 1;
        player_pos->dash_flag = TRUE;
    } else if (check_collision(player_pos->pos_row, 
        player_pos->pos_col, board)) {
        player_pos->pos_col -= 1;
        player_pos->dash_flag = TRUE;
    } else {
        player_pos->dash_flag = FALSE;
    }
}

// The second part of the dash
struct pos second_dash(struct tile board[ROWS][COLS], struct pos player_pos) {
    // Also has to be checked with grid and obstacles
    char command_two;
    scanf("%c", &command_two);
    if (command_two == 'W') {
        player_pos.pos_row -= 1;
        if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
            player_pos.pos_row += 1;
        } else if (check_collision(player_pos.pos_row, 
            player_pos.pos_col, board)) {
            player_pos.pos_row += 1;
        }
    } else if (command_two == 'A') {
        player_pos.pos_col -= 1;
        if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
            player_pos.pos_col += 1;
        } else if (check_collision(player_pos.pos_row, 
            player_pos.pos_col, board)) {
            player_pos.pos_col += 1;
        }
    } else if (command_two == 'S') {
        player_pos.pos_row += 1;
        if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
            player_pos.pos_row -= 1;
        } else if (check_collision(player_pos.pos_row, 
            player_pos.pos_col, board)) {
            player_pos.pos_row -= 1;
        }
    } else if (command_two == 'D') {
        player_pos.pos_col += 1;
        if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
            player_pos.pos_col -= 1;
        } else if (check_collision(player_pos.pos_row, 
            player_pos.pos_col, board)) {
            player_pos.pos_col -= 1;
        }
    } 

    return player_pos;
}

// Checks if first move is invalid 
// so can be used to determine if second move should occur
int dash_first_move_flag(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    char command
) {
    // First move is invalid if it's off the grid
    // or if it hits an obstacle
    int flag = FALSE;
    if (player_pos.dash_flag == TRUE) {
        if (command == 'W') {
            player_pos.pos_row -= 1;
            if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
                flag = TRUE;
            } else if (check_collision(player_pos.pos_row, 
                player_pos.pos_col, board)) {
                flag = TRUE;
            }
            player_pos.pos_row += 1;
        } else if (command == 'A') {
            player_pos.pos_col -= 1;
            if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
                flag = TRUE;
            } else if (check_collision(player_pos.pos_row, 
                player_pos.pos_col, board)) {
                flag = TRUE;
            }
            player_pos.pos_col += 1;
        } else if (command == 'S') {
            player_pos.pos_row += 1;
            if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
                flag = TRUE;
            } else if (check_collision(player_pos.pos_row, 
                player_pos.pos_col, board)) {
                flag = TRUE;
            }
            player_pos.pos_row -= 1;
        } else if (command == 'D') {
            player_pos.pos_col += 1;
            if (check_off_map(player_pos.pos_row, player_pos.pos_col)) {
                flag = TRUE;
            } else if (check_collision(player_pos.pos_row, 
                player_pos.pos_col, board)) {
                flag = TRUE;
            }
            player_pos.pos_col -= 1;
        }  
    } else {
        // Situation where first move of dash wins
        if (board[player_pos.pos_row][player_pos.pos_col].entity 
            == EXIT_UNLOCKED) {
            flag = TRUE;
        } 
    }

    return flag;
}


// Commands for printing points and stats
void command_print_stats(
    struct tile board[ROWS][COLS], 
    char command, 
    int player_score, 
    int initial_collectibles_count, 
    int num_collected,
    int is_game_of_lava,
    int is_lava_seeds
) {
    if (command == 'p') {
        printf("You have %d point(s)!\n", player_score);
    } else if (command == 'm') {
        print_statistics(board, initial_collectibles_count, 
            num_collected, is_game_of_lava, is_lava_seeds);
    } 
}

// Function to handle the boulder turn
void boulder_turn(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    char gravity_dir, 
    int radius
) {
    // Moves boulder depending on gravity direction
    // Correct movement determined in boulder move already
    if (gravity_dir == 'w') {
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                check_and_move_boulder(board, player_pos, row, 
                    col, gravity_dir);
            }
        }
    } else if (gravity_dir == 'a') {
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                check_and_move_boulder(board, player_pos, row, 
                    col, gravity_dir);
            }
        }
    } else if (gravity_dir == 's') {
        // Scans from bottom up so doesn't move boulder again
        // after it moves down one tile
        for (int row = ROWS - 1; row >= 0; row--) {
            for (int col = 0; col < COLS; col++) {
                check_and_move_boulder(board, player_pos, row, 
                    col, gravity_dir);
            }
        }
    } else if (gravity_dir == 'd') {
        // Same here scanning from right to left when gravity is right
        for (int row = 0; row < COLS; row++) {
            for (int col = COLS - 1; col >= 0; col--) {
                check_and_move_boulder(board, player_pos, row, 
                    col, gravity_dir);
            }
        }
    }
    illuminate(board, player_pos, radius);
}

void check_and_move_boulder(
    struct tile board[ROWS][COLS],
    struct pos player_pos, 
    int row,
    int col,
    char gravity_dir
) {
    if (board[row][col].entity == BOULDER ||
        (board[row][col].entity == HIDDEN &&
        board[row][col].original_entity == BOULDER)) {
        boulder_move(board, player_pos, row, 
            col, gravity_dir);
    }    
}

// Function to move a boulder
void boulder_move(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    int boulder_row, 
    int boulder_col,
    char gravity_dir
) {
    // Depending on gravity sets next boulder placement
    int dest_row = boulder_row;
    int dest_col = boulder_col;
    if (gravity_dir == 'w') {
        dest_row -= 1;
    } else if (gravity_dir == 'a') {
        dest_col -= 1;
    } else if (gravity_dir == 's') {
        dest_row += 1;
    } else if (gravity_dir == 'd') {
        dest_col += 1;
    }

    // Moves boulder only if in map and not blocked by obstacle
    // Allow boulders to move onto lava
    if (dest_row >= 0 && 
    dest_row < ROWS && 
    dest_col >= 0 && 
    dest_col < COLS && 
    (board[dest_row][dest_col].entity == EMPTY ||
    (board[dest_row][dest_col].entity == HIDDEN &&
    board[dest_row][dest_col].original_entity == EMPTY))) {
        board[dest_row][dest_col].entity = BOULDER;
        board[boulder_row][boulder_col].entity = EMPTY;
    }
}

// Runs everything up to and including the boulder turn
void run_game_mechanics(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    char gravity_dir, 
    int radius,
    int *num_collected,
    int *player_score,
    int *is_game_of_lava,
    int *is_lava_seeds
) {
    *num_collected += collectibles_collected(board, player_pos);
    *player_score += add_points(board, player_pos, 
        *is_game_of_lava, *is_lava_seeds);
    replace_gem_empty(board, player_pos);
    check_and_unlock_exit(board);
    dirt_to_empty(board, player_pos);
    boulder_turn(board, player_pos, gravity_dir, radius);
}

// Respawns player if allowed, reducing lives each time
int respawn(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    struct pos starting_pos,
    int radius,
    int lives
) {
    lives -= 1;
    if (lives > 0) {
        player_pos = starting_pos;
        printf("Respawning!\n");
        illuminate(board, player_pos, radius);
    }

    return lives;
}

// Ends game and prints blocked respawn comment
void respawn_blocked(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    int radius, 
    int *death_flag, 
    int player_score
) {
    *death_flag = TRUE;
    printf("Respawn blocked! Game over. Final score: %d points.\n", 
        player_score);
    illuminate(board, player_pos, radius);

}

// Checks for death and respawnable
int player_death_check_respawn(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    struct pos starting_pos, 
    int player_score
) {
    if (player_squashed_by_boulder(board, player_pos)) {
        // Saves hidden tiles
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {            
                board[row][col].spare_entity = board[row][col].entity;
            }
        }
        // Resets to original entities for respawnable check
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {            
                restore_original_entity(board, row, col);
            }
        }
        // Different comments for if spawn point is blocked by lava or other, 
        // so different if statements for them
        if (board[starting_pos.pos_row][starting_pos.pos_col].entity == EMPTY &&
            board[starting_pos.pos_row][starting_pos.pos_col].lava_entity 
            != LAVA) {
            return TRUE;
        } else if (board[starting_pos.pos_row][starting_pos.pos_col].lava_entity 
            == LAVA) {
            return BURNT;
        } else {
            return DEAD;
        }
        // After checking respawnable return board back to with hidden tiles.
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {            
                board[row][col].entity = board[row][col].spare_entity;
            }
        }
    } else {
        return FALSE;
    }
}

// Did player get hit by a boulder
int player_squashed_by_boulder(
    struct tile board[ROWS][COLS], 
    struct pos player_pos
) {
    int squashed = FALSE;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < ROWS; col++) {
            // Check occurs if player pos and boulder pos are same
            if (board[row][col].entity == BOULDER && 
                row == player_pos.pos_row &&
                col == player_pos.pos_col) {
                squashed = TRUE;
            } 
        }
    }

    return squashed;
}

// Changes dirt tile to empty (for when player moves)
void dirt_to_empty(struct tile board[ROWS][COLS], struct pos player_pos) {
    if (board[player_pos.pos_row][player_pos.pos_col].entity == DIRT) {
        board[player_pos.pos_row][player_pos.pos_col].entity = EMPTY;
    }
}

// Output for when player quits game
void print_quit_game(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    int lives, 
    int death_flag, 
    char command
) {
    if (board[player_pos.pos_row][player_pos.pos_col].entity != EXIT_UNLOCKED && 
        lives > 0 && death_flag == FALSE && command == 'q') {
        printf("--- Quitting Game ---\n");
    }
}

// Player input for radius and activates/deactivates illumination
int illumination(struct tile board[ROWS][COLS], struct pos player_pos) {
    int radius;
    scanf("%d", &radius);
    if (radius > 0) {
        printf("Illumination Mode: Activated\n");
        illuminate(board, player_pos, radius);
    } else {
        printf("Illumination Mode: Deactivated\n");
        illuminate(board, player_pos, radius);
    }
    return radius;
}

// Illumination! Hides unilluminated tiles
void illuminate(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    int radius
) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            double distance = sqrt(pow(row - player_pos.pos_row, 2) + 
                                   pow(col - player_pos.pos_col, 2));
            // Allows for illumination to be turned off with a
            // non-positive radius
            if (distance > radius && radius > 0) {
                hide_and_save_tiles(board, row, col);
            } else {
                restore_original_entity(board, row, col);
            }
        }
    }
}

// Single player movement
void single_move(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos, 
    char command, 
    int radius,
    int *dash_cooldown
) {
    // After a single move double dash is allowed again
    *dash_cooldown = 0;
    *player_pos = player_movement(board, *player_pos, command);
    // Resets board before illuminating with updated player pos
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {            
            restore_original_entity(board, row, col);
        }
    }
    illuminate(board, *player_pos, radius);
}

// Putting everyting that happens before and after 2nd dash into 1 function
void dash_second_move(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos, 
    char command, 
    int radius
) {
    replace_gem_empty(board, *player_pos);
    check_and_unlock_exit(board);
    if (dash_first_move_flag(board, *player_pos, command)) {
        // voids second dash if first move is not allowed
        second_dash(board, *player_pos);
    } else {
        dirt_to_empty(board, *player_pos);
        *player_pos = second_dash(board, *player_pos);
    }
    // illuminates after possible 2nd dash
    illuminate(board, *player_pos, radius);
}

// Player cannot dash twice in a row
int out_of_breath(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    int dash_cooldown
) {
    printf("You're out of breath! Skipping dash move...\n");
    // Voids second input
    second_dash(board, player_pos);
    dash_cooldown = 0;

    return dash_cooldown;
}

// Gives gravity direction depending on input
char gravity_shift(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    char gravity_dir
) {
    scanf(" %c", &gravity_dir);
    if (gravity_dir == 'w') {
        printf("Gravity now pulls UP!\n");
    } else if (gravity_dir == 'a') {
        printf("Gravity now pulls LEFT!\n");
    } else if (gravity_dir == 's') {
        printf("Gravity now pulls DOWN!\n");
    } else if (gravity_dir == 'd') {
        printf("Gravity now pulls RIGHT!\n");
    }

    return gravity_dir;
}

// Lava turn checks lava death and respawnable
void lava_turn(
    struct tile board[ROWS][COLS],
    struct pos *player_pos,        
    struct pos starting_pos, 
    int *lives,                   
    int *death_flag,              
    int player_score,             
    int radius                   
) {
    if (can_respawn_lava(board, *player_pos, starting_pos, player_score)
        == TRUE) {
        *lives = respawn(board, *player_pos, starting_pos, radius, *lives);
        if (*lives > 0) {
            // respawns player to starting position
            *player_pos = starting_pos;
            illuminate(board, *player_pos, radius);
        }
    } else if (can_respawn_lava(board, *player_pos, starting_pos, player_score)
        == BURNT) {
        // Potential bug: If player dies by lava but respawns into a boulder,
        // "you're toast" statement prints. Assignment instructions did not
        // specity what would happen for this situation so assumed this message
        // is to be printed
        printf("Respawn blocked! You're toast! Final score: %d points.\n", 
            player_score);
        (*lives)--; 
        *death_flag = TRUE;
        illuminate(board, *player_pos, radius);
    }
    
}

// Checks lava death and respawnable
int can_respawn_lava(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    struct pos starting_pos, 
    int player_score
) {
    if (lava_death(board, player_pos)) {
        // Saves hidden tiles
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {            
                board[row][col].spare_entity = board[row][col].entity;
            }
        }
        // Resets to original entities for respawnable check
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {            
                restore_original_entity(board, row, col);
            }
        }
        // If start not blocked then respawnable, if not players dead
        if (board[starting_pos.pos_row][starting_pos.pos_col].entity
             == EMPTY &&
            board[starting_pos.pos_row][starting_pos.pos_col].lava_entity
             != LAVA) {
            return TRUE;
        } else {
            return BURNT;
        }
        // After checking respawnable return board back to with hidden tiles.
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {            
                board[row][col].entity = board[row][col].spare_entity;
            }
        }
    } else {
        return FALSE;
    }
}

// Checks death with lava
int lava_death(
    struct tile board[ROWS][COLS], 
    struct pos player_pos
) {
    int burned = FALSE;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < ROWS; col++) {
            if (board[row][col].lava_entity == LAVA && 
                row == player_pos.pos_row &&
                col == player_pos.pos_col) {
                burned = TRUE;
            } 
        }
    }

    return burned;
}

// Konami code check for the lava games
int lava_code(
    struct tile board[ROWS][COLS], 
    struct pos player_pos,
    char command,
    int round_counter,
    char konami_code[KONAMISIZE]
) {
    // Make an array and every iteration move every element left
    // This is because we only need to check the last given input
    // Previous occurences of konami code do not activate game so are discarded
    for (int i = 0; i < KONAMISIZE - 1; i++) {
        konami_code[i] = konami_code[i + 1];
    }
    konami_code[KONAMISIZE - 1] = command;
    
    // Check for sequences in the last 5 commands
    // Need at least 5 commands to check
    if (round_counter >= 4) {  
        // Checks from 5th last element to last
        int start = KONAMISIZE - 5;
        if (konami_code[start] == 'w' &&
            konami_code[start + 1] == 'd' &&
            konami_code[start + 2] == 's' &&
            konami_code[start + 3] == 'a' &&
            konami_code[start + 4] == 'L') {
            printf("Game Of Lava: Activated\n");
            return GAMEOFLAVA;
        }
        if (konami_code[start] == 'w' &&
            konami_code[start + 1] == 'a' &&
            konami_code[start + 2] == 's' &&
            konami_code[start + 3] == 'd' &&
            konami_code[start + 4] == 'L') {
            printf("Lava Seeds: Activated\n");
            return LAVASEEDS;
        }
    }
    return FALSE;
}
    

void game_of_lava(struct tile board[ROWS][COLS]) {
    // Create a temporary array to store the next state
    enum entity next_lava[ROWS][COLS];
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            next_lava[row][col] = EMPTY;
        }
    }
    // First pass: calculate next state without modifying the board
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int alive_neighbors = count_lava_neighbors(board, row, col);
            apply_game_of_lava_rules(board, next_lava, 
                row, col, alive_neighbors);
        }
    }
    // Second pass: apply the new state to the board
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].lava_entity = next_lava[row][col];
        }
    }
}

void lava_seeds(struct tile board[ROWS][COLS]) {
    // Create a temporary array to store the next state
    enum entity next_lava[ROWS][COLS];
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            next_lava[row][col] = EMPTY;
        }
    }
    // First pass: calculate next state without modifying the board
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int alive_neighbors = count_lava_neighbors(board, row, col);
            apply_lava_seeds_rules(board, next_lava, row, col, alive_neighbors);
        }
    }
    // Second pass: apply the new state to the board
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].lava_entity = next_lava[row][col];
        }
    }
}

//Counts how many lava tiles near a tile, including wrapping around sides
int count_lava_neighbors(struct tile board[ROWS][COLS], int row, int col) {
    int alive_neighbors = 0;
    
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue; 
            }
            // Handles wrapping at edges, 
            // + ROWS and + COLS ensures never goes to negative
            int neighbor_row = (row + i + ROWS) % ROWS;
            int neighbor_col = (col + j + COLS) % COLS;
            
            if (board[neighbor_row][neighbor_col].lava_entity == LAVA) {
                alive_neighbors++;
            }
        }
    }

    return alive_neighbors;
}

// Next board iteration in lava seeds gamemode
void apply_lava_seeds_rules(
    struct tile board[ROWS][COLS], 
    enum entity next_lava[ROWS][COLS],
    int row,
    int col,
    int alive_neighbors
) {
    if (board[row][col].lava_entity == LAVA) {
        // All pre-existing lava cells die
        next_lava[row][col] = EMPTY;
    } else {
        // Birth rule
        if (alive_neighbors == 2) {
            next_lava[row][col] = LAVA;
        }
    }
}

// Next board iteration in game of lava gamemode
void apply_game_of_lava_rules(
    struct tile board[ROWS][COLS], 
    enum entity next_lava[ROWS][COLS],
    int row,
    int col,
    int alive_neighbors
) {
    if (board[row][col].lava_entity == LAVA) {
        // Survival rule
        if (alive_neighbors == 2 || alive_neighbors == 3) {
            next_lava[row][col] = LAVA;
        } else {
            next_lava[row][col] = EMPTY;
        }
    } else {
        // Birth rule
        if (alive_neighbors == 3) {
            next_lava[row][col] = LAVA;
        }
    }
}

// Changes lava mode depending on konami code
void change_lava_mode(
    int lava_mode, 
    int *is_game_of_lava, 
    int *is_lava_seeds, 
    int *round_counter
) {
    if (lava_mode == GAMEOFLAVA) {
        *is_game_of_lava = TRUE;
        // Ensure only one mode is active
        *is_lava_seeds = FALSE;
    } else if (lava_mode == LAVASEEDS) {
        *is_lava_seeds = TRUE;
        *is_game_of_lava = FALSE;
    }
    *round_counter += 1;
}

// Activates/deactivates shadow mode only after 'u' is pressed
void shadow_mode_activation(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos, 
    int *lives,
    int *shadow_mode
) {
    // Change over to other mode depending on what it originally was
    if (*shadow_mode == FALSE) {
        printf("Shadow Mode: Activated\n");
        *shadow_mode = TRUE;
        // Hides tiles that are shadowed
        shadow_tiles(board, *player_pos);
        print_board(board, player_pos->pos_row, player_pos->pos_col, *lives);
    } else {
        printf("Shadow Mode: Deactivated\n");
        *shadow_mode = FALSE;
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                // Changes hidden tiles back to original
                restore_original_entity(board, row, col);
            }
        }
        print_board(board, player_pos->pos_row, player_pos->pos_col, *lives);
    }
}

void shadow_tiles(
    struct tile board[ROWS][COLS], 
    struct pos player_pos
) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            // Skip the player's position
            if (row == player_pos.pos_row && col == player_pos.pos_col) {
                continue;
            }

            int blocked = FALSE;
            // Set horizontal and vertical components of vector
            double horizontal_dist = col - player_pos.pos_col;
            double vertical_dist = row - player_pos.pos_row;

            // Flags for which side is tangential to ray
            int tangent_from_above = FALSE;
            int tangent_from_below = FALSE;

            // Margin of error for tangential ray
            double epsilon = 0.005; 

            ray_sampling(player_pos, horizontal_dist, vertical_dist, board,
                epsilon, &tangent_from_above, &tangent_from_below, &blocked,
                row, col);

            // If the tile is blocked, mark it as hidden
            if (blocked) {
                hide_and_save_tiles(board, row, col);
            }
        }
    }
}

// Samples in tiny increments points on the ray from player
// to target tile
void ray_sampling(
    struct pos player_pos,
    double horizontal_dist,
    double vertical_dist,
    struct tile board[ROWS][COLS], 
    double epsilon,
    int *tangent_from_above,
    int *tangent_from_below,
    int *blocked,
    int row,
    int col
) {
    // Delta is small increments along the line vector
    for (double delta = 0; delta <= 1; delta += 0.0001) {
        // Parametric equation of a line
        double sample_y = player_pos.pos_row + delta * vertical_dist;
        double sample_x = player_pos.pos_col + delta * horizontal_dist;

        // Calculate the coordinates of the sample point  
        int sample_y_cord = (int)(round(sample_y));
        int sample_x_cord = (int)(round(sample_x));

        // Ensure the coordinates are within bounds
        if (check_off_map(sample_y_cord, sample_x_cord)) {
            continue;
        }

        //Skip player position and obstacle position
        if ((sample_y_cord == player_pos.pos_row && 
            sample_x_cord == player_pos.pos_col) || 
            (sample_y_cord == row && sample_x_cord == col)) {
            continue;
        }

        is_tile_shadowed(board, sample_y, sample_x, epsilon, sample_y_cord, 
            sample_x_cord, tangent_from_above, tangent_from_below, blocked);
    }
}

// Determines which tiles to shadow
void is_tile_shadowed(
    struct tile board[ROWS][COLS], 
    double sample_y,
    double sample_x,
    double epsilon,
    int sample_y_cord, 
    int sample_x_cord,
    int *tangent_from_above,
    int *tangent_from_below,
    int *blocked
) {
    if (is_opaque(board[sample_y_cord][sample_x_cord].entity)) {
        // Calculate the local coordinates 
        // (coordinates within the tile) of the sample point
        double local_x = fabs(sample_y - sample_y_cord);
        double local_y = fabs(sample_x - sample_x_cord);
        
        // Check if the sample point lies 
        // close to a vertical or horizontal boundary.
        // Since tile is a 1x1 square, 
        // boundary is ±0.5 units away from the centre.
        int near_vertical_edge = (fabs(local_x - 0.5) < epsilon);
        int near_horizontal_edge = (fabs(local_y - 0.5) < epsilon);
        
        if (near_vertical_edge || near_horizontal_edge) {
            // Corner case: decide if the ray is 
            // partially blocked from above or below.
            // Use the opaque cell's centre to decide.
            double cell_center_y = sample_x_cord;  
            if (sample_x < cell_center_y)
                *tangent_from_above = TRUE;
            else if (sample_x > cell_center_y)
                *tangent_from_below = TRUE;
            
            // If both flags are set, the ray is fully blocked.
            if (tangent_from_above && tangent_from_below) {
                *blocked = TRUE;
            } 
            // Otherwise, continue sampling.
        } else {
            // Shadowed if ray passes directly through obstacle
            *blocked = TRUE;
        }                
    } 
}

// Checks if the entity is an obstacle
int is_opaque(enum entity entity) {
    return (entity == WALL || entity == BOULDER || entity == GEM);
}

// Hides tiles and saves original tile
void hide_and_save_tiles(struct tile board[ROWS][COLS], int row, int col) {
    if (board[row][col].entity != HIDDEN) {
        board[row][col].original_entity = board[row][col].entity;
        board[row][col].entity = HIDDEN;
    }
}

// Unhides tiles
void restore_original_entity(struct tile board[ROWS][COLS], int row, int col) {
    if (board[row][col].entity == HIDDEN) {
        board[row][col].entity = board[row][col].original_entity;
    }
}

// player inputs and runs respective functions 
void commands(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos,         
    int *player_score,             
    int *num_collected,             
    char command,
    int *dash_cooldown,            
    int *lives,                   
    int *radius,                  
    char *gravity_dir,
    int *is_game_of_lava,
    int *is_lava_seeds,
    int *shadow_mode    
) {
    if (single_dash_command(command)) { 
        single_move(board, player_pos, command, *radius, dash_cooldown);
    } else if (double_dash_command(command) && *dash_cooldown == 0) { 
        dash_first_move(board, player_pos, command, dash_cooldown, 
            num_collected, player_score, is_game_of_lava, is_lava_seeds);
        dash_second_move(board, player_pos, command, *radius);
    } else if (command == 'i') {
        *radius = illumination(board, *player_pos);
        print_board(board, player_pos->pos_row, player_pos->pos_col, *lives);
    } else if (command == 'g') {
        *gravity_dir = gravity_shift(board, *player_pos, *gravity_dir);
    } else if (dash_on_cooldown(command, *dash_cooldown)) {
        *dash_cooldown = out_of_breath(board, *player_pos, *dash_cooldown);
    } else if (command == 'u') {
        shadow_mode_activation(board, player_pos, lives, shadow_mode);
    }
}

// Checks death/respawns before lava turn
void respawn_and_lava_turns(
    struct tile board[ROWS][COLS], 
    struct pos *player_pos,
    struct pos starting_pos,          
    int *player_score,             
    int *lives, 
    int *death_flag,                   
    int *radius,                  
    int *is_game_of_lava,
    int *is_lava_seeds
) {
    // If respawnable then respawn then do a lava turn (including lava games)
    if (player_death_check_respawn(board, *player_pos, 
        starting_pos, *player_score) == TRUE) {
        *lives = respawn(board, *player_pos, starting_pos, *radius, *lives);
        if (*lives > 0) {
            *player_pos = starting_pos;
        }
        if (*is_game_of_lava == TRUE) {
            game_of_lava(board);
        } else if (*is_lava_seeds == TRUE) {
            lava_seeds(board);
        }
        lava_turn(board, player_pos, starting_pos, lives, 
            death_flag, *player_score, *radius);

    } else if (player_death_check_respawn(board, *player_pos, 
        starting_pos, *player_score) == DEAD) {
        // If respawn blocked by anything not lava then game over
        (*lives)--;
        respawn_blocked(board, *player_pos, *radius, death_flag, *player_score);

    } else if ((player_death_check_respawn(board, *player_pos, 
        starting_pos, *player_score) == BURNT)) {
        // If respawn blocked by lava then game over
        // prints different game over comment
        printf("Respawn blocked! You're toast! Final score: %d points.\n", 
            *player_score);
        (*lives)--; 
        *death_flag = TRUE;
        illuminate(board, *player_pos, *radius);

    } else {
        // If no death then do lava turn
        if (*is_game_of_lava == TRUE) {
            game_of_lava(board);
        } else if (*is_lava_seeds == TRUE) {
            lava_seeds(board);
        }
        lava_turn(board, player_pos, starting_pos, lives, 
            death_flag, *player_score, *radius);
    }
}

// Print game lost comment and board when game over
void game_lost(
    struct tile board[ROWS][COLS], 
    struct pos player_pos, 
    int radius, 
    int lives,
    int player_score
) {
    if (lives == 0) {
        printf("Game Lost! You scored %d points!\n", player_score);
        illuminate(board, player_pos, radius);
        print_board(board, player_pos.pos_row, player_pos.pos_col, lives);
    }

}


// =============================================================================
// Definitions of Provided Functions
// =============================================================================

// Given a 2D board array, initialise all tile entities to DIRT.
void initialise_board(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].entity = DIRT;
            board[row][col].original_entity = DIRT;
            board[row][col].spare_entity = DIRT;
            board[row][col].lava_entity = EMPTY;
        }
    }
}

// Prints the game board, showing the player's position and lives remaining
void print_board(
    struct tile board[ROWS][COLS], 
    int player_row, 
    int player_col,
    int lives_remaining
) {
    print_board_line();
    print_board_header(lives_remaining);
    print_board_line();
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            printf("|");
            if (row == player_row && col == player_col) {
                printf("^_^");
            } else if (board[row][col].lava_entity == LAVA) {
                printf("^^^");
            } else if (board[row][col].entity == EMPTY) {
                printf("   ");
            } else if (board[row][col].entity == DIRT) {
                printf(" . ");
            } else if (board[row][col].entity == WALL) {
                printf("|||");
            } else if (board[row][col].entity == BOULDER) {
                printf("(O)");
            } else if (board[row][col].entity == GEM) {
                printf("*^*");
            } else if (board[row][col].entity == EXIT_LOCKED) {
                printf("[X]");
            } else if (board[row][col].entity == EXIT_UNLOCKED) {
                printf("[ ]");
            } else if (board[row][col].entity == HIDDEN) {
                printf(" X ");
            } else {
                printf("   ");
            }
        }
        printf("|\n");
        print_board_line();
    }
    printf("\n");
    return;
}

// Helper function for print_board(). You will not need to call this.
void print_board_header(int lives) {
    printf("| Lives: %d    C A V E R U N             |\n", lives);
}

// Helper function for print_board(). You will not need to call this.
void print_board_line(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("---+");
    }
    printf("\n");
}

// Prints game statistics: tile types, completion %, and points remaining.
void print_map_statistics(
    int number_of_dirt_tiles,
    int number_of_gem_tiles,
    int number_of_boulder_tiles,
    double completion_percentage,
    int maximum_points_remaining
) {
    printf("========= Map Statistics =========\n");
    printf("Tiles Remaining by Type:\n");
    printf("  - DIRT:      %d\n", number_of_dirt_tiles);
    printf("  - GEMS:      %d\n", number_of_gem_tiles);
    printf("  - BOULDERS:  %d\n", number_of_boulder_tiles);
    printf("Completion Status:\n");
    printf("  - Collectible Completion: %.1f%%\n", completion_percentage);
    printf("  - Maximum Points Remaining: %d\n", maximum_points_remaining);
    printf("==================================\n");
}
