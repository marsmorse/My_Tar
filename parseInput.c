#include "parseInput.h"
#include "parserHelper.h"
/*
**@param {int} param_1
**@param {char**} param_2
** Parse command line arguments, and fill t_input struct.
** Prints to the stdout error messages accordingly
*/
t_input *parseInput (int ac, char **av)
{
    t_input *input = NULL;

    if (ac == 1)
        my_putstr_len("my_tar: Must specify one of -c, -r, -t, -u, -x\n");
    else
    {
        input = (t_input*)malloc(sizeof(t_input));
        if (!input)
            return NULL;
        init_input(&input);
        //first arg flags[-crtux] options
        if (my_strlen(av[1]) <= 3)
            check_first_arg(av, &input);

        if (!input->mode)
        {
            my_putstr_len("Usage:\nList:    my_tar -tf <archive-filename>\n");
            my_putstr_len("Extract: my_tar -xf <archive-filename>\n");
            my_putstr_len("Create:  my_tar -cf <archive-filename> [filenames...]");
            return NULL;
        }

        //second arg and beyond [-f] of [filenames...]
        check_options(av, &input);
        check_files(ac, av, &input);
        if (!input->nbr_of_files)
        {
            my_putstr_len("my_tar: no files or directories specified\n");
            return NULL;
        }
    }
    return input;
}

int  main(int ac, char **av)
{
    t_input *input;

    input = parseInput(ac, av);
    if (!input)
        return 1;
    printf("Mode: %c\n", input->mode);
    printf("Options: %c\n", input->options);
    printf("Number of files: %d\n", input->nbr_of_files);
    printf("Tar file index and name: %d %s\n", input->tar_name_index, av[input->tar_name_index]);
    printf("Files indexes and names: \n");
    int i = 0;
    while (i < input->nbr_of_files)
    {
        printf("%d %s\n", input->files_index[i], av[input->files_index[i]]);
        i++;
    }
    
    free(input->files_index);
    input->files_index = NULL;
    free(input);
    return 0;
}