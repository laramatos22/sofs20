/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2020
 */

#include "freedatablocks.h"

#include <string.h>
#include <errno.h>
#include <iostream>

#include "core.h"
#include "devtools.h"
#include "daal.h"


namespace sofs20
{
    void grpReplenishRetrievalCache(void)
    {
        soProbe(443, "%s()\n", __FUNCTION__);

        /* replace the following line with your code */
        //binReplenishRetrievalCache();
         
		 
        SOSuperblock* sb = soGetSuperblockPointer();
        
        if(sb->retrieval_cache.idx < REF_CACHE_SIZE)				
        {
			return;
		}
		else {
			
			if(sb->reftable.count == 0)		// se nao existirem referencias na reference table				
			{
				uint32_t block = sb->insertion_cache.idx;
				uint32_t block_ref = block*sizeof(uint32_t);
				memcpy(&sb->retrieval_cache.ref[REF_CACHE_SIZE-block],&sb->insertion_cache.ref[block],block_ref);
				memset(&sb->insertion_cache.ref[block],BlockNullReference,block_ref);
				sb->insertion_cache.idx = 0;
				sb->retrieval_cache.idx = REF_CACHE_SIZE-block;
				
			}
			else {				
				
				uint32_t* rt = soGetReferenceBlockPointer(sb->reftable.blk_idx);
				uint32_t block = RPB-sb->reftable.ref_idx;
				
				if(block < sb->reftable.count)
				{
					if(block > REF_CACHE_SIZE)
					{
						block = REF_CACHE_SIZE;
					}
				}
				else 
				{
					if(sb->reftable.count < REF_CACHE_SIZE)
					{
						block = sb->reftable.count;
					
					}
					else 
					{
						block = REF_CACHE_SIZE;
					
					}
				}
				
				uint32_t block_ref = block*sizeof(uint32_t);
				memcpy(&sb->retrieval_cache.ref[REF_CACHE_SIZE-block],&rt[sb->reftable.ref_idx],block_ref);				
				memset(&rt[sb->reftable.ref_idx],BlockNullReference,block_ref);
				uint32_t tmp = (sb->reftable.ref_idx + block);
				
				if(tmp < RPB)
				{
					sb->reftable.ref_idx = tmp;
					sb->reftable.blk_idx = sb->reftable.blk_idx;
				
				}
				else 
				{
					sb->reftable.ref_idx = tmp%RPB;
					sb->reftable.blk_idx = (sb->reftable.blk_idx+1)%sb->rt_size;
					
				}
				
				sb->reftable.count = sb->reftable.count - block;
				sb->retrieval_cache.idx = REF_CACHE_SIZE-block;
				
				soSaveReferenceBlock();
			} 
		}
		soSaveSuperblock();

    }
};