/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2020
 */

#include "freeinodes.h"

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
    void grpFreeInode(uint16_t in)
    {
        soProbe(402, "%s(%u)\n", __FUNCTION__, in);

        SOSuperblock* psb = soGetSuperblockPointer();
        
        if(in>psb->itotal){
            throw SOException(EINVAL, __FUNCTION__);
        }
        
        int inode_handler = soOpenInode(in);
        SOInode* pinode = soGetInodePointer(inode_handler);
        
        pinode->mode = 0;
        pinode->lnkcnt = 0;
        pinode->owner = 0;
        pinode->group = 0;
        pinode->size = 0;
        pinode->blkcnt = 0;
        pinode->atime = 0;
        pinode->ctime = 0;
        pinode->mtime = 0;

        for (uint32_t i = 0; i < N_DIRECT; i++)
        {
            pinode->d[i] = BlockNullReference;    
        }
       
        for (uint32_t i = 0; i < N_INDIRECT; i++)
        {
            pinode->i1[i] = BlockNullReference; 
        }
        
        for (uint32_t i = 0; i < N_DOUBLE_INDIRECT; i++)
        {
            pinode->i2[i] = BlockNullReference; 
        }
        
        uint32_t bloco = in/32;
        uint32_t pos = in%32;
        uint32_t one = 1;
        uint32_t bit = ~(one<<pos);
        psb->ibitmap[bloco] = psb->ibitmap[bloco] & bit;
        psb->ifree = psb->ifree + 1;
        // uint32_t lastiidx = 0;
        // if(psb->iidx == in){
        //     for (uint32_t i = 0; i < psb->itotal; i++){
        //         uint32_t position = psb->ibitmap[i/32]>>(i%32)& 0x1;
        //         if(position == 1){
        //             lastiidx = i;
        //         }
        //     }       
        //     psb->iidx = lastiidx;
        // } 
        

        soSaveInode(inode_handler);
        soSaveSuperblock();
        
        /* replace the following line with your code */
        // binFreeInode(in);
    }
};

