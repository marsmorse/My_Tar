#ifndef __PARSERHELPER_H__
#define __PARSERHELPER_H__
#include "parseInput.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "my_str.h"
void init_input(t_input **input);
char validFlag(char c);
char validOption(char c);
void check_first_arg(char **av, t_input **input);
void check_options(char **av, t_input **input);
void check_files(int ac, char **av, t_input **input);
#endif