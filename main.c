
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_USERNAME 20
#define MAX_PASSWORD 20


void login_screen_io(char *username, char *password, int *new_login);


int main()
{
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int new_login;

    login_screen_io(username, password, &new_login);
    printf("Login info:\n%s\n%s\n%d\n", username, password, new_login);


    // Ask for user data


    // Print maze and play the game


    // Print game score

    return 0;
}


// Print login screen and read user credentials.
// If no username and password given, ask to create a new account.
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
            puts("Exiting...");
        }
    }
}


// Read account information from credentials file 'credentials_file_ptr'. 
// Allocate space for an array to store accounts.
void read_credentials_file(char ***credentials, int *number_of_accounts, FILE *credentials_file_ptr)
{
    int i;
    *number_of_accounts = 0; 

    
    // Count number of lines (accounts). Then rewind for reading.
    while (feof(credentials_file_ptr)) 
    {
        if (getc(credentials_file_ptr) == '\n')
        {
            *number_of_accounts++;
        }
    }
    rewind(credentials_file_ptr);


    // Allocate space for credentials array. Then read account data.
    credentials = (char ***) calloc(number_of_accounts, sizeof(char **));
    if (credentials == NULL)
    {
        puts("Not enough memory, exiting...");
        return;
    }
    for (i = 0; i < number_of_accounts; i++) 
    {
        credentials[i] = calloc(2, sizeof(char *));
        credentials[i][0] = calloc(MAX_USERNAME, sizeof(char));
        credentials[i][1] = calloc(MAX_PASSWORD, sizeof(char));

        fscanf(credentials_file_ptr, "%s %s", credentials[i][0], credentials[i][1]);
    }
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
        return;
    }

    return credentials_file_ptr;
}