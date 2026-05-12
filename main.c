// file: main.c
// main.c 

#include <stddef.h>     // size_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "HashADT.h"

// struct representing a person in the network
typedef struct person_s {
    char *name;                 ///< name of the person
    char *handle;               ///< handle of the person
    struct person_s **friends;  ///< dynamic collection of friends
    size_t friend_count;        ///< current number of friends 
    size_t max_friends;         ///< current limit on friends
} person_t;

// the size for input buffers
#define BUFFER_SIZE 256

// the max possible valid number of arguments in a command
#define MAX_ARGS 4

// separate tokens using whitespace
#define   TOKEN_DELIMITERS   " \t"

// separate lines using newlines
#define   LINE_DELIMITERS   "\n"

// total number of people in the system, this is needed because the ht_getvalues 
// of the provided hash file does not work properly
int N_PEOPLE = 0;

// total number of friendships in the network
int N_FRIENDSHIPS = 0;

/// validate_name() - verify if the given string meets the requirements for a 
///     name
///
/// @param str the c-string to validate
///
/// @return true/false if the str is valid or not
///
bool validate_name(char * str) {
    char c;
    int i = 0;
    while ((c = str[i++])) {
        if (c >= 'a' && c <= 'z') continue;
        if (c >= 'A' && c <= 'Z') continue;
        if (c == '\'') continue;
        if (c == '-') continue;
        printf("error: argument \"%s\" is invalid", str);
        return false;
    }
    return true;
}

