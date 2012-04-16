#include "protocol.h"

#include "debug.h"
#include "error.h"

#include <stdio.h>
#include <string.h>

static void sanitize_key(char *key_name);
static int install_action(char *action_str, Action *action);

int read_message(Action *action, char **set_name, char **key_name) {
    /* protocol:
        first three chars = action (GET, SET, DEL)
        then /
        chars upto / or .: set
        chars upto / or . or \n: key
        set and key must each be a max of 255 chars
        line is thus a max of:
        3 + 1 + 255 + 1 + 255 + 1 = 516 chars
        sticking NULLs into your sets or keys will probably cause bad_ref errors
    */
    char action_str[5];
    static char set_slash_key[512];

    if(fread(action_str, 1, 4, stdin) != 4) {
        if(feof(stdin))
            return ERROR;
        return error("Could not read action from stdin\n");
    }
    action_str[4] = '\0';

    if(install_action(action_str, action) != SUCCESS)
        return respond_bad_act();

    if(!fgets(set_slash_key, 512, stdin))
        return respond_bad_ref();

    /* start both set name and key name at the front, then move key_name
       forward til it finds where it's supposed to start */    
    *set_name = *key_name = set_slash_key;
    while(**key_name) {
        if(**key_name == '/' || **key_name == '.') {
            **key_name = '\0';
            (*key_name)++;
            break;
        }
        (*key_name)++;
    }
    if(!(**key_name))
        return respond_bad_ref();

    sanitize_key(*key_name);

    debug("action: %s\n", action_str);
    debug("set: %s\n", *set_name);
    debug("key: %s\n", *key_name);
    return SUCCESS;
}

int respond(char *response) {
    printf("%s", response);
    fflush(stdout);
    return SUCCESS;
}

int respond_key_add() { return respond("ADDED\n"); }
int respond_has_key() { return respond("HASKEY\n"); }
int respond_no_key() { return respond("NOKEY\n"); }
int respond_cleared() { return respond("CLEARED\n"); }
int respond_bad_set() { return respond("BADSET\n"); }
int respond_bad_act() { return respond("BADACT\n"); }
int respond_bad_ref() { return respond("BADREF\n"); }

static void sanitize_key(char *key_name) {
    while(*key_name) {
        if(*key_name == '/' || *key_name == '.' || *key_name == '\n')
            *key_name = '\0';
        else
            key_name++;
    }
}

static int install_action(char *action_str, Action *action) {
    if(strcmp(action_str, "GET/") == 0)
        *action = CHECK;
    else if(strcmp(action_str, "SET/") == 0)
        *action = ADD;
    else if(strcmp(action_str, "DEL/") == 0)
        *action = CLEAR;
    else
        return EXT_ERROR;

    return SUCCESS;
}


