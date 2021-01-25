#include "fileblocks.h"

#include "daal.h"
#include "core.h"
#include "devtools.h"

#include <errno.h>

namespace sofs20
{
    /* ********************************************************* */


    static uint32_t grpGetIndirectFileBlock(SOInode * ip, uint32_t fbn);
    static uint32_t grpGetDoubleIndirectFileBlock(SOInode * ip, uint32_t fbn);


    /* ********************************************************* */

    uint32_t grpGetFileBlock(int ih, uint32_t fbn)
    {
        uint32_t i1size = N_INDIRECT*RPB;                                           //Tamanho do i1
        uint32_t i2size = N_DOUBLE_INDIRECT*RPB*RPB;                                //Tamanho do i2

        if (fbn >= N_DIRECT+i1size+i2size) {                                        //se o fbn for maior que a soma dos tamanhos do d i1 i2                                              
            throw SOException(EINVAL, __FUNCTION__);
        }

        SOInode* pinode = soGetInodePointer(ih);
        uint32_t block_ref;

        if (fbn < N_DIRECT) {                                                       //se fbn for menor que o tamanho do d
            block_ref = pinode->d[fbn];                                              // guarda valor da referencia do bloco
        }
        else if (fbn < N_DIRECT+i1size) {                                           // se fbn for menor que a soma do tamanho do d e i1
            block_ref = grpGetIndirectFileBlock(pinode, fbn-N_DIRECT);               // guarda o valor da funçao grpGetIndirectFileBlock com valor de entrada de fbn-(tamanho de d) para pesquisar o index do i1
        }
        else {                                                                      // se fbn for menor que a soma do tamanho do d e i1 e i2
            block_ref = grpGetDoubleIndirectFileBlock(pinode, fbn-N_DIRECT-i1size);  // guarda o valor da funçao grpGetDoubleIndirectFileBlock com valor de entrada de fbn-(tamanho de d)-i1size para pesquisar o index do i2 e de i1referenceblock_index
        }
        return block_ref;                                                           // retorna o a referencia do bloco
    }

    /* ********************************************************* */


    static uint32_t grpGetIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);

        uint32_t i1_idx = afbn / RPB;                                               // index do bloco de referencias dentro do i1
        uint32_t ref_idx = afbn % RPB;                                              // index da refencia dentro bloco de refencias 
        uint32_t ref[RPB];                                                          // ponteiro para a referencia dentro do bloco

        if (ip->i1[i1_idx] == BlockNullReference) {                                 // se o bloco referencias for Null entao   
            return BlockNullReference;                                              // retorna BlockNullReference 0xFFFFFFFF
        }

        soReadDataBlock(ip->i1[i1_idx], &ref);                                      //ler o datablock e guarda o ponteiro para a referencia no buffer
        return ref[ref_idx];                                                        //retorna a refencia do bloco
    }


    /* ********************************************************* */

    static uint32_t grpGetDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);

        uint32_t i2_idx = afbn / (RPB*RPB);                                         //index para o bloco de bloco de dados
        uint32_t referenceblock_idx = (afbn / RPB) % RPB;                           //index do bloco de referencias dentro do bloco referenciado no i2
        uint32_t rdb_index = afbn % RPB;                                            //index da referencia do bloco dentro do bloco de referencia
        uint32_t ref_i2[RPB];                                                       //ponteiro para o bloco de referencias dentro do i2
        uint32_t ref[RPB];                                                          //ponteiro para a referencia dentro do bloco de referencias

        if (ip->i2[i2_idx] == BlockNullReference) {                                 //se o bloco de referencias for null
            return BlockNullReference;                                              // retorna BlockNullReference 0xFFFFFFFF
        }
        
        soReadDataBlock(ip->i2[i2_idx], &ref_i2);                                   //ler o datablock e guarda o ponteiro para o blocode referencias no buffer                                   
        if (ref_i2[referenceblock_idx] == BlockNullReference) {                     // se o bloco referencias for Null entao 
            return BlockNullReference;                                              // retorna BlockNullReference 0xFFFFFFFF
        }

        soReadDataBlock(ref_i2[referenceblock_idx], &ref);                          //ler o datablock e guarda o ponteiro para a referencia no buffer
        return ref[rdb_index];                                                      // retorna o a referencia do bloco
    }
};

