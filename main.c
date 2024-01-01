
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <conio.h>

    #define _ARROW_KEY 224
    #define _LEFT_ARROW 75
    #define _DOWN_ARROW 80
    #define _RIGHT_ARROW 77
    #define _UP_ARROW 72
    #define _ESC_KEY 27
    #define _CTRL_C_KEY 3

    #define CLEAR_SCREEN() system("cls")
#endif 


// TODO: Also add linux compatibility
#ifdef linux
    #define CLEAR_SCREEN() system("clear")
#endif


#define MAX_USERNAME 20
#define MAX_PASSWORD 20
#define MAX_PATH_LEN 200
#define MAX_MAP_NAME 10
#define NUM_OF_TOP_SCORES 5


#define HIDE_CURSOR() printf("\033[?25l")       // Hide console cursor.
#define SHOW_CURSOR() printf("\033[?25h")       // Show console cursor.
#define MIN_INT(a, b) (((a) < (b)) ? (a) : (b)) // Return the minimum of a and b
#define MAX_INT(a, b) (((a) > (b)) ? (a) : (b)) // Return the maximum of a and b


// Struct to hold player password and username.
typedef struct player
{
    char *username, *password;
}
PLAYER;


// Struct to represent game maps. Contains height and width of the map,
// along with its 2D-Array representation. 'player_i' and 'player_j' also represents
// the current location of the player.
typedef struct maze_map 
{
    int height, width;
    int player_i, player_j;
    char **map_array;
} 
MAZE_MAP;


void login_screen_io(char *username, char *password, int *new_login);
PLAYER *read_credentials_file(int *number_of_accounts);
void create_new_account(char *username, char *password);
void print_credentials_debug(PLAYER *credentials, int number_of_accounts);
void free_credentials(PLAYER *credentials, int number_of_accounts);
int search_username(PLAYER *credentials, int number_of_accounts, char *username);
int check_password(PLAYER *credentials, int number_of_accounts, int username_index, char *password);
void new_account_io(char *new_username, char *new_password);
char *login_to_account_wrapper_io();

void print_map_data(MAZE_MAP map);
int import_map_from_file(const char *path, MAZE_MAP *new_map);
int get_number_of_maps();
void save_new_map(MAZE_MAP new_map);
void read_maps(MAZE_MAP *maps);
void print_single_map(MAZE_MAP map);
void print_map_character(char ch);
void print_maps(MAZE_MAP *maps, int number_of_maps);
void free_map(MAZE_MAP map);
void free_maps_array(MAZE_MAP *maps, int number_of_maps);
MAZE_MAP initialize_new_map(int height, int width, int player_i, int player_j);

void update_player_location(int new_player_i, int new_player_j, MAZE_MAP *map);
void print_inventory(char *inventory, int inventory_length);
int check_map_boundaries(int i, int j, MAZE_MAP map);
int main_game_loop(MAZE_MAP map, int total_time, int *seconds_elapsed);
int get_number_of_antimatter(char *inventory, int inventory_length);
void print_game_over_screen(int status, int seconds_elapsed, int total_time);
int calculate_total_seconds(MAZE_MAP map);

int calculate_score(int status, int seconds_elapsed, int total_time);
void read_top_scores(int *top_scores, char **top_score_usernames);
int *init_top_scores_array();
char **init_top_score_usernames_array();
int compare_new_score(int *top_scores, int score);
void update_top_scores(int *top_scores, char **top_score_usernames, int new_score, char *username);
void print_scores(int *top_scores, char **top_score_usernames);
void free_score_arrays(int *top_scores, char **top_score_usernames);

char main_menu_io();
void print_help_tutorial();
void wait_prompt();


int main()
{
    char ch, *username = NULL;

    do {
        CLEAR_SCREEN();
        
        ch = main_menu_io();
        switch (ch)
        {
            case '1':
                username = login_to_account_wrapper_io();
                break;
            case '2':
        }
    }

    return 0;
}


