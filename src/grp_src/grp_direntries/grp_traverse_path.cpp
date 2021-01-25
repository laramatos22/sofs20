#include "direntries.h"

#include "core.h"
#include "devtools.h"
#include "daal.h"
#include "fileblocks.h"
#include "direntries.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

namespace sofs20
{    
    uint16_t grpTraversePath(char *path)
    {
        soProbe(221, "%s(%s)\n", __FUNCTION__, path);

        /* replace the following line with your code */
        //return binTraversePath(path);

         if(strcmp(path, "/") == 0)
            return 0;

        char *baseName = basename(strdupa(path));
        char *dirName = dirname(strdupa(path));
    
        uint32_t inode = grpTraversePath(dirName);

        uint32_t ih = soOpenInode(inode);
        SOInode* ip = soGetInodePointer(ih);

        //if one of the path components, with the exception 
        //of the rightmost one, is not a directory 
        if((ip->mode & S_IFDIR) != S_IFDIR)
            throw SOException(ENOTDIR, __FUNCTION__);

        //if the process that calls the operation does not have 
        //traverse (x) permission on all the components 
        //of the path, with exception of the rightmost one
        if(!soCheckInodeAccess(ih, X_OK))
            throw SOException(EACCES, __FUNCTION__);
        
        uint32_t in = soGetDirentry(ih,baseName);

        if(in == InodeNullReference)
            throw SOException(ENOENT, __FUNCTION__);
            //Error NO ENTry

        soCloseInode(ih);
        return in;


    }
};

