#include "parserHelper.h"

/*
**@param {t_input**} param_1
** Initialize all the members of the t_input struct
*/
void init_input(t_input **input)
{
    (*input)->mode = 0;
    (*input)->options = 0;
    (*input)->nbr_of_files = 0;
    (*input)->tar_name_index = 0;
    (*input)->files_index = NULL;
}

/*
**@param {char} param_1
** Checks if char c is a valid flags [crtux]
*/
char validFlag(char c)
{
    char flags_sup[] = "crtux";
    char mode = 0;
    int index = 0;

    while(flags_sup[index])
    {
        if (c == flags_sup[index])
            mode = flags_sup[index];
        index++;
    }
    return mode;
}

/*
**@param {char} param_1
** Checks if char c is a valid options [f]
*/
char validOption(char c)
{
    char options_sup[] = "f";
    char option = 0;
    int index = 0;

    while(options_sup[index])
    {
        if (c == options_sup[index])
            option = options_sup[index];
        index++;
    }
    return option;
}

/*
**@param {char**} param_1
**@param {t_input**} param_2
** Checks the first argument passed to my_tar, validating flags, options
** and filling t_input struct. Prints to stdout if more then one valid flag is chosen
*/
void check_first_arg(char **av, t_input **input)
{
    int index = 0;
    char flag = 0;
    char option = 0;

    while (av[1][index])
    {
        flag = validFlag(av[1][index]);
        option = validOption(av[1][index]);
        if (flag && (*input)->mode == 0)
            (*input)->mode = flag;
        else if (option && (*input)->options == 0)
            (*input)->options = option;
        else if (flag && (*input)->mode != 0)
        {
            my_putchar(av[1][index]);
            my_putstr_len(" and -");
            my_putchar((*input)->mode);
            my_putstr_len("\n");
            free(input);
        }
        index++;
    }
}

/*
**@param {char**} param_1
**@param {t_input**} param_2
** Checks if a valid option and format were passed
*/
void check_options(char **av, t_input **input)
{
    int index = 0;
    int option = 0;

    if (av[2])
    {
        if (my_strlen(av[2]) <= 2 && !(*input)->options)
        {
            while (av[2][index])
            {
                option = validOption(av[2][index]);
                if (option)
                    (*input)->options = option;
                index++;
            }
        }
    }
}

/*
**@param {int} param_1
**@param {char**} param_2
**@param {t_input**} param_2
** Checks if a valid file or directory were passed
*/
void check_files(int ac, char **av, t_input **input)
{
    int arg = 2;
    filestat f_stat;
    char *file_path = NULL;
    int index = 0;

    while (arg < ac)
    {
        file_path = my_strjoin("./", av[arg]);
        if (stat(file_path, &f_stat) == 0)
            (*input)->nbr_of_files += 1;
        arg++;
        free(file_path);
    }
    
    arg = 2;
    (*input)->files_index = (int*)malloc(sizeof(int) * (*input)->nbr_of_files);
    while (arg < ac)
    {
        file_path = my_strjoin("./", av[arg]);
        if (stat(file_path, &f_stat) == 0)
        {
            (*input)->files_index[index] = arg;
            index++;
        }
        else 
        {
            if (arg == 2 || arg == 3)
              (*input)->tar_name_index = arg;
        }
        arg++;
        free(file_path);
    }
}

