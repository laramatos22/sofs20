/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2020
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
    void grpFreeDataBlock(uint32_t bn)
    {
        soProbe(442, "%s(%u)\n", __FUNCTION__, bn);

        /* replace the following line with your code */
        //binFreeDataBlock(bn);

        SOSuperblock *sb; //pointer for a superblock

        sb = soGetSuperblockPointer(); //returns pointer to the superblock

        //bn must represent a valid data block number
        //EINVAL if data block number is not valid.
        if (bn > ((*sb).ntotal) || bn < 0)
        {
            throw SOException(EINVAL,__FUNCTION__);
        }

        //If the insertion cache is full, 
        //the deplete function must be called first        
        if(((*sb).insertion_cache.idx) == REF_CACHE_SIZE){
            soDepleteInsertionCache();
        }


        //The reference must be inserted at the first empty cell.
        ((*sb).insertion_cache.ref)[(*sb).insertion_cache.idx] = bn;

        //The data block meta data fields 
        //in the superblock must be updated.

        (*sb).dbfree++;
        (*sb).insertion_cache.idx++;

        //save superblock to disk
        soSaveSuperblock();
    }
};