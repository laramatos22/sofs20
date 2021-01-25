#include "fileblocks.h"

#include "daal.h"
#include "core.h"
#include "devtools.h"

#include <string.h>
#include <inttypes.h>

namespace sofs20
{
    void grpWriteFileBlock(int ih, uint32_t fbn, void *buf)
    {
        soProbe(332, "%s(%d, %u, %p)\n", __FUNCTION__, ih, fbn, buf);

        /* replace the following line with your code */
        //binWriteFileBlock(ih, fbn, buf);

        //write a file block
        //usar a função soGetFileBlock
        uint32_t nblock = soGetFileBlock(ih,fbn);

        //Data is written into a specific file block of an in-use inode
        /**If the referred block has not been allocated yet, it should be allocated
        now so that the data can be stored as its contents.*/
        //Caso o bloco esteja vazio
        if (nblock == BlockNullReference)
        {
            //O conteúdo é alocado nesse bloco
            nblock = soAllocFileBlock(ih, fbn);
        }
              
        soWriteDataBlock(nblock, buf);
        

    }
};

