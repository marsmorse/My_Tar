#include <grp.h>
#include <pwd.h>
#include <sys/sysmacros.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "ArchiveHeader.h"

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION " 0"           /* 00 and no null */
#define TVERSLEN 2

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

#define FATALERR 2
#define SUCCESS 0
/*---------------*

    ructor and destructor

*---------------*/
archiveHeader createHeader()
{
    archiveHeader h = malloc(sizeof(posix_headerObj));
    return h;
}
void deleteHeader(archiveHeader h)
{
    free(h);
    h = NULL;
}
/*---------------*

    CORE FUNCTIONS

*---------------*/
//    getHeaderData: populates the archiveHeader struct with data from a the passed in path to a 
// file dir or link. 
UINT8 loadHeaderData(archiveHeader h, char* path, UINT8 mode)
{
    UINT8 result = SUCCESS;
    if (mode == FROMFILE)
    {
        result = getHeaderDataFromFile(h, path);
    }
    else if (mode == FROMARCH)
    {   
        result = getHeaderDataFromArch(h, path);
    }
    return result;
}
UINT8 getHeaderDataFromFile(archiveHeader header,  char* filepath)
{
    struct stat file_info;
    if (lstat(filepath, &file_info) == -1) {
        fprintf(stderr , "lstat(%s) failed in getHeaderDataFromFile: %s\n", filepath, strerror(errno));
        return FATALERR;
    }
    char type = getTypeFlag(file_info);
    if (header)
    {
        (type == DIRTYPE) ? strncpy(header->name, strcat(filepath, "/"), 100) : strncpy(header->name, filepath, 100) ;
        strncpy(header->mode, itoa(file_info.st_mode & 0777, 8), 8);
        strncpy(header->uid, itoa(file_info.st_uid, 8), 8);
        strncpy(header->gid, itoa(file_info.st_gid, 8), 8);
        strncpy(header->size, (type != REGTYPE ? "\0\0\0\0\0\0\0\0\0\0\0\0": itoa(file_info.st_size, 8)), 12);
        strncpy(header->mtime, itoa(file_info.st_mtim.tv_sec, 8), 12);
        header->typeflag = type;
        strncpy(header->linkname, getLinkName(filepath, type), 100);
        //printf("archiveHeader linkname loaded as %s\n", header->linkname);
        strncpy(header->magic, TMAGIC, TMAGLEN);
        strcpy(header->version, "0");
        strncpy(header->uname, getUserName(file_info.st_uid), 32);
        strncpy(header->gname, getGroupName(file_info.st_gid), 32);
        char emptyblock[8] = {'\0'};
        strncpy(header->devmajor, (type == BLKTYPE) ? itoa(major(file_info.st_dev), 8) : emptyblock, 8);
        strncpy(header->devminor, (type == BLKTYPE) ? itoa(minor(file_info.st_dev), 8) : emptyblock, 8);
        char prefix[155] = {'\0'};
        strncpy(header->prefix, prefix, 155);
        unsigned int chksum = getChkSum(header);
        strncpy(header->chksum, itoa(chksum, 8), 8);
    }
    return SUCCESS;
}
/* getHeaderDataFromArchive: parses a buffer and fills an archiveHeader obj.
        input: 
            archive_buf = 500 size buffer containing an archive header info

*/
UINT8 getHeaderDataFromArch(archiveHeader header,  char* header_buf)
{
    char * arch_buf = header_buf;
    strncpy(header->name, arch_buf, 100);
    arch_buf += 100;
    strncpy(header->mode, arch_buf, 8);
    arch_buf += 8;
    strncpy(header->uid, arch_buf, 8);
    arch_buf += 8;
    strncpy(header->gid, arch_buf, 8);
    arch_buf += 8;
    strncpy(header->size, arch_buf, 12);
    arch_buf += 12;
    strncpy(header->mtime, arch_buf, 12);
    arch_buf += 12;
    strncpy(header->chksum, arch_buf, 8);
    arch_buf += 8;
    header->typeflag = *arch_buf;
    arch_buf += 1;
    strncpy(header->linkname, arch_buf, 100);
    arch_buf += 100;
    strncpy(header->magic, arch_buf, TMAGLEN);
    arch_buf += TMAGLEN;
    strncpy(header->version, arch_buf, 2);
    arch_buf += 2;
    strncpy(header->uname, arch_buf, 32);
    arch_buf += 32;
    strncpy(header->gname, arch_buf, 32);
    arch_buf += 32;
    strncpy(header->devmajor, arch_buf, 8);
    arch_buf += 8;
    strncpy(header->devminor, arch_buf, 8);
    arch_buf += 8;
    strncpy(header->prefix, arch_buf, 155);
    arch_buf += 155;
    return SUCCESS;
}
/*---------------*

    HELPER FUNCTIONS

*---------------*/
/*
|       calcButeSum: loops over a posixheader struct and returns the checksum
*/
/*
       getCheckSum: iterates over all fields, except type and checksum, of the input header struct and adds the sum of their bytes to the checksum
*/
unsigned int calcByteSums( char* header_field, size_t size, UINT8 isNumber)
{
    unsigned int sum = 0;
    for (int i = 0; i < (int)strlen(header_field); i++)
    {
        sum += (header_field[i]);
    }
    if (isNumber)
    {
        for (size_t i = 0; i < size - strlen(header_field); i++)
        {
            sum += '0';
        }
    }
    return sum;
}
unsigned int getChkSum(archiveHeader h)
{
    unsigned int chksum = 0;
    chksum += calcByteSums(h->name, sizeof(h->name), 0);
    chksum += calcByteSums(h->mode, sizeof(h->mode), 1);
    chksum += calcByteSums(h->uid, sizeof(h->uid), 1);
    chksum += calcByteSums(h->gid, sizeof(h->gid), 1);
    chksum += calcByteSums(h->size, sizeof(h->size), 1);
    chksum += calcByteSums(h->mtime, sizeof(h->mtime), 1);
    chksum += h->typeflag;
    chksum += calcByteSums(h->linkname, sizeof(h->linkname), 0);
    chksum += calcByteSums(h->magic, sizeof(h->magic), 0);
    chksum += '0';
    //chksum += '0';
    chksum += ' ';
    chksum += calcByteSums(h->uname, sizeof(h->uname), 0);
    chksum += calcByteSums(h->gname, sizeof(h->gname), 0);
    return chksum;
}

