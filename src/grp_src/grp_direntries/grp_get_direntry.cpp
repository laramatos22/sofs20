#include "direntries.h"

#include "core.h"
#include "devtools.h"
#include "daal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace sofs20
{
    uint16_t grpGetDirentry(int pih, const char *name)
    {
        soProbe(201, "%s(%d, %s)\n", __FUNCTION__, pih, name);

        /* replace the following line with your code */
        //return binGetDirentry(pih, name);

        //Get the inode associated to a given name. 

        //pih is a valid inode handler of a directory
        SOInode *inode = soGetInodePointer(pih);
        SODirentry dir[DPB];

        //name is a valid base name (it doesn't contains '/')
        if (strcmp(name, "/") == 0)
        {
            throw SOException(EINVAL, __FUNCTION__);
        }
        

        //uint32_t nblock = (*inode).size/BlockSize;

        for (uint32_t i = 0; i < inode->size ; i++)
        {
            soReadFileBlock(pih, i, dir);

            for (uint32_t j = 0; j < DPB; j++)
            {
                if (strcmp(name, dir[j].name) == 0)
                {
                    //returns the corresponding inode number
                    return dir[j].in;
                }
                
            }
            
        }

        //or returns InodeNullReference, if name doesn't exist
        return InodeNullReference;

    }
};

