/*

    archive class to perform basic operations on a archive file

    archive class has three main functions update, extract, and create.
    operations include:
    
        * updating
        * appending
        * extracting an archive
        * create an archive
        * listing
*/
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysmacros.h>
#include <pwd.h>
#include <sys/xattr.h>
#include <grp.h>
#include "archive.h"

#define ENDBUFFER_SIZE 512
#define BLOCK_SIZE 512
#define BUFFER_SIZE 16080
#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION " 0"           /* 00 and no null */
#define TVERSLEN 2
//the typedef is used so we can use the name archiveObj to represent the structure instead of struct archiveObj
typedef struct archiveObj
{
    int arc_fd;
    int size;
    char name[256];
    int block_count;
} archiveObj;
/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

/* return values */
#define FATALERR 2
#define SUCCESS 0

//opens/creates new file with file descriptor. if c flag is set truncate the old .tar file with the same name if it exists
Archive newArchive (const char* tarName, char mode)
{
    Archive arc;
    arc = (Archive)malloc(sizeof(archiveObj));
    if (mode == 'c')
        arc->arc_fd = open(tarName, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
    else if ( mode == 'x' || mode == 't')
        arc->arc_fd = open(tarName, O_RDONLY);
    else if (mode == 'r' || mode == 'u')
        arc->arc_fd = open(tarName, O_WRONLY);
    else
        arc->arc_fd = -1;
    if (arc->arc_fd == -1)
    {
        fprintf(stderr , "open failed in newArchive: %s\n", strerror(errno));
        exit(FATALERR);
    }
    strncpy(arc->name, tarName, 100);
    arc->block_count = getBlockCount(tarName);
    //BLOCK_SIZE * arc->block_count
    if (mode == 'r' || mode == 'u')
        lseek(arc->arc_fd, (BLOCK_SIZE * arc->block_count), SEEK_CUR);
    else
        lseek(arc->arc_fd, -(BLOCK_SIZE * (arc->block_count + 2)), SEEK_CUR);
    return arc;
}
/*
    int arc_fd;
**@param {Archive} self
** closes the file descriptor of the archive and then frees the archive!
*/
void deleteArchive(Archive self)
{
    if (close(self->arc_fd) == -1)
    {
        fprintf(stderr , "close failed in deleteArchive function: %s\n", strerror(errno));
        exit(0);
    }
    free(self);
    self = NULL;
}
/*
    given an archive get the block content of the archive
*/
size_t getBlockCount(const char* tarName)
{
    int fd = open(tarName, O_RDONLY);
    if (fd == -1)
        exit(FATALERR);
    char buf[512];
    size_t block_count = 0;
    while (read(fd, buf, 512) > 0)
    {
        //check for two 512 blocks signalling end of archive
        if (!*buf)
        {
            read(fd, buf, 512);
            if (!*buf)
                break;
            else
                block_count++;
        }
        block_count++;
    }
    close(fd);
    return block_count;
}
/*
|       writeFileToArchive: writes file specified by input to the archive
|       
|       input: [char** ] array of strings representing the file paths
|       return: Null
|       
|       creates an archive given an array of input from the user to archive
*/
/***********************************
|                                  |
|                                  |
|     Create Archive Functions
|                                  |
|                                  |
************************************/
UINT8 createArchive(Archive self, const char** paths, UINT8 arch_member_start_index, UINT8 argc)
{
    //loop over file and folder names in inputArr and create an archive of each
    UINT8 result = SUCCESS;
    char name[100];
    for (UINT8 i = arch_member_start_index; i < argc; i++)
    {
        strcpy(name, paths[i]);
        result = createArchiveEntry(self, name);
        if (result == FATALERR)
            return FATALERR;
    }
    char buf[512] = {'\0'};
    printf("block_count = %u\n", self->block_count);
    //fill rest of record. Assuming blocking factor is 20 so size of the record/ archive content is 10240
    for (int i = 0; i < (20 - (self->block_count % 20)); i++)
        write(self->arc_fd, buf, 512);
    return SUCCESS;
}
/*
    createArchiveEntry
*/
UINT8 createArchiveEntry(Archive self,const char* path)
{
    UINT8 result = SUCCESS;
    archiveHeader header = createHeader();
    if (loadHeaderData(header, (char*)path, FROMFILE) == FATALERR)
        return FATALERR;
    if (writeHeaderToArchive(self, header) == FATALERR)
    {
        fprintf(stderr , "Error writing header to archive in createArchiveEntry: %s\n", strerror(errno));
        return FATALERR;
    }
    self->block_count += 1;
    // if the file is a director create another struct type with an array of file and folder names with size and call createArchive on that
    if (header->typeflag == DIRTYPE)
    {
        DIR* dirp;
        if ((dirp = opendir(path)) == NULL)
        {
            fprintf(stderr , "opendir(%s) failed in createArchiveEntry: %s\n", path, strerror(errno));
            return FATALERR;//exit(EXIT_FAILURE); 
        }
        struct dirent* dirc;
        //errno = 0;
        while ((dirc = readdir(dirp)) != NULL)
        {
            //printf("dircname = %s\n", dirc->d_name);
            if (strcmp(dirc->d_name, ".") != 0 && strcmp(dirc->d_name, "..") != 0 && strcmp(dirc->d_name, "") != 0)
            {
                char dName[256];
                strcpy(dName, header->name);
                strcat(dName, dirc->d_name);
                dName[strlen(dName)] = '\0';
                createArchiveEntry(self, dName);
            }
        }
        if (errno) // if an error occured when reading directory
        {
            fprintf(stderr , "readdir failed in createArchiveEntry: %s\n", strerror(errno));
            result = FATALERR;
        }
        closedir(dirp);       
    }
    else if(header->typeflag == REGTYPE)
    {
        result = writeFileToArchive(self, header->name);
        int block_count = atoi(header->size)/512;
        if (atoi(header->size) > 0 && atoi(header->size)%512 != 0)
            block_count += 1;
        self->block_count += block_count;
    }
    deleteHeader(header);
    if (result == SUCCESS)
        return SUCCESS;
    else
        return FATALERR;
}
/*
|       writeFileToArchive: writes file specified by input to the archive
|       
|       input: [char* ] filepath
|
!!      we are assuming that the archive file descriptor offset is located at position we want to write to in the archive file.
!!      Assuming file has read permissions. (Probably shouldn't make this assumption)!!!
|       return:
|           number of bytes read from file which should also equal the number of bytes written to archive. if return value is 0 then the file was empty.
*/
UINT8 writeFileToArchive(Archive self, const char* filepath)
{
    int fd;
    size_t file_size;
    if ((fd = open(filepath, O_RDONLY)) == -1)
    {
        fprintf(stderr , "open(%s) failed in writeFileToArchive: %s\n", filepath, strerror(errno));
        return FATALERR;
    }
    char buf[ENDBUFFER_SIZE] = {'\0'};
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buf, ENDBUFFER_SIZE)) > 0)
    {
        char writebuf[ENDBUFFER_SIZE] = {'\0'};
        strncpy(writebuf, buf, ENDBUFFER_SIZE);
        if (write(self->arc_fd, writebuf, ENDBUFFER_SIZE) == -1)
        {
            fprintf(stderr , "write failed in writeFileToArchive: %s\n", strerror(errno));
            close(fd);
            return FATALERR;
        }
        file_size += bytes_read;
    }
    if (bytes_read == -1)
    {
        close(fd);
        fprintf(stderr , "read failed writeFileToArchive: %s\n", strerror(errno));
        return FATALERR;
    }
    close(fd);
    return SUCCESS;
}
/*
|       writeHeaderEntry: general function to write a string less than 255 bytes long to the archive header
            if the numFlag is set then the function prepends the number with zero's if the number is smaller than input size
|
!           assumes no entry will be over 255 bytes
|  
|           if an error occur with the write the program exits
*/
UINT8 writeHeaderEntry(int arc_fd, char* entry, UINT8 size, UINT8 numFlag)
{
    if (numFlag)
    {
        for (UINT8 i = strlen(entry); i < size-1; i++)
            write(arc_fd, "0", 1);
    }
    if (write(arc_fd, entry, strlen(entry) + 1)  == -1)
    {
        fprintf(stderr , "write failed in writeToArchive: %s\n", strerror(errno));
        return FATALERR;
    }
    if (!numFlag)
    {
        for (UINT8 i = strlen(entry); i < size-1; i++)
            write(arc_fd, "\0", 1);
    }
    return SUCCESS;
}
/*
|   writes data from a header object to the archive using the writeHeaderEntry function. The last arugment into signals the function to prepend the value with 0's for text entries like name and linkname
*/

