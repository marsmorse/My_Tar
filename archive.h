#include <stddef.h>
#include <stdint.h>
#include <grp.h>
#include <pwd.h>
#include "ArchiveHeader.h"

typedef uint_fast8_t UINT8;
typedef struct archiveObj* Archive;
typedef struct flagObj* flag;

//constructor
Archive newArchive (const char* , char);
//destructor
void deleteArchive(Archive );

UINT8 createArchive(Archive ,const char** , UINT8, UINT8);
UINT8 extractArchive(Archive ,const char**, UINT8, UINT8);

UINT8 listArchive(Archive );
UINT8 updateArchive(Archive , const char** , UINT8 , UINT8 );
size_t getBlockCount(const char*);

UINT8 createArchiveEntry(Archive ,const char* );
UINT8 writeFileToArchive(Archive ,const char* );


UINT8 writeHeaderToArchive(Archive , archiveHeader );
/* 
    extraction functions
*/
UINT8 isSelectArchMember(const char** , UINT8, UINT8, char*);
UINT8 xtractEntry(Archive, archiveHeader, size_t);
UINT8 writeContentToFile(Archive , const char* , size_t , mode_t );
// function to convert a number as first argument to string of base specified by the second argument
/* 
    creation functions
*/
UINT8 writeHeaderEntry(int , char* , UINT8 , UINT8);
void writeXattr( Archive self, char* filepath);
/* 
    Debugging functions
*/