// Print MAZE_MAP data other than the 2D-Array. It includes height, width, and
// player indexes, respectively. Use 'print_single_map' and 'print_maps' functions
// to print the 2D-Arrays.
void print_map_data(MAZE_MAP map)
{
    printf("%d, %d, %d, %d\n", map.height, map.width, map.player_i, map.player_j);
}


// Allocate memory for 2D-Array. Define map height and width. Define player indexes.
MAZE_MAP initialize_new_map(int height, int width, int player_i, int player_j)
{
    int i;
    MAZE_MAP map;

    map.height = height;
    map.width  = width;
    map.player_i = player_i;
    map.player_j = player_j;

    map.map_array = (char **) calloc(map.height, sizeof(char *));
    for (i = 0; i < map.height; i++) 
        map.map_array[i] = (char *) calloc(map.width, sizeof(char));

    return map;
}


// Maps are read from a file called 'maps.bin'. To play with a new map, one should first
// import it. Reads the file of the map from the given path, and returns the map as the 
// MAZE_MAP struct. To save the map for future use, please use 'save_new_map' function.
int import_map_from_file(const char *path, MAZE_MAP *new_map)
{
    FILE *new_map_file_ptr = fopen(path, "r");
    int i, j;
    char ch;


    if (new_map_file_ptr == NULL) 
    {
        printf("Unable to open file '%s'.\n", path);
        return 0;
    }


    // Get map height and width.
    i = j = 0;
    while ((ch = fgetc(new_map_file_ptr)) != EOF)
    {
        if (ch == '\n') 
        {
            i++;
            j = 0;
        }
        else if (ch != ' ') j++;
    }
    rewind(new_map_file_ptr);


    // Initialize new map.
    *new_map = initialize_new_map(i+1, j, 0, 0);


    // Read characters from the map file and transform it into an array.
    i = j = 0;
    while ((ch = fgetc(new_map_file_ptr)) != EOF)
    {
        if (ch == '\n') 
        {
            i++;
            j = 0;
        }
        else if (ch != ' ') 
        {
            // TODO: MIGHT CHANGE IN FUTURE
            if (ch == 'X' || ch == 'G')
            {
                new_map->player_i = i;
                new_map->player_j = j;
                new_map->map_array[i][j] = 'X';
            }
            else 
                new_map->map_array[i][j] = ch;
            j++;
        }
    }


    // Close file and return new MAZE_MAP.
    fclose(new_map_file_ptr);
    return 1;
}


int get_number_of_maps()
{
    int number_of_maps;
    FILE *maps_ptr = fopen("maps.bin", "rb");

    if (maps_ptr == NULL)
    {
        return 0;
    }

    fread(&number_of_maps, sizeof(int), 1, maps_ptr);
    fclose(maps_ptr);

    return number_of_maps;
}

// Save a MAZE_MAP (possibly a new, imported map) to 'maps.bin'.
// Create 'maps.bin' if it doesn't exist.
void save_new_map(MAZE_MAP new_map)
{
    int i, number_of_maps = 0;
    FILE *maps_ptr = fopen("maps.bin", "r+b");

    if (maps_ptr == NULL)
    {
        // Create the file if it does not exist.
        // 'maps.bin' consists of a number at the top, indicating number of maps saved.
        // And after a new line, lines that hold information about the maps. 
        maps_ptr = fopen("maps.bin", "wb");
        fwrite(&number_of_maps, sizeof(int), 1, maps_ptr);
        fclose(maps_ptr);

        maps_ptr = fopen("maps.bin", "r+b");
    }

    
    // Update number of maps in the file.
    fread(&number_of_maps, sizeof(int), 1, maps_ptr);
    rewind(maps_ptr);
    number_of_maps++;
    fwrite(&number_of_maps, sizeof(int), 1, maps_ptr);

    fclose(maps_ptr);


    // Save MAZE_MAP struct by appending it to the 'maps.bin' file.
    maps_ptr = fopen("maps.bin", "ab");

    fwrite(&new_map.height, sizeof(int), 1, maps_ptr);
    fwrite(&new_map.width, sizeof(int), 1, maps_ptr);
    fwrite(&new_map.player_i, sizeof(int), 1, maps_ptr);
    fwrite(&new_map.player_j, sizeof(int), 1, maps_ptr);

    for (i = 0; i < new_map.height; i++)
        fwrite(new_map.map_array[i], sizeof(char), new_map.width, maps_ptr);

    fclose(maps_ptr);
}


