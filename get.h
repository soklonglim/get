/*
 * get.h :
 *
 *
 */

#ifndef _GET_H
#define _GET_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

// debugging flag
#define DB 0


char* get_username(uid_t uid);
bool is_exist(char* fname);
bool match_ownership(const char* fname, const char *access);
int check_access_file(const char* fname);
bool copy_file(char* source, char* destination);
bool perform_get_request(char* source, char* destination);

#endif
