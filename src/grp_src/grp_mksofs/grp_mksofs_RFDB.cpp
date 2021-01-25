#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "devtools.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>

namespace sofs20
{
    void grpResetFreeDataBlocks(uint32_t ntotal, uint32_t itotal, uint32_t dbtotal)
    {
        soProbe(607, "%s(%u, %u, %u)\n", __FUNCTION__, ntotal, itotal, dbtotal);

        /* replace the following line with your code */
        // binResetFreeDataBlocks(ntotal, itotal, dbtotal);
        
        // Buffer 
        u_int32_t buff[RPB] = {};
        
        u_int32_t nBlocksInodes = itotal/IPB;

        // Fill buffer with 0's
        // memset(buff, 0, sizeof(buff));

        for(u_int32_t i=nBlocksInodes+2; i<dbtotal+nBlocksInodes+1; i++) {
            soWriteRawBlock(i, buff);
        }
    }
};