// Read maps from 'maps.bin'. Copies them to a MAZE_MAP array. 
void read_maps(MAZE_MAP *maps)
{
    int i, h, number_of_maps;
    FILE *maps_ptr = fopen("maps.bin", "rb");

    if (maps_ptr == NULL)
    {
        puts("Unable to access 'maps.bin' file.");
        return;
    }

    // Get number of maps.
    fread(&number_of_maps, sizeof(int), 1, maps_ptr);

    // Read the map structures. 
    for (i = 0; i < number_of_maps; i++)
    {
        fread(&maps[i].height, sizeof(int), 1, maps_ptr);
        fread(&maps[i].width, sizeof(int), 1, maps_ptr);
        fread(&maps[i].player_i, sizeof(int), 1, maps_ptr);
        fread(&maps[i].player_j, sizeof(int), 1, maps_ptr);
        
        maps[i] = initialize_new_map(maps[i].height, maps[i].width, maps[i].player_i, maps[i].player_j);    
        for (h = 0; h < maps[i].height; h++) {
            fread(maps[i].map_array[h], sizeof(char), maps[i].width, maps_ptr);
        }
    }

    fclose(maps_ptr);
}


// Print 2D-Array of a single map.
void print_single_map(MAZE_MAP map)
{
    int i, j;

    for (i = 0; i < map.height; i++) 
    {
        for (j = 0; j < map.width; j++)
        {
            print_map_character(map.map_array[i][j]);
        }
        puts("");
    }
}


// Print map representation of a char. If it doesn't have
// a special representation, print the char itself. 
void print_map_character(char ch)
{
    switch (ch)
    {
        case 'P': printf("P+ "); break;
        case 'p': printf("P- "); break;
        case 'E': printf("e+ "); break;
        case 'e': printf("e- "); break;
        default: printf("%c  ", ch);
    }
}


// Print multiple maps from a MAZE_MAP array.
void print_maps(MAZE_MAP *maps, int number_of_maps)
{
    int i; 

    puts("PRINTING MAZE MAPS\n");

    for (i = 0; i < number_of_maps; i++)
    {
        printf("MAP NUMBER: %d\n", i);
        print_single_map(maps[i]);
        puts("");
    }
}


// Free MAZE_MAP array.
void free_map(MAZE_MAP map)
{
    int i;
    for (i = 0; i < map.height; i++) free(map.map_array[i]);
    free(map.map_array);
}


// Free arrays of the maps from a MAZE_MAP array.
void free_maps_array(MAZE_MAP *maps, int number_of_maps)
{
    int i;
    for (i = 0; i < number_of_maps; i++) free_map(maps[i]);
}


// Print login screen and read user credentials. new_login = 0.
// If no username and password given, ask to create a new account. new_login = 1.
// If user denies creating a new account, new_login = 2.
void login_screen_io(char *username, char *password, int *new_login)
{
    *new_login = 0;

    // Ask username and password.
    printf("Username: ");
    fgets(username, MAX_USERNAME, stdin);
    printf("Password: ");
    fgets(password, MAX_PASSWORD, stdin);

    username[strcspn(username, "\n")] = '\0';
    password[strcspn(password, "\n")] = '\0';

    // When no username and password given, ask user to create
    // a new account.
    if (strlen(username) == 0 && strlen(password) == 0)
    {
        printf("New account? (y/n) ");
        if (getc(stdin) == 'y') 
        {
            *new_login = 1;
        }
        else 
        {
            *new_login = 2;
        }
        getc(stdin); // Prevent remaining newline cause problems. 
    }
}


