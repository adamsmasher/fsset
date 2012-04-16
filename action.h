#ifndef ACTION_H
#define ACTION_H

typedef enum {ADD, CHECK, CLEAR} Action;
typedef int (*actionfp)(char *key_name);

#endif
