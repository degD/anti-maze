
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_USERNAME 20
#define MAX_PASSWORD 20
#define MAX_PATH_LEN 200
#define MAX_MAP_NAME 10


// TODO: user account struct


typedef struct maze_map 
{
    int height, width;
    char **map_array;
} 
MAZE_MAP;


void login_screen_io(char *username, char *password, int *new_login);
char ***read_credentials_file(int *number_of_accounts, FILE *credentials_file_ptr);
void create_new_account(char *username, char *password, FILE *credentials_file_ptr);
FILE *open_credentials_file();
void print_credentials_debug(char ***credentials, int number_of_accounts);
void free_credentials(char ***credentials, int number_of_accounts);
int search_username(char ***credentials, int number_of_accounts, char *username);
int check_password(char ***credentials, int number_of_accounts, int username_index, char *password);
void new_account_io(char *new_username, char *new_password);
char *login_to_account_wrapper_io();

int import_map_from_file(char *path, MAZE_MAP *new_map);
int get_number_of_maps();
void save_new_map(MAZE_MAP new_map);
void read_maps(MAZE_MAP *maps);
void print_single_map(MAZE_MAP map);
void print_maps(MAZE_MAP *maps, int number_of_maps);
void free_map(MAZE_MAP map);
void free_maps_array(MAZE_MAP *maps, int number_of_maps);
MAZE_MAP initialize_new_map(int height, int width);


int main()
{
    int n;
    MAZE_MAP m[10], t;

    puts("TEST FOR READING MAP FILES...");
    printf("Currently, there are %d maps.\n", get_number_of_maps());

    // import_map_from_file("examplemap1.dat", &t);
    // save_new_map(t);

    n = get_number_of_maps();
    read_maps(m);
    puts("Read maps.dat file.");
    print_single_map(m[0]);

    return 0;
}


MAZE_MAP initialize_new_map(int height, int width)
{
    int i;
    MAZE_MAP map;

    map.height = height;
    map.width  = width;

    map.map_array = (char **) calloc(map.height, sizeof(char *));
    for (i = 0; i < map.height; i++) 
        map.map_array[i] = (char *) calloc(map.width, sizeof(char));

    return map;
}


int import_map_from_file(char *path, MAZE_MAP *new_map)
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
    *new_map = initialize_new_map(i+1, j);


    // Read characters from the map and convert it to the array.
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
    int number_of_maps = 0;
    FILE *maps_ptr = fopen("maps.dat", "rb");

    if (maps_ptr != NULL)
    {
        fscanf(maps_ptr, "%d ", &number_of_maps);
        fclose(maps_ptr);
        return number_of_maps;
    }
    fclose(maps_ptr);
    return 0;
}


void save_new_map(MAZE_MAP new_map)
{
    int i, number_of_maps;
    FILE *maps_ptr = fopen("maps.dat", "r+b");

    if (maps_ptr == NULL)
    {
        // Create the file if it does not exist.
        maps_ptr = fopen("maps.dat", "w");
        fputs("0\n", maps_ptr);
        fclose(maps_ptr);

        maps_ptr = fopen("maps.dat", "r+b");
    }

    
    number_of_maps = get_number_of_maps();
    fprintf(maps_ptr, "%d\n", number_of_maps+1);
    fclose(maps_ptr);


    // Saving MAZE_MAP struct by appending to 'maps.dat' file.
    maps_ptr = fopen("maps.dat", "ab");
    fwrite(&new_map, sizeof(MAZE_MAP), 1, maps_ptr);
    for (i = 0; i < new_map.height; i++)
        fwrite(new_map.map_array[i], sizeof(char), new_map.width, maps_ptr);

    fclose(maps_ptr);
}


void read_maps(MAZE_MAP *maps)
{
    int i, h, number_of_maps;
    FILE *maps_ptr = fopen("maps.dat", "rb");

    if (maps_ptr == NULL)
    {
        puts("Unable to access 'maps.dat' file.");
        return;
    }


    fscanf(maps_ptr, "%d ", &number_of_maps);
    for (i = 0; i < number_of_maps; i++)
    {
        fread(&maps[i], sizeof(MAZE_MAP), 1, maps_ptr);
        
        maps[i] = initialize_new_map(maps[i].height, maps[i].width);    
        for (h = 0; h < maps[i].height; h++) {
            fread(maps[i].map_array[h], sizeof(char), maps[i].width, maps_ptr);
        }
    }

    fclose(maps_ptr);
}


void print_single_map(MAZE_MAP map)
{
    int i, j;

    for (i = 0; i < map.height; i++) 
    {
        for (j = 0; j < map.width; j++)
        {
            switch (map.map_array[i][j])
            {
                case 'P': printf("P+ "); break;
                case 'p': printf("P- "); break;
                case 'E': printf("e+ "); break;
                case 'e': printf("e- "); break;
                default: printf("%c  ", map.map_array[i][j]);
            }
        }
        puts("");
    }
}