/// validate_name() - verify if the given string meets the requirements for a 
///     handle
///
/// @param str the c-string to validate
///
/// @return true/false if the str is valid or not
///
bool validate_handle(char * str) {
    char c = str[0];

    if ((!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z'))) return false;

    int i = 1;
    while ((c = str[i++])) {
        if (c >= 'a' && c <= 'z') continue;
        if (c >= 'A' && c <= 'Z') continue;
        if (c >= '0' && c <= '9') continue;
        printf("error: argument \"%s\" is invalid", str);
        return false;
    }
    return true;
}

/// check_handle() - check if the handle exists
///
/// @param handle the handle to check
///
/// @return true/false if the str is valid or not
bool check_handle(HashADT t, char * handle) {
    bool res = ht_has(t, handle);
    if (!res)
        printf("error: handle \"%s\" is unknown", handle);
    return res;
}

/// check_handle() - check if two handles are the same
///
/// @param handle1 handle to check
/// @param handle2 handle to check
///
/// @return true/false if the str is valid or not
bool check_handles(HashADT t, char * handle1, char * handle2) {
    bool res = strcmp(handle1, handle2) != 0;
    if (!res)
        printf("error: \"%s\" and \"%s\" are the same person", handle1, handle2);
    return res;
}
/// person_equals2 function - compares two persons for equality
///
/// @param element1 first person_s
/// @param element2 second person_s
///
/// @return true if the two strings are equal
///
bool person_equals2( struct person_s *element1, struct person_s *element2 ) {
    return strcmp( element1->handle, element2->handle ) == 0;
}

/// person_hash function returns the hash of a person_s
///
/// Based on djb2, A simple c-string hashing algorithm by Dan Bernstein
/// http://www.cse.yorku.ca/~oz/hash.html
///
/// @param element the person_s to hash
///
/// @return the hash value of the person_s
///
size_t person_hash( const void *element ) {
    // unsigned char *str = ((struct person_s *) element)->handle;
    unsigned char *str = ((unsigned char *) element);
    size_t hash = 5381;
    int c;
    int i = 0;

    while( (c = str[i++]) ) {
        if (c == -1 || c == '\0') break;
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return (size_t)hash;
}

void person_destroy(void *key, void *value) {
    unsigned char *handle = ((unsigned char *) key);
    struct person_s * p = ((struct person_s *) value);
    free(handle);
    if (p->friends)
        free(p->friends);
    free(p->name);
    free(p);
}

/// person_equals function - compares the elements as two stringss.
///
/// @param element1 first handle
/// @param element2 second handle
///
/// @return true if the two strings are equal
///
bool person_equals( const void *element1, const void *element2 ) {
    char * str1 = ((char * ) element1);
    char * str2 = ((char * ) element2);

    return strcmp( str1, str2 ) == 0;
}

/// person_print function - keys are long and values are person_ss.
///
/// @param key the long integer key
/// @param value the person_s value
///
static void person_print( const void *key, const void *value ) {
    printf( "%lu : %s\n", (long)key, (char*)value );
}

/// add function, function for the add command
///
/// Attempt to add a new person identified by the arguments given. If the handle
/// is found to be already in the system, report this fact with a message having
/// the following format (where nick is the erroneous handle):
///
/// @param t the hash table to add the person to
/// @param first_name the person_s first name
/// @param last_name the person_s last name
/// @param handle_name the person_s handle 
///
/// @return 1 if the function ran succesfully, -1 if there was an error
///
int add(HashADT t, char * first_name, char * last_name, char * handle) {
    if (ht_has(t, handle)) {
        printf("error: handle \"%s\" is already in use\n", handle);
        return 1;
    }

    struct person_s *p = NULL;
    p = (struct person_s *) malloc( sizeof(struct person_s) );

    p->name = malloc( sizeof(char) * (strlen(first_name) + strlen(last_name) + 2)); // extra for null and space
    p->handle = malloc( sizeof(char) * (strlen(handle) + 1));
    p->friend_count = 0;
    p->max_friends = 1;
    p->friends = NULL;

    // copy name into person
    int idx = 0;
    for (int i = 0; i < (int) strlen(first_name); i++) {
        p->name[idx++] = first_name[i];
    }
    p->name[idx++] = ' ';
    for (int i = 0; i < (int) strlen(last_name) + 1; i++) {
        p->name[idx++] = last_name[i];
    }

    // copy handle into person
    for (int i = 0; i < (int) strlen(handle) + 1; i++) {
        p->handle[i] = handle[i];
    }

    ht_put(t, p->handle, p);
    N_PEOPLE++;

    return 1;
}

/// check_friends() - check if the two people given are friends
///
/// @param p1 person one to check
/// @param p2 person two to check
///
/// @return true/false if they are friends
///
bool check_friends(struct person_s * p1, struct person_s * p2) {
    if (p1->friends == NULL) return false;
    int n = 0;

    struct person_s * cur_p = p1->friends[n];
    while (cur_p != NULL) {

        if (person_equals2(cur_p, p2)) {
            return true;
        }

        cur_p = p1->friends[++n];
    }
    return false;
}

/// add_friend() - add p2 to p1's friend list
///
/// @param p1 person one
/// @param p2 person two
void add_friend(struct person_s * p1, struct person_s * p2) {
    p1->friend_count++;

    if (p1->friend_count + 1 >= p1->max_friends) { // +1 for null terminator
        // allocate more space
        p1->max_friends = p1->max_friends * 2;
        p1->friends = (struct person_s **) realloc(p1->friends, (p1->max_friends + 1) * sizeof(struct person_s *));
    }

    p1->friends[p1->friend_count-1] = p2;
    p1->friends[p1->friend_count] = NULL;
}

/// un_friend() - remove p2 from p1's friend list
///
/// @param p1 person one
/// @param p2 person two
void un_friend(struct person_s * p1, struct person_s * p2) {
    
    int idx = 0;
    for (int i = 0; i < (int) p1->friend_count; i++) {
        if (person_equals2(p2, p1->friends[i])) {
            p1->friends[i] = NULL;
            idx = i;
            break;
        }
    }

    p1->friend_count--;

    for (int i = idx; i < (int) p1->friend_count; i++) {
        p1->friends[i] = p1->friends[i+1];
    }
}

/// friend function, function for the friend command
///
/// add bi-directional friend connections between the persons associated with 
/// the handles
///
/// @param t the hash table to get the people from
/// @param handle1 handle of person 1
/// @param handle2 handle of person 2
///
/// @return 1 if the function ran successfully, -1 if there was an error
///
int friend(HashADT t, char * handle1, char * handle2) {
    if (!check_handles(t, handle1, handle2)) return 0;

    struct person_s * p1 = ((struct person_s *) ht_get(t, handle1));
    struct person_s * p2 = ((struct person_s *) ht_get(t, handle2));

    if (check_friends(p1, p2)) {
        printf("%s and %s are already friends.\n", p1->handle, p2->handle);
        return 1;
    }

    add_friend(p1, p2);
    add_friend(p2, p1);
    N_FRIENDSHIPS++;

    printf("%s and %s are now friends.\n", p1->handle, p2->handle);

    return 1;
}

/// unfriend function, function for the friend command
///
/// @param t the hash table to get the people from
/// @param handle1 handle of person 1
/// @param handle2 handle of person 2
///
/// @return 1 if the function ran successfully, -1 if there was an error
///
int unfriend(HashADT t, char * handle1, char * handle2) {
    if (!check_handles(t, handle1, handle2)) return 0;

    struct person_s * p1 = ((struct person_s *) ht_get(t, handle1));
    struct person_s * p2 = ((struct person_s *) ht_get(t, handle2));

    if (check_friends(p1, p2)) {
        printf("%s and %s are not friends.\n", p1->handle, p2->handle);
        return 1;
    }

    un_friend(p1, p2);
    un_friend(p2, p1);
    N_FRIENDSHIPS--;

    printf("%s and %s are no longer friends.\n", p1->handle, p2->handle);

    return 1;
}

/// stats function,prints stats for the network
///
/// @param t the hash table to get the people from
///
/// @return 1 if the function ran successfully, -1 if there was an error
///
int stats(HashADT t) {
    printf("Statistics: ");

    if (N_PEOPLE) printf("%d", N_PEOPLE);
    printf(N_PEOPLE ? (N_PEOPLE > 1 ? " people" : " person") : "no people");

    printf(", ");

    if (N_FRIENDSHIPS) printf("%d", N_FRIENDSHIPS);
    printf(N_FRIENDSHIPS ? 
        (N_FRIENDSHIPS > 1 ? " friendships" : " friendship") : "no friendships");

    printf("\n");

    return 1;
}

/// size function, print size for the given user
///
/// @param t the hash table to get the people from
/// @param handle the handle to print stats
///
/// @return 1 if the function ran succesfully, -1 if there was an error
///
int size(HashADT t, char * handle) {
    if (!check_handle(t, handle)) return 0;

    struct person_s * p = (struct person_s *) ht_get(t, handle);
    printf("%s (%s) has ", handle, p->name);

    int n_friends = p->friend_count;
    if (n_friends) printf("%d", n_friends);
    printf(n_friends ? 
        (n_friends > 1 ? " friends" : " friend") : "no friends");
    printf("\n");

    return 1;
}

/// print function, print stats for the given user
///
/// @param t the hash table to get the people from
/// @param handle the handle to print
///
/// @return 1 if the function ran succesfully, -1 if there was an error
///
int print(HashADT t, char * handle) {
    if (!check_handle(t, handle)) return 0;

    struct person_s * p = (struct person_s *) ht_get(t, handle);
    
    size(t, handle);

    if (p->friends == NULL) return 1;

    struct person_s * cur_p; // iterate over people
    int n_friends = 0;
    while ((cur_p = p->friends[n_friends++])) {
        printf("\t%s (%s)\n", cur_p->handle, cur_p->name);
    }

    return 1;
}

/// check_command() - check if the given command is valid for the given parameters
///
/// @param echo 1 to echo
/// @param token the current token to verify
/// @param command the command to check if the token is equal to
/// @param usage the usage string to report for an error
/// @param args_valid the number of arguments that are valid for this command
/// @param argc the number of args provided from the line
/// @param tokens the list of tokens from the line
///
/// @return 1 if the function parameters match, -1 if there was an error, 0 if the command did not match
int check_command(int echo, char * token, char * command, char * usage, int args_valid, int argc, char * (tokens[MAX_ARGS])) {
    if (strcmp(token, command) != 0) {
        return 0;
    }

    if (args_valid != argc) {
        fprintf(stderr, "error: %s", usage);
        return -1;
    }

    // PRINT ALL ARGS
    if (!echo) {
        printf("Amici> +");
        for (int i = 0; i < argc; i++) {
            printf(" \"%s\"", tokens[i]);
        }
        printf("\n");
    }

    return 1;
}

/// init() - delete the whole system and re-initialize everything
///
/// @param t the hash table
void init(HashADT * t) {
    ht_destroy(*t);
    *t = ht_create(person_hash, person_equals, person_print, person_destroy);
    N_PEOPLE = 0;
    N_FRIENDSHIPS = 0;
    printf("System re-initialized\n");
}

/// process_line() - process the given line
///
/// @param t the hash table
/// @param line pointer to the c-string line to process
/// @param echo 0 to echo back the command, 1 to stay silent
///
/// @return 1 for success, 0 for error, -1 for quit
int process_line(HashADT * table, char ** line, int echo) {
    HashADT t = *table;

    char * token;

    // first call specifies the buffer to be parsed
    token = strtok( *line, TOKEN_DELIMITERS );

    char * (tokens[MAX_ARGS]) = {"", "", "", ""};
    int n_tokens = 0;

    while( token != NULL ) {
        // do something with this token
        if (n_tokens < MAX_ARGS) {
            tokens[n_tokens] = token;
        }
        n_tokens++;

        if (n_tokens > MAX_ARGS) {
            token = NULL;
            break;
        }

        // subsequent calls to parse the same buffer
        token = strtok( NULL, TOKEN_DELIMITERS );
    }

    int code = 0;
    char * usage_str;

    /// add  first-name  last-name  handle
    if ((code = check_command(echo, tokens[0], "add", 
            usage_str = "usage: add  first-name  last-name  handle", 4, n_tokens, tokens)) == -1) {
        return 0;
    } else if (code == 1) {
        if (!validate_name(tokens[1]) || !validate_name(tokens[2]) || !validate_handle(tokens[3])) {
            return 1;
        } else if (add(t, tokens[1], tokens[2], tokens[3]) == -1) {
            return 0;
        }
    }

    /// friend  handle1  handle2
    else if ((code = check_command(echo, tokens[0], "friend", 
            "usage: friend  handle1  handle2", 3, n_tokens, tokens)) == -1) {
        return 0;
    } else if (code == 1) {
        if (!validate_handle(tokens[1]) || !validate_handle(tokens[2])) {
            return 1;
        } else if (friend(t, tokens[1], tokens[2]) == -1) {
            return 0;
        }
    }

    /// init
    else if ((code = check_command(echo, tokens[0], "init", 
            usage_str = "usage: init", 1, n_tokens, tokens)) == -1) {
        return 0;
    } else if (code == 1) {
        // function call
        init(table);
    }

    /// print  handle
    else if ((code = check_command(echo, tokens[0], "print", 
            usage_str = "usage: print  handle", 2, n_tokens, tokens)) == -1) {
        return 0;
    } else if (code == 1) {
        if (!validate_handle(tokens[1])) {
            return 1;
        } else if (print(t, tokens[1]) == -1) {
            return 0;
        }
    }

    /// quit
    else if ((code = check_command(echo, tokens[0], "quit", 
            "usage: quit", 1, n_tokens, tokens)) == -1) {
        return 0;
    } else if (code == 1) {
        // function call
        return -1;
    }

    /// size  handle
    else if ((code = check_command(echo, tokens[0], "size", 
            usage_str = "usage: size  handle", 2, n_tokens, tokens)) == -1) {
        return 0;
    } else if (code == 1) {
        if (!validate_handle(tokens[1])) {
            return 1;
        } else if (size(t, tokens[1]) == -1) {
            return 0;
        }
    }

    /// stats
    else if ((code = check_command(echo, tokens[0], "stats", 
            "usage: stats", 1, n_tokens, tokens)) == -1) {
        return 0;
    } else if (code == 1) {
        if (stats(t) == -1) {
            return 0;
        }
    }

    /// unfriend  handle1  handle2
    else if ((code = check_command(echo, tokens[0], "unfriend", 
            "usage: unfriend  handle1  handle2", 3, n_tokens, tokens)) == -1) {
        return 0;
    } else if (code == 1) {
        if (!validate_handle(tokens[1]) || !validate_handle(tokens[2])) {
            return 1;
        } else if (unfriend(t, tokens[1], tokens[2]) == -1) {
            return 0;
        }
    }

    return 1;
}

///
/// main : [ datafile ] -> int
///     get command line input, and run the simulation
///
/// @param  argc  command-line argument count
/// @param  argv  array of strings containing command-line arguments
///
/// @return termination status for the program
///
int main( int argc, char * argv[] ) {

    HashADT t = ht_create(person_hash, person_equals, person_print, person_destroy);

    char * usage_string = "usage: amici [ datafile ]";

    if (argc >= 2) {
        if (argc > 2) {
            // too many inputs
            fprintf(stderr, "%s", usage_string);
            return EXIT_FAILURE;
        }

        // process file
        // echo = 0

        char * fname = argv[1];

        FILE *file1 = fopen(fname, "rb");
        if (!file1) {
            perror(fname);
            exit(EXIT_FAILURE);
        } else {
            char buffer[BUFFER_SIZE];

            while( fgets(buffer,BUFFER_SIZE,file1) != NULL ) {
                char * line;

                // first call specifies the buffer to be parsed
                line = strtok( buffer, LINE_DELIMITERS );

                while( line != NULL ) {
                    printf("%s\n", line);
                    // do something with this token
                    int code = process_line(&t, &line, 0);
                    if (code == 0) {
                        return EXIT_FAILURE;
                    } else if (code == -1) {
                        // quit command
                        ht_destroy(t);
                    }

                    // subsequent calls to parse the same buffer
                    line = strtok( NULL, LINE_DELIMITERS );
                }
            }

            fclose(file1);

            return 0;
        }
    }

    // read from standard input
    // echo = 1
    char buffer[BUFFER_SIZE];

    while( fgets(buffer,BUFFER_SIZE,stdin) != NULL ) {
        char * line;

        // first call specifies the buffer to be parsed
        line = strtok( buffer, LINE_DELIMITERS );

        while( line != NULL ) {
            // do something with this token
            int code = process_line(&t, &line, 0);
            if (code == 0) {
                return EXIT_FAILURE;
            } else if (code == -1) {
                // quit command
                ht_destroy(t);
            }

            // subsequent calls to parse the same buffer
            line = strtok( NULL, LINE_DELIMITERS );
        }
    }

    return EXIT_SUCCESS;
}
