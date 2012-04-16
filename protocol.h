#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "action.h"

int read_message(Action *action, char **set_name, char **key_name);

int respond_key_add();
int respond_has_key();
int respond_no_key();
int respond_cleared();
int respond_bad_set();
int respond_bad_act();
int respond_bad_ref();

#endif