// Read account information from credentials file 'credentials_file_ptr'. 
// Allocate space for an array to store accounts.
PLAYER *read_credentials_file(int *number_of_accounts)
{
    int i = 0;
    PLAYER *credentials;
    FILE *credentials_file_ptr;

    credentials_file_ptr = fopen("login.dat", "rb");
    if (credentials_file_ptr == NULL)
    {
        // Create the file if it does not exist.
        credentials_file_ptr = fopen("login.dat", "wb");
        fwrite(&i, sizeof(int), 1, credentials_file_ptr);
        fclose(credentials_file_ptr);

        credentials_file_ptr = fopen("login.dat", "rb");
    }

    // Read number of accounts.
    fread(number_of_accounts, sizeof(int), 1, credentials_file_ptr);

    // Allocate space for credentials array. Then read account data.
    credentials = (PLAYER *) calloc(*number_of_accounts, sizeof(PLAYER));
    if (credentials == NULL)
    {
        puts("Not enough memory, exiting...");
        return NULL;
    }
    for (i = 0; i < *number_of_accounts; i++) 
    {
        credentials[i].username = (char *) calloc(MAX_USERNAME, sizeof(char));
        credentials[i].password = (char *) calloc(MAX_PASSWORD, sizeof(char));

        fread(credentials[i].username, sizeof(char), MAX_USERNAME, credentials_file_ptr);
        fread(credentials[i].password, sizeof(char), MAX_PASSWORD, credentials_file_ptr);
    }

    // Return accounts.
    fclose(credentials_file_ptr);
    return credentials;
}


// Create a new account by appending to 'login.dat' file. 
void create_new_account(char *username, char *password)
{
    int number_of_accounts;
    FILE *credentials_file_ptr;

    // Update number of accounts.
    credentials_file_ptr = fopen("login.dat", "r+b");

    fread(&number_of_accounts, sizeof(int), 1, credentials_file_ptr);
    rewind(credentials_file_ptr);
    number_of_accounts++;
    fwrite(&number_of_accounts, sizeof(int), 1, credentials_file_ptr);

    fclose(credentials_file_ptr);


    // Add new account.
    credentials_file_ptr = fopen("login.dat", "ab");

    fwrite(username, sizeof(char), MAX_USERNAME, credentials_file_ptr);
    fwrite(password, sizeof(char), MAX_PASSWORD, credentials_file_ptr);

    fclose(credentials_file_ptr);
}


// Print account information. Only for DEBUG purpose.
void print_credentials_debug(PLAYER *credentials, int number_of_accounts)
{
    int i;

    puts("Printing accounts:");
    for (i = 0; i < number_of_accounts; i++)
    {
        printf("%d %s %s\n", i+1, credentials[i].username, credentials[i].password);
    }
    puts("Printing accounts complete...");
}


// Free credentials array.
void free_credentials(PLAYER *credentials, int number_of_accounts)
{
    int i;

    for (i = 0; i < number_of_accounts; i++)
    {
        free(credentials[i].username);
        free(credentials[i].password);
    } 
    free(credentials);
}


// Search the credentials array. If username found, return its index.
// Otherwise return -1.
int search_username(PLAYER *credentials, int number_of_accounts, char *username)
{
    int i = 0, is_found = 0;


    // Search trough the credentials array.
    while ((i < number_of_accounts) && (is_found == 0))
    {
        if (strcmp(credentials[i].username, username) == 0)
        {
            is_found = 1;
        }

        i++;
    }


    // If username found, return its index. Otherwise return -1.
    if (is_found)
    {
        return i-1;
    }
    else
    {
        return -1;
    }
}


// If passwords match, return 1. If not, return 0.
int check_password(PLAYER *credentials, int number_of_accounts, int username_index, char *password)
{
    int i = username_index;

    if (strcmp(credentials[i].password, password) == 0)
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}


// Read new username and password from user to create new account.
void new_account_io(char *new_username, char *new_password)
{
    printf("New username: ");
    fgets(new_username, MAX_USERNAME, stdin);
    printf("New password: ");
    fgets(new_password, MAX_PASSWORD, stdin);

    new_username[strcspn(new_username, "\n")] = '\0';
    new_password[strcspn(new_password, "\n")] = '\0';
}


