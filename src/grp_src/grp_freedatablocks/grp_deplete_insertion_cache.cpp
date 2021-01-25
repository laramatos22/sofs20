#include "freedatablocks.h"

#include "core.h"
#include "devtools.h"
#include "daal.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <math.h>
using namespace std;

namespace sofs20
{
    /* only fill the current block to its end */
    void grpDepleteInsertionCache(void)
    {
        soProbe(444, "%s()\n", __FUNCTION__);
        //binDepleteInsertionCache();

        SOSuperblock *sb  = soGetSuperblockPointer();
        uint32_t rt_last_idx;
        uint32_t rt_last_block_idx;
        uint32_t rt_last_block;
        uint32_t free_ref; 
        uint32_t refs_to_move;    

        if(sb->insertion_cache.idx != REF_CACHE_SIZE)
        {
          return;
        }
        else
        {
          rt_last_idx = sb->reftable.ref_idx + sb->reftable.count; 
          rt_last_block_idx = rt_last_idx % RPB;
          rt_last_block = (sb->reftable.blk_idx + rt_last_idx/RPB)%sb->rt_size;
          uint32_t *ref = soGetReferenceBlockPointer(rt_last_block); 
          free_ref = RPB - rt_last_block_idx;

          if(free_ref < REF_CACHE_SIZE)
          {
            refs_to_move = free_ref *  sizeof(uint32_t);
            sb->reftable.count = sb->reftable.count + free_ref;
            sb->insertion_cache.idx = REF_CACHE_SIZE-free_ref;
            memcpy(&ref[rt_last_block_idx] , sb->insertion_cache.ref , refs_to_move);
            refs_to_move = (REF_CACHE_SIZE-free_ref) *  sizeof(uint32_t);
            memcpy(sb->insertion_cache.ref, &sb->insertion_cache.ref[REF_CACHE_SIZE-free_ref] , refs_to_move);
            memset(sb->insertion_cache.ref + (REF_CACHE_SIZE-free_ref) , BlockNullReference , refs_to_move);
          }
          else 
          {
            refs_to_move = REF_CACHE_SIZE *  sizeof(uint32_t);
            sb->reftable.count = sb->reftable.count + REF_CACHE_SIZE;
            sb->insertion_cache.idx = 0; 
            memcpy(&ref[rt_last_block_idx] , sb->insertion_cache.ref , refs_to_move);
            memset(sb->insertion_cache.ref, BlockNullReference , refs_to_move);
          }
        }
        soSaveReferenceBlock();
        soSaveSuperblock();
    }
};

