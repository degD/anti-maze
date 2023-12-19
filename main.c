
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_USERNAME 20
#define MAX_PASSWORD 20


void login_screen_io(char *username, char *password, int *new_login);
char ***read_credentials_file(int *number_of_accounts, FILE *credentials_file_ptr);
void create_new_account(char *username, char *password, FILE *credentials_file_ptr);
FILE *open_credentials_file();
void print_credentials_debug(char ***credentials, int number_of_accounts);
void free_credentials(char ***credentials, int number_of_accounts);
int search_username(char ***credentials, int number_of_accounts, char *username);


int main()
{
    int n;
    char username[MAX_USERNAME] = "test";
    char password[MAX_PASSWORD] = "1234";
    char ***credentials;
    FILE *fp;

    // login_screen_io(username, password, &new_login);
    // printf("Login info:\n%s\n%s\n%d\n", username, password, new_login);

    fp = open_credentials_file();
    // create_new_account(username, password, fp);
    credentials = read_credentials_file(&n, fp);
    // printf("%d\n", n);
    // print_credentials_debug(credentials, n);
    printf("%s is found? %d\n", "test", search_username(credentials, n, "test"));
    printf("%s is found? %d\n", "tea", search_username(credentials, n, "tea"));
    fclose(fp);
    free_credentials(credentials, n);


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