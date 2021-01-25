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
    uint16_t grpDeleteDirentry(int pih, const char *name)
    {
        soProbe(203, "%s(%d, %s)\n", __FUNCTION__, pih, name);

        // precondition: pih is a valid inode handler of a directory
        //soCheckInodeHandler(pih);

        // precondition: where the user has write access
        if(!soCheckInodeAccess(pih, S_IWOTH))
            throw SOException(ENOENT, __FUNCTION__);
        
        // aceder ao inode
        SOInode *pinode = soGetInodePointer(pih);

        // abrir array de diretórios
        SODirentry pdirentry[DPB];

        // enquanto existirem entradas no bloco
        for(uint32_t i=0; i<pinode->blkcnt; i++) {

            soReadFileBlock(pih, i, pdirentry);
            
            for(uint32_t j=0; j<DPB; j++){
                
                if(strcmp(name, pdirentry[j].name) == 0) {

                    uint32_t in = pdirentry[j].in;
                    pdirentry[j].in = '\0';
                    
                    strncpy(pdirentry[j].name, "\0", SOFS20_FILENAME_LEN +1);

                    // escrever nova direntry
                    soWriteFileBlock(pih, i, pdirentry);
                    return in;
                }
            }
        }

        // name does not exist
        throw SOException(ENOENT, __FUNCTION__);
        
        //return binDeleteDirentry(pih, name);
    }
};

