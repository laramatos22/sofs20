/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2009-2020
 */

#include "freedatablocks.h"

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "core.h"
#include "devtools.h"
#include "daal.h"

namespace sofs20
{
    uint32_t grpAllocDataBlock()
    {
        soProbe(441, "%s()\n", __FUNCTION__);

        /* replace the following line with your code */
        // return binAllocDataBlock();
        
        SOSuperblock* SB = soGetSuperblockPointer(); // Super block buff 

        // Throw ENOSPC if there are no free DB
        if(SB->dbfree == 0)
            throw SOException(ENOSPC, __FUNCTION__);
        

        // If retrieval cache is empty
        if(SB->retrieval_cache.idx == REF_CACHE_SIZE)
            soReplenishRetrievalCache();
        
        uint32_t idx = SB->retrieval_cache.idx;
        uint32_t ref = SB->retrieval_cache.ref[idx];
        SB->retrieval_cache.ref[idx] = BlockNullReference;
        SB->retrieval_cache.idx += 1;
        SB->dbfree--;

        soSaveSuperblock();

        return ref;
    }
};