void print_maps(MAZE_MAP *maps, int number_of_maps)
{
    int i; 

    puts("PRINTING MAZE MAPS\n\n");

    for (i = 0; i < number_of_maps; i++)
    {
        printf("MAP NUMBER: %d\n", i);
        print_single_map(maps[i]);
        puts("");
    }
}


void free_map(MAZE_MAP map)
{
    int i;
    for (i = 0; i < map.height; i++) free(map.map_array[i]);
    free(map.map_array);
}


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
char ***read_credentials_file(int *number_of_accounts, FILE *credentials_file_ptr)
{
    int i;
    char ch, ***credentials;
    *number_of_accounts = 0; 

    
    // Count number of lines (accounts). Then rewind for reading.
    while ((ch = fgetc(credentials_file_ptr)) != EOF) 
    {
        if (ch == '\n')
        {
            *number_of_accounts += 1;
        }
    }
    rewind(credentials_file_ptr);


    // Allocate space for credentials array. Then read account data.
    credentials = (char ***) calloc(*number_of_accounts, sizeof(char **));
    if (credentials == NULL)
    {
        puts("Not enough memory, exiting...");
        return NULL;
    }
    for (i = 0; i < *number_of_accounts; i++) 
    {
        credentials[i] = (char **) calloc(2, sizeof(char *));
        credentials[i][0] = (char *) calloc(MAX_USERNAME, sizeof(char));
        credentials[i][1] = (char *) calloc(MAX_PASSWORD, sizeof(char));

        fscanf(credentials_file_ptr, "%s %s", credentials[i][0], credentials[i][1]);
    }


    // Return accounts.
    return credentials;
}


// Create a new account by appending to 'credentials_file_ptr'. 
// Assumes 'credentials_file_ptr' is already opened.
void create_new_account(char *username, char *password, FILE *credentials_file_ptr)
{
    if (credentials_file_ptr == NULL)
    {
        puts("Credentials file is not accessible. Have you opened the file?");
        return;
    }

    fprintf(credentials_file_ptr, "%s %s\n", username, password);
}


// Open credentials file for both reading and writing.
FILE *open_credentials_file()
{
    FILE *credentials_file_ptr;

    credentials_file_ptr = fopen("login.dat", "a+b");
    if (credentials_file_ptr == NULL)
    {
        puts("Credentials file is not accessible.");
    }

    return credentials_file_ptr;
}


// Print account information. Only for DEBUG purpose.
void print_credentials_debug(char ***credentials, int number_of_accounts)
{
    int i;

    puts("Printing accounts:");
    for (i = 0; i < number_of_accounts; i++)
    {
        printf("%d %s %s\n", i, credentials[i][0], credentials[i][1]);
    }
    puts("Printing accounts complete...");
}


// Free credentials array.
void free_credentials(char ***credentials, int number_of_accounts)
{
    int i;

    for (i = 0; i < number_of_accounts; i++)
    {
        free(credentials[i][0]);
        free(credentials[i][1]);
        free(credentials[i]);
    } 
    
    free(credentials);
}


// Search the credentials array. If username found, return its index.
// Otherwise return -1.
int search_username(char ***credentials, int number_of_accounts, char *username)
{
    int i = 0, is_found = 0;


    // Search trough the credentials array.
    while ((i < number_of_accounts) && (is_found == 0))
    {
        if (strcmp(credentials[i][0], username) == 0)
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
int check_password(char ***credentials, int number_of_accounts, int username_index, char *password)
{
    int i = username_index;

    if (strcmp(credentials[i][1], password) == 0)
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
// Overall wrapper for account and login operations.
char *login_to_account_wrapper_io()
{
    char *username = calloc(MAX_USERNAME, sizeof(char));
    char *password = calloc(MAX_PASSWORD, sizeof(char));
    char ***credentials;
    int new_login, number_of_accounts, login_successful = 0;
    int username_index;
    FILE *credentials_file_ptr;


    // Read credentials file.
    credentials_file_ptr = open_credentials_file();
    credentials = read_credentials_file(&number_of_accounts, credentials_file_ptr);


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
                    create_new_account(username, password, credentials_file_ptr);
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
    fclose(credentials_file_ptr);
    free_credentials(credentials, number_of_accounts);
    free(password);


    // Return the username of user.
    return username;
}


int main_game_loop(MAZE_MAP map)
{
    int i;
    char *inventory = calloc((map.height * map.width), sizeof(char));

    // TODO: clock and score calculation
}


// TODO: auto-play mode