
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_USERNAME 20
#define MAX_PASSWORD 20

// TODO: FIX USER DENIES NEW ACCOUNT. EXIT IN RETURN.


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


int main()
{
    char *username;

    username = login_to_account_wrapper_io();
    puts(username);

    return 0;
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