#include <stddef.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>

#define FROMFILE 1
#define FROMARCH 2

typedef uint_fast8_t UINT8;
typedef struct posix_headerObj
{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
                                /* 500 */
} posix_headerObj;
typedef posix_headerObj* archiveHeader;



/*---------------*

    Constructor and destructor

*---------------*/
archiveHeader createHeader();
void deleteHeader(archiveHeader);
/*---------------*

    CORE FUNCTIONS

*---------------*/
UINT8 getHeaderDataFromFile(archiveHeader , char*);
UINT8 getHeaderDataFromArch(archiveHeader , char*);
UINT8 loadHeaderData(archiveHeader, char* path, UINT8 mode);
/*---------------*

    HELPER FUNCTIONS

*---------------*/
//checksum helpers
unsigned int calcByteSums( char* , size_t , UINT8 );
unsigned int getChkSum(archiveHeader);

//general helpers
char* itoa(ssize_t , int);
char getTypeFlag(struct stat file_info);
char* getGroupName(gid_t);
char* getUserName(uid_t);
char* getLinkName(char*, char);
// Debugging functions
void printHeader(archiveHeader);