/*
** OBS: Alternative to <getopt.h>, in order to parse flags passed as argument to main function (my_tar)
**
*/
#ifndef __PARSEINPUT_H__
#define __PARSEINPUT_H__
#include "my_str.h"
/*
** OBS: char option would need to be changed to char *options, in order to implement more options
** mode -> flag representing the mode chosen by the user. [crtux] Create, Append, List, Update, Extract, respectviely;
** options -> char representing the options chosen by the user. [f]
** nbr_of_files -> number of valid files pased by the user;
** files_index -> indexes on the ac array (read by the main), of each file name
*/
typedef struct s_input
{
    char mode;
    char options;
    int nbr_of_files;
    int tar_name_index;
    int *files_index;
}t_input;
typedef struct stat filestat;
#endif