char* getUserName(uid_t uid)
{
    struct passwd * pw = getpwuid(uid);
    return pw->pw_name;
}
char* getGroupName(gid_t gid)
{
    struct group* gr;
    gr = getgrgid(gid);
    return gr->gr_name;
}
char* getLinkName( char* pathname, char type)
{
    char buf[100] = {'\0'};
    char* t = buf;
    ssize_t bytes_read;
    if (type == '2')
    {
        if ((bytes_read = readlink(pathname, buf, 100)) == -1)
        {
            fprintf(stderr , "readlink(%s) failed in readLinkName: %s\n", pathname, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return(t);
}
char getTypeFlag(struct stat file_info)
{
    if (S_ISREG(file_info.st_mode))
    {
        return REGTYPE;
    }
    else if (S_ISDIR(file_info.st_mode))
    {
        return DIRTYPE;
    }
    else if (S_ISCHR(file_info.st_mode))
    {
        return CHRTYPE;
    }
    else if (S_ISBLK(file_info.st_mode))
    {
        return BLKTYPE;
    }
    else if (S_ISFIFO(file_info.st_mode))
    {
        return FIFOTYPE;
    }
    else if (S_ISLNK(file_info.st_mode))
    {
        return SYMTYPE;
    }
    else
    {
        fprintf(stderr , "getTypeFlag found an invalid type: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
char* itoa(ssize_t val, int base)
{
	static char buf[32] = {0};
	int i = 30;
	for (; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];
}
void printHeader(archiveHeader h)
{
    printf("\n\nARCHIVE HEADER\n");
    printf("______________\n");
    printf("name: %s\n", h->name);
    printf("mode: %s\n", h->mode);
    printf("uid: %s\n", h->uid);
    printf("gid: %s\n", h->gid);
    printf("size: %s\n", h->size);
    printf("mtime: %s\n", h->mtime);
    printf("chksum: %s\n", h->chksum);
    printf("typeflag: %c\n", h->typeflag);
    printf("linkname: %s\n", h->linkname);
    printf("magic: %s\n", h->magic);
    printf("version: %s\n", h->version);
    printf("uname: %s\n", h->uname);
    printf("gname: %s\n", h->gname);
    printf("devmajor: %s\n", h->devmajor);
    printf("devminor: %s\n", h->devminor);
    printf("prefix: %s\n\n", h->prefix);
}