// Ask user to enter account informations. Warn user when entered wrong/incomplete information. 
// Create new account for user. Modify accounts file according to operations.
// Overall, wrapper for account and login operations.
char *login_to_account_wrapper_io()
{
    char *username = calloc(MAX_USERNAME, sizeof(char));
    char *password = calloc(MAX_PASSWORD, sizeof(char));
    PLAYER *credentials;
    int new_login, number_of_accounts, login_successful = 0;
    int username_index;


    // Read credentials file.
    credentials = read_credentials_file(&number_of_accounts);


    // Login loop. Repeat until user manages to login with either an existing account or by
    // creating a new account. New accounts get saved to 'login.dat' file.
    while (!login_successful)
    {
        // When no username and password given, that means user wants to create a new account.
        login_screen_io(username, password, &new_login);

        // Search the username and check the password. If both are correct, exit the login loop.
        if (new_login == 0)
        {
            username_index = search_username(credentials, number_of_accounts, username);
            if (username_index != -1)
            {
                if (check_password(credentials, number_of_accounts, username_index, password))
                {
                    login_successful = 1;
                }
                else
                {
                    puts("Wrong password. Try again.");
                }
            }
            else
            {
                puts("Username not found.");
            }
        }
        else if(new_login == 1)
        {
            // Creat a new account for the user.
            while (!login_successful) 
            {
                // Ask for a new username and password.
                new_account_io(username, password);

                // Search username of the user and save as the new account if it's unique.
                if (search_username(credentials, number_of_accounts, username) == -1)
                {
                    puts("Creating new account...");
                    create_new_account(username, password);
                    login_successful = 1;
                }
                else 
                {
                    puts("Username exists. Please select another username.");
                }
            }
        }
        else 
        {
            // Return NULL to indicate exit.
            puts("Exiting...");
            return NULL;
        }
    }
    puts("Login successful.");


    // Close the file and free allocated arrays.
    free_credentials(credentials, number_of_accounts);
    free(password);


    // Return the username of user.
    return username;
}


int main_game_loop(MAZE_MAP map, int total_time, int *seconds_elapsed)
{
    int game_running = 1, game_complete = 0, esc_pressed = 0, game_lost = 0;
    int new_player_i, new_player_j;
    int inventory_index = 0;
    int number_of_antimatter;
    char *inventory = calloc((map.height * map.width), sizeof(char));
    char new_block;
    int ch;


    // TODO: clock and score calculation

    CLEAR_SCREEN();
    print_single_map(map);
    print_inventory(inventory, inventory_index);

    while (game_running)
    {
        // TODO: Use thread for user input.
        // TODO: Use thread for time.

        ch = _getch();
        if (ch == _ARROW_KEY) 
        {
            new_player_i = map.player_i;
            new_player_j = map.player_j;

            ch = _getch();
            switch (ch)
            {
                case _LEFT_ARROW: 
                    new_player_j--;
                    break;
                case _RIGHT_ARROW: 
                    new_player_j++;
                    break;
                case _UP_ARROW: 
                    new_player_i--;
                    break;
                case _DOWN_ARROW: 
                    new_player_i++;
                    break;                                                 
            }

            new_block = map.map_array[new_player_i][new_player_j];
            if (new_block != '1')
            {
                update_player_location(new_player_i, new_player_j, &map);
                if (new_block == 'C') 
                {
                    game_running = 0;
                    game_complete = 1;
                }
                else if (new_block == 'K')
                {
                    game_running = 0;
                    game_lost = 1;     
                }
                else if (new_block != '0')
                    inventory[inventory_index++] = new_block;
            }

            CLEAR_SCREEN();
            print_single_map(map);
            print_inventory(inventory, inventory_index);
        }
        else if (ch == _ESC_KEY)
        {
            game_running = 0;
            esc_pressed = 1;
        }
    }

    number_of_antimatter = get_number_of_antimatter(inventory, inventory_index);
    free(inventory);

    if (game_complete)
        return number_of_antimatter;
    if (game_lost)
        return -1;
    if (esc_pressed)
        return -2;
    // TODO: ADD TIMEOUT 'return -3'


    // Return '-4' when exiting in an unexpected way.
    return -4;
}


// Return 0 if given indexes are out of map boundaries. 
// Return 1 otherwise.
int check_map_boundaries(int i, int j, MAZE_MAP map)
{

    if ((i < 0 || i >= map.height) || (j < 0 || j >= map.width))
        return 0;
    return 1;
}


// Update player indexes (location) on map. Only applicable when new indexes are 
// not out of the map boundaries.
void update_player_location(int new_player_i, int new_player_j, MAZE_MAP *map)
{
    if (check_map_boundaries(new_player_i, new_player_j, *map)) 
    {
        map->map_array[map->player_i][map->player_j] = '0';
        map->player_i = new_player_i;
        map->player_j = new_player_j;
        map->map_array[map->player_i][map->player_j] = 'X';
    }
}


// Print the inventory array and items in it.
void print_inventory(char *inventory, int inventory_length)
{
    int i;
    puts("Collected sub-atomic particles:");
    for (i = 0; i < inventory_length; i++)
    {
        print_map_character(inventory[i]);
        printf(" ");
    }
}


int get_number_of_antimatter(char *inventory, int inventory_length)
{
    int i, num_of_p_minus = 0, num_of_e_plus = 0;


    for (i = 0; i < inventory_length; i++)
    {
        switch (inventory[i])
        {
            case 'P': 
                num_of_p_minus--;
                break;
            case 'p':
                num_of_p_minus++;
                break;
            case 'E':
                num_of_e_plus++;
                break;
            case 'e':
                num_of_e_plus--;
                break;
        }
    }

    num_of_p_minus = MAX_INT(num_of_p_minus, 0);
    num_of_e_plus = MAX_INT(num_of_e_plus, 0);
    return MIN_INT(num_of_p_minus, num_of_e_plus);
}


void print_game_over_screen(int status, int seconds_elapsed, int score)
{
    CLEAR_SCREEN();
    if (status >= 0)        // Game complete.
    {
        printf("Congratulation! You managed to finish the game.\n");
        printf("Seconds elapsed: %d\n", seconds_elapsed);
        printf("Number of antimatter atoms produced: %d\n", status);
        printf("Score: %d\n", score);
    }
    else if (status == -1)  // Game lost.
    {
        printf("Game over.\nYou ran into a black hole.\nNo one really knows what happened to you.\n");
    }
    else if (status == -2); // ESC pressed to exit.
    else if (status == -3)  // Timeout.
    {
        printf("Game over. You are too slow.\nMicro world is way faster than this.\n");
    }
}


// Return how many seconds the player will have to finish the game.
int calculate_total_seconds(MAZE_MAP map)
{
    return (map.height * map.width);
}


// Calculate the score of the victor player.
int calculate_score(int status, int seconds_elapsed, int total_time)
{
    return (status + 1) * (total_time - seconds_elapsed);
}


// Try to read top scores from file. Save scores and usernames.
void read_top_scores(int *top_scores, char **top_score_usernames)
{
    int i;
    FILE *scores_file_ptr = fopen("scores.txt", "r");

    if (scores_file_ptr != NULL)
    {
        i = 0;
        while (!feof(scores_file_ptr))
        {   
            // Read scores until EOF.
            fscanf(scores_file_ptr, "%s %d", top_score_usernames[i], &top_scores[i]);
            i++;
        }
        fclose(scores_file_ptr);
    }
}


int *init_top_scores_array()
{
    return (int *) calloc(NUM_OF_TOP_SCORES, sizeof(int));
}


char **init_top_score_usernames_array()
{
    int i;
    char **top_score_usernames = (char **) calloc(NUM_OF_TOP_SCORES, sizeof(char *));
    for (i = 0; i < NUM_OF_TOP_SCORES; i++) 
    {
        top_score_usernames[i] = (char *) calloc(MAX_USERNAME, sizeof(char));
        top_score_usernames[i][0] = '\0';
    }
    return top_score_usernames;
}


// Return a possible index for new score. Compare this score with  
// top scores and decide its place.
int compare_new_score(int *top_scores, int new_score)
{
    int i = 0;
    while (top_scores[i] > new_score) i++;
    return i;
}


