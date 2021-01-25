#include "direntries.h"

#include "core.h"
#include "devtools.h"
#include "daal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <string.h>
#include <errno.h>
#include <sys/stat.h>

namespace sofs20
{
    void grpRenameDirentry(int pih, const char *name, const char *newName)
    {
        soProbe(204, "%s(%d, %s, %s)\n", __FUNCTION__, pih, name, newName);
        SOInode* pin=soGetInodePointer(pih);
        uint32_t block_index=BlockNullReference;
        uint32_t entry_index=BlockNullReference;

        for (uint32_t i=0;i<(pin->blkcnt);i++) { //iterate through all blocks to find entry
            SODirentry entries[DPB];
            soReadFileBlock(pih,i,entries);
            for (uint32_t j=0;j<DPB;j++) {
                if (strcmp(newName,entries[j].name)==0) {
                    throw SOException(EEXIST, __FUNCTION__);
                }
                if (strcmp(name,entries[j].name)==0) {
                    block_index=i;
                    entry_index=j;
                }
            }
        }
        SODirentry entries[DPB];

        if (block_index==BlockNullReference) {
            throw SOException(ENOENT,__FUNCTION__);
        }

        strcpy(entries[entry_index].name,newName);
        soWriteFileBlock(pih,block_index,entries);

        /* replace the following line with your code */
        // binRenameDirentry(pih, name, newName);
    }
};

