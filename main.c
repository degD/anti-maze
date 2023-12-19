
#include <stdio.h>
#include <string.h>

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
        printf("New account? (y/n)");
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