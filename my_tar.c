#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "archive.h"
#include <sys/types.h>
#include <unistd.h>
/*
-c Create a new archive containing the specified items.
-r Like -c, but new entries are appended to the archive. The -f option is required.
-t List archive contents to stdout.
-u Like -r, but new entries are added only if they have a modification date newer than the corresponding entry in the archive. 
    The -f option is required.
-x Extract to disk from the archive. 
    If a file with the same name appears more than once in the archive, each copy will be extracted, with later copies overwriting (replacing) earlier copies.
*/

#define FATALERR 2
#define SUCCESS 0


int
main(int argc, char *argv[])
{
    char mode;
    char* tar_name;
    int flags, opt;
    flags = 0;
    //each of ctrxu must be unique and if more than one of them is input then the program crashes.
    while((opt = getopt(argc, argv, "curxtf:")) != -1)
    {
        //printf("opt = %c\n", opt);
        switch (opt)
        {
            case 't':
                if (flags == 0)
                {
                    flags = 1;
                    mode = opt;
                    break;
                }
                else
                {
                    fprintf(stderr, "ERROR: conflicting input flags\nUsage: %s [-%c] [-f tarname] [archive members] OR %s [-%cf] [tar name] [archive members]\n", argv[0], opt, argv[0], opt);
                    exit(EXIT_FAILURE);
                }
            case 'r':
                if (flags == 0)
                {
                    flags = 1;
                    mode = opt;
                }
                else
                {
                    fprintf(stderr, "ERROR: conflicting input flags\nUsage: %s [-%c] [-f tarname] [archive members] OR %s [-%cf] [tar name] [archive members]\n", argv[0], opt, argv[0], opt);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'c':
                if (flags == 0)
                {
                    flags = 1;
                    mode = 'c';
                    break;
                }
                else
                {
                    fprintf(stderr, "ERROR: conflicting input flags\nUsage: %s [-%c] [-f tarname] [archive members] OR %s [-%cf] [tar name] [archive members]\n", argv[0], opt, argv[0], opt);
                    exit(EXIT_FAILURE);
                }
            case 'f':
                tar_name = optarg;
                break;
            case 'x':
                if (flags == 0)
                {
                    flags = 1;
                    mode = 'x';
                }
                else
                {
                    fprintf(stderr, "ERROR: conflicting input flags\nUsage: %s [-%c] [-f tarname] [archive members] OR %s [-%cf] [tar name] [archive members]\n", argv[0], opt, argv[0], opt);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'u':
                if (flags == 0)
                {
                    flags = 1;
                    mode = 'u';
                }
                else
                {
                    fprintf(stderr, "ERROR: conflicting input flags\nUsage: %s [-%c] [-f tarname] [archive members] OR %s [-%cf] [tar name] [archive members]\n", argv[0], opt, argv[0], opt);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-%c] [-f tarname] restofinput\n", argv[0], opt);
                exit(EXIT_FAILURE);
        }
    }

    //printf("tarname = %s and mode = %c and optind = %d and argc = %d\n", tar_name, mode, optind, argc);
    if (optind >= argc && (mode == 'c' || mode == 'r'))
    {
        fprintf(stderr, "ERROR: no archive members specified\nUsage: %s [-%c] [-f tarname] restofinput\n", argv[0], opt);
        exit(EXIT_FAILURE);
    }

    Archive tar = newArchive(tar_name, mode);
    UINT8 result;     
    if (mode == 'r')
        result = createArchive(tar, (const char** )argv, optind, argc);
    else if (mode == 'c')
        result = createArchive(tar, (const char** )argv, optind, argc);
    else if (mode == 'x')
        result = extractArchive(tar, (const char** )argv, optind, argc);
    else if (mode == 'u')
        result = updateArchive(tar, (const char** )argv, optind, argc);
    else if (mode == 't')
        result = listArchive(tar);
    //result = listArchive(tar, (const char** )argv, optind, argc);
    
    deleteArchive(tar);
    tar = NULL;
    if (result == FATALERR)
    {
        fprintf(stderr, "my_tar: Exiting with failure status due to previous errors\n");
        return(FATALERR);
    }
    return(SUCCESS);
}
