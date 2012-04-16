#include "action.h"
#include "error.h"
#include "protocol.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

static int handle_message(void);

static int set_action(actionfp action, char *set_name, char *key_name);

static int add_key(char *key_name);
static int find_key(char *key_name);
static int clear_key(char *key_name);

int usage() {
    fprintf(stderr, "USAGE: fsset <base-dir>\n");
    return -1;
}

int main(int argc, char **argv) {
    char *base_dir;
    int err;

    if(argc != 2)
        return usage();

    base_dir = argv[1];

    if(chdir(base_dir) != 0)
        return error("could not set base directory %s\n", base_dir);

    while(!feof(stdin))
        if((err = handle_message()))
            return err;

    return 0;
}

int handle_message(void) {
    char *set_name;
    char *key_name;
    Action action;
    actionfp f;
    int err;

    if((err = read_message(&action, &set_name, &key_name)) == ERROR)
        return err;
    else if(err == EXT_ERROR)
        return SUCCESS;

    switch(action) {
        case ADD:
            f = add_key;
            break;
        case CHECK:
            f = find_key;
            break;
        case CLEAR:
            f = clear_key;
            break;
        default:
            return error("internal error: invalid action ID %d\n", action);
    }

    return set_action(f, set_name, key_name);
}

/* returns 0 unless an internal error happened. External errors - such as if
   the user tried to use a non-existent set - are still treated internally as
   successes */
int set_action(actionfp action, char *set_name, char *key_name) {
    int basedir_fd;
    int err = 0;
    
    /* get a FD for the current (base) dir to later return to */
    if((basedir_fd = open(".", O_RDONLY)) == -1) {
        err = error("internal error: could not get base directory\n");
        perror("errno");
        return err;
    }

    /* try to enter the set. If we fail, we consider the set not correctly set
       up and tell the user it was an invalid set */
    if(chdir(set_name) == -1) {
        respond_bad_set(set_name);

        /* goto cleanup to close the basedir_fd */
        goto cleanup;
    }

    if((err = action(key_name)))
        goto cleanup;

    if(fchdir(basedir_fd) == -1) {
        err = error("internal error: could not return to base directory\n");
        goto cleanup;
    }

    cleanup:
    if(close(basedir_fd) == -1) {
        err = error("internal error: could not free basedir fd\n");
        perror("errno");
    }
    return err;
}

/* a set action - add key to the current set. Assumes the pwd is set correctly
   (by set_action) */
int add_key(char *key_name) {
    int fd;
    int err;

    if((fd = open(key_name, O_WRONLY | O_CREAT, DEFFILEMODE)) == -1) {
        err =  error("internal error: could not touch key\n");
        perror("errno");
        return err;
    }
    if(close(fd) == -1) {
        err = error("internal error: could not close key\n");
        perror("errno");
        return err;
    }

    return respond_key_add();
}

/* a set action - add key to the current set. Assumes the pwd is set correctly
   (by set_action) */
int find_key(char *key_name) {
    if(access(key_name, F_OK) == 0)
        return respond_has_key();
    else
        return respond_no_key();
}

/* a set action - add key to the current set. Assumes the pwd is set correctly
   (by set_action) */
int clear_key(char *key_name) {
    unlink(key_name);
    return respond_cleared();
}