// Update 'scores.txt' based on the value of new score. Insert it to correct place
// if it is a new top score. 
void update_top_scores(int *top_scores, char **top_score_usernames, int new_score, char *username)
{
    int i = 0, n = NUM_OF_TOP_SCORES;
    int new_score_index = compare_new_score(top_scores, new_score);
    FILE *scores_file_ptr = fopen("scores.txt", "w");

    while ((i < n) && (top_scores[i] != 0))
    {
        if (new_score_index == i)
        {
            fprintf(scores_file_ptr, "%s %d\n", username, new_score);
            n--;
        }
        fprintf(scores_file_ptr, "%s %d\n", top_score_usernames[i], top_scores[i]);
        i++;
    }

    if ((i < n) && (top_scores[i] == 0))
        fprintf(scores_file_ptr, "%s %d\n", username, new_score);

    fclose(scores_file_ptr);
}


// Print top scores as a list. 
void print_scores(int *top_scores, char **top_score_usernames)
{
    int i = 0;

    CLEAR_SCREEN();

    puts("TOP SCORES\n");
    while ((i < NUM_OF_TOP_SCORES) && (top_scores[i] != 0))
    {
        printf("%s %d\n", top_score_usernames[i], top_scores[i]);
        i++;
    }
}


// Free score arrays.
void free_score_arrays(int *top_scores, char **top_score_usernames)
{
    int i;

    free(top_scores);
    for (i = 0; i < NUM_OF_TOP_SCORES; i++)
        free(top_score_usernames[i]);
    free(top_score_usernames);
}


char main_menu_io()
{
    char ch;

    CLEAR_SCREEN();
    puts("1 - Login and Register");
    puts("2 - Top Scores");
    puts("3 - Help and Tutorial");
    puts("4 - Play in Manual Mode");
    puts("5 - Play in Automatic Mode (WIP)");
    puts("0 - Exit");
    printf("Enter option: ");

    scanf("%c", &ch);
    fgetc(stdin);
    return ch;
}


void print_help_tutorial()
{
    CLEAR_SCREEN();
    
    puts("\nTHE GAME TUTORIAL");
    puts("Hello new player! I know you will not read the following text,");
    puts("but lets start. This game's name is Anti-Maze. In this game,");
    puts("you are collecting subatomic particles to produce antimatter.");
    puts("The more antimatter you produce, the better score you will get.");
    puts("You move with arrow keys. The map consists of several characters.");
    puts("'0' is empty space, you can walk trough it. '1' is wall, you cannot");
    puts("pass it. 'X' is you. 'K' is micro black hole, moving into in accidentally");
    puts("will result with a lost. 'C' is exit. You should reach 'C' to win");
    puts("the game. 'e-', 'e+', 'P-', 'P+' are subatomic particles. Electron, positron");
    puts("(antielectron), antiproton, proton, respectively. To produce an");
    puts("antimatter atom, you need an antiproton and a positron. But keep");
    puts("in mind, that particle-antiparticle pairs will annihilate each other.");
    puts("For each map, there is a finite amount of time to complete the game.");
    puts("The amount of time is different for different maps. If you couldn't complete");
    puts("a game (reach 'C') in time, you lose. Faster you complete the game, better");
    puts("the score you get. That's all, have fun!");

    puts("\nACCOUNT SYSTEM");
    puts("To start playing, you first need to login. If you don't have an account,");
    puts("you can simply create a new one. Select the option 'Login and Register'");
    puts("but leave username and password fields empty. The game will then ask");
    puts("you about creating a new account. If you enter 'y', the game will prompt");
    puts("you about a new username and password. If you enter something else, the");
    puts("game will exit. After logging in, you can select the");
    puts("'Play in Manual Mode' option, and start the game. Automatic mode doesn't");
    puts("require an account, because it doesn't save scores.");

    puts("\nMAPS (WIP)");
    puts("...");

    wait_prompt();
}


void wait_prompt()
{
    puts("Press ENTER to continue...");
    fgetc(stdin);
}


// TODO: auto-play mode