UINT8 writeHeaderToArchive(Archive self, archiveHeader header)
{
    if (writeHeaderEntry(self->arc_fd, header->name, 100, 0) == FATALERR)
        return FATALERR;
    if (writeHeaderEntry(self->arc_fd, header->mode, 8, 1) == FATALERR)
        return FATALERR;
    if (writeHeaderEntry(self->arc_fd, header->uid, 8, 1) == FATALERR)
        return FATALERR;
    if (writeHeaderEntry(self->arc_fd, header->gid, 8, 1) == FATALERR)
        return FATALERR;
    if (writeHeaderEntry(self->arc_fd, header->size, 12, 1) == FATALERR)
        return FATALERR;
    if (writeHeaderEntry(self->arc_fd, header->mtime, 12, 1) == FATALERR)
        return FATALERR;
    if (writeHeaderEntry(self->arc_fd, header->chksum, 7, 1) == FATALERR)
        return FATALERR;
    char space = ' ';
    if (write(self->arc_fd, &space, 1) == -1)
        return FATALERR;
    if (write(self->arc_fd, &(header->typeflag), 1) == -1)
        return FATALERR;
    if (writeHeaderEntry(self->arc_fd, header->linkname, 100, 0) == FATALERR)
        return FATALERR;
    if (write(self->arc_fd, header->magic, 5) == -1)
        return FATALERR;
    //if (writeHeaderEntry(self->arc_fd, header->magic, TMAGLEN, 1) == FATALERR)
        //return FATALERR;           // magic[6];
    if (write(self->arc_fd, &space, 1) == -1)
        return FATALERR;
    if (write(self->arc_fd, &space, 1) == -1)
        return FATALERR;
    if (write(self->arc_fd, "\0", 1) == -1)
        return FATALERR;
    //if (writeHeaderEntry(self->arc_fd, header->version, TVERSLEN, 1) == FATALERR)
      //  return FATALERR;        //version[2];
    if (writeHeaderEntry(self->arc_fd, header->uname, 32, 0) == FATALERR)
        return FATALERR;                      // uname[32];
    if (writeHeaderEntry(self->arc_fd, header->gname, 32, 0) == FATALERR)
        return FATALERR;                      // gname[32];
    if (header->typeflag != BLKTYPE)
    {
        for (UINT8 i = 0; i < 16; i++)
        {
            if (write(self->arc_fd, "\0", 1) == -1)
                return FATALERR;
        }
    }
    else
    {
        if (writeHeaderEntry(self->arc_fd, header->devmajor, 8, 1) == FATALERR)
            return FATALERR;                    // devminor[8];
        if (writeHeaderEntry(self->arc_fd, header->devminor, 8, 1) == FATALERR)
            return FATALERR;                    // devminor[8];
    }
    for (UINT8 i = 0; i < 167; i++)
    {                         // prefix[155];
        if (write(self->arc_fd, "\0", 1) == -1)
            return FATALERR;
    }
    return SUCCESS;
}
/*
|       writeXattr: converts a number to a character array with a size specified by the input. 
|   taken from listxattr(7)
*/
/*
void writeXattr( Archive self, char* filepath)
{
    ssize_t buflen, keylen, vallen;
    char *buf, *key, *val;
    buflen = listxattr(filepath, NULL, 0);
    if (buflen == -1)
    {
        fprintf(stderr , "realink failed in writeSymLnkName: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (buflen == 0) 
    {
        printf("%s has no attributes.\n", filepath);
        exit(EXIT_SUCCESS);
    }
    buf = malloc(buflen);
    if (buf == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    buflen = listxattr(filepath, buf, buflen);
    if (buflen == -1)
    {
       perror("listxattr");
       exit(EXIT_FAILURE);
    }
    key = buf;
    while (buflen > 0)
    {

        printf("%s: ", key);


        vallen = getxattr(filepath, key, NULL, 0);
        if (vallen == -1)
            perror("getxattr");

        if (vallen > 0)
        {


            val = malloc(vallen + 1);
            if (val == NULL)
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            vallen = getxattr(filepath, key, val, vallen);
            if (vallen == -1)
                perror("getxattr");
            else
            {

                val[vallen] = 0;
                printf("%s", val);
            }
            free(val);
        } 
        else if (vallen == 0)
            printf("<no value>");

        printf("\n");
        keylen = strlen(key) + 1;
        buflen -= keylen;
        key += keylen;
    }
   free(buf);
}*/
/*
|       printHeader: prints the archive header struct object. useful for debugging.
|
*/
/***********************************
|                                  |
|                                  |
|   Archive Extraction Functions
|                                  |
|                                  |
************************************/
/*
    returns true if the member name is in the input archive member names
*/
UINT8 isSelectArchMember(const char** argv, UINT8 member_index, UINT8 argc, char* member_name)
{
    for (UINT8 i = member_index; i < argc; i++)
    {
        if (strcmp(member_name, argv[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}
UINT8 extractArchive(Archive self, const char** argv, UINT8 member_index, UINT8 argc)
{
    ssize_t bytes_read;
    char* buf = malloc(512);
    size_t content_length = 0;
    //read through archive until at the end
    while ((bytes_read = read(self->arc_fd, buf, 512)) > 0)
    {
        printf("bytes read = %lu\n", bytes_read);
        //read archive and create archiveHeader obj
        archiveHeader header = createHeader();
        if (loadHeaderData(header, buf, FROMARCH) == FATALERR)
            return FATALERR;
        content_length = strtol(header->size, NULL, 8);
        // handle archive entry extraction
        if (member_index == argc || isSelectArchMember(argv, member_index, argc, header->name))
        {
            printf("archMember was specified");
            if (xtractEntry(self, header, content_length) == FATALERR)
            {
                return FATALERR;
            }
        }
        else
        {
            lseek(self->arc_fd, content_length, SEEK_CUR);
        }
        //move file offset to next entry in case of extra padding after the content or if the offset is at the end move to end of file
        unsigned int garbage_zeroes = 0;
        while ((bytes_read = read(self->arc_fd, buf, 1)) > 0)
        {
            garbage_zeroes++;
            if (buf[0] != '\0' && buf[0] != '\n')
                break;
        }
        if (bytes_read > 0 && garbage_zeroes > 0)
            lseek(self->arc_fd, -1, SEEK_CUR);
        //printHeader(header);
        deleteHeader(header);
    }
    if (bytes_read == -1)
    {
        fprintf(stderr , "read failed in extractArchive: %s\n", strerror(errno));
        return FATALERR;
    }
    free(buf);
    buf = NULL;
    return SUCCESS;
        //handle type (dir, file, )
}
UINT8 xtractEntry(Archive self, archiveHeader h, size_t length)
{
    mode_t mode = strtol(h->mode, NULL, 8);
    //printf("the mode = %u\n",  mode);
    if (h->typeflag == DIRTYPE)
    {
        if (mkdir(h->name, 0777) == -1)
        {
            if (errno != EEXIST)
            {
                fprintf(stderr , "Error in mkdir(%s, %u). xtractEntry: %s\n", h->name, mode, strerror(errno));
                return FATALERR;
            }
        }
        printf("mode = %u\n", mode);
    }
    else if (h->typeflag == SYMTYPE)
    {
        // delete link if it already exists so we can make a new one, if it doesn't then do nothing
        if (unlink(h->name) == -1)
        {
            if (errno != ENOENT)
            {
                fprintf(stderr , "Error unlinkinng symblink in xtractEntry: %s\n", strerror(errno));
                return FATALERR;
            }
            errno = 0;
        }
        if (symlink(h->linkname, h->name) == -1)
        {
            fprintf(stderr , "Error in symblink in xtractEntry: %s\n", strerror(errno));
            return FATALERR;
        }

    }
    else if (h->typeflag == REGTYPE)
    {
        if (writeContentToFile(self, h->name, length, mode) == FATALERR)
        {
            return FATALERR;
        }
    }
    return SUCCESS;
}
UINT8 writeContentToFile(Archive self, const char* filepath, size_t length, mode_t mode)
{
    int fd;
    if ((fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1)
    {
        fprintf(stderr , "open(%s) failed in writeContentToFile: %s\n", filepath, strerror(errno));
        return FATALERR;
    }
    char buf[BUFFER_SIZE];
    ssize_t bytes_read = 0;
    size_t content_length = length;
    // read full buffers from archive and write to new file
    while ( content_length > BUFFER_SIZE)
    {
        if ((bytes_read = read(self->arc_fd, buf, BUFFER_SIZE)) == -1)
        {
            fprintf(stderr , "read failed writeContentToFile: %s\n", strerror(errno));
            return FATALERR;
        }
        write(fd, buf, bytes_read);
        content_length -= BUFFER_SIZE;
    }
    // write remaining length smaller than buffer size and greater than 0 to the file
    if (content_length > 0)
    {
        if ((bytes_read = read(self->arc_fd, buf, content_length)) == -1)
        {
            fprintf(stderr , "read failed writeFileToArchive: %s\n", strerror(errno));
            return FATALERR;
        }
        write(fd, buf, bytes_read);
    }
    close(fd);
    return SUCCESS;
}
/***********************************
|                                  |
|                                  |
|   Archive Update Functions
|                                  |
|                                  |
************************************/

UINT8 updateArchive(Archive self, const char** paths, UINT8 arch_member_start_index, UINT8 argc)
{
    ssize_t bytes_read;
    char* buf = malloc(512);
    size_t content_length = 0;
    //open file descriptor to beginning of archive
    int start_fd = open(self->name, O_RDONLY);
    if (start_fd == -1)
        return FATALERR;
    int curr_block_count = 0;
    while ((bytes_read = read(start_fd, buf, BLOCK_SIZE)) > 0 && curr_block_count <= self->block_count)
    {
        archiveHeader header = createHeader();
        if (loadHeaderData(header, buf, FROMARCH) == FATALERR)
            return FATALERR;
        curr_block_count++;
        content_length = strtol(header->size, NULL, 8);
        size_t content_factor = (content_length) ? (1 + content_length/BLOCK_SIZE) : 0;
        
        if (isSelectArchMember(paths, arch_member_start_index, argc, header->name))
        {
            printf("found an archive member specified from input\n");
            struct stat file_info;
            if (lstat(header->name, &file_info) == -1)
            {
                fprintf(stderr , "lstat(%s) failed in getHeaderDataFromFile: %s\n", header->name, strerror(errno));
                return FATALERR;
            }
            if (file_info.st_mtime > strtol(header->mtime, NULL, 8))
            {
                printf("creating new archive entry at what is supposed to be the end of it\n");
                if (createArchiveEntry(self, header->name) == FATALERR)
                    return FATALERR;
            }
            else
            {
                lseek(start_fd, 512 * content_factor, SEEK_CUR);
                curr_block_count += content_factor;
            }
        }
        else
        {
            lseek(start_fd, 512 * content_factor, SEEK_CUR);
            curr_block_count += content_factor;
        }
        //printf("content_length = %s\n, content factor = %u, curr_block_count = %d\n", header->name, self->block_count, curr_block_count);
        deleteHeader(header);
    }
    free(buf);
    close(start_fd);
    buf = NULL;
    return SUCCESS;
}
/***********************************
|                                  |
|                                  |
|   Archive List Functions
|                                  |
|                                  |
************************************/
UINT8 listArchive(Archive self)
{
    char buf[BLOCK_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(self->arc_fd, buf, BLOCK_SIZE)) > 0)
    {
        if (!*buf)
            break;
        archiveHeader header = createHeader();
        if (loadHeaderData(header, buf, FROMARCH) == FATALERR)
            return FATALERR;
        size_t content_length = strtol(header->size, NULL, 8);
        size_t content_factor = (content_length) ? (1 + content_length/BLOCK_SIZE) : 0;
        if (write(1, header->name, 100) == -1)
            return FATALERR;
        if (write(1, "\n", 1) == -1)
            return FATALERR;
        if (content_factor)
            lseek(self->arc_fd, content_factor * BLOCK_SIZE,SEEK_CUR);
        deleteHeader(header);
    }
    return SUCCESS;
}