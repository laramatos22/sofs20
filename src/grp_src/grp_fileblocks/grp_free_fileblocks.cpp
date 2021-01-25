#include "fileblocks.h"

#include "freedatablocks.h"
#include "daal.h"
#include "core.h"
#include "devtools.h"

#include <inttypes.h>
#include <errno.h>
#include <assert.h>

namespace sofs20
{

    /* free all blocks between positions ffbn and RPB - 1
     * existing in the block of references given by i1.
     * Return true if, after the operation, all references become BlockNullReference.
     * It assumes i1 is valid.
     */
    static bool grpFreeIndirectFileBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn);

    /* free all blocks between positions ffbn and RPB**2 - 1
     * existing in the block of indirect references given by i2.
     * Return true if, after the operation, all references become BlockNullReference.
     * It assumes i2 is valid.
     */
    static bool grpFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn);

    /* ********************************************************* */

    void grpFreeFileBlocks(int ih, uint32_t ffbn)
    {
        soProbe(303, "%s(%d, %u)\n", __FUNCTION__, ih, ffbn);

        // Precondition: ih is a valid handler of an inode in-use...
        // soCheckInodeHandler(ih, __FUNCTION__); Não implementado??

        // aceder ao inode
        SOInode *inode = soGetInodePointer(ih);
                
        // libertar File Blocks em d, começando no bloco ffbn
        //     d: direct references to the first data blocks with file's data
        for(; ffbn<N_DIRECT; ffbn++) {
            // se o inode estiver a usar o bloco...
            if (inode->d[ffbn] != BlockNullReference) {
                // liberta data    
                soFreeDataBlock(inode->d[ffbn]);
                // muda referência
                inode->d[ffbn] = BlockNullReference;
                // atualiza metadata
                inode->blkcnt--;
            }
        }

        // modificar número de ffbn para poder ser usado como referência em i1[]
        ffbn -= N_DIRECT;

        // libertar File Blocks em i1, começando no bloco ffbn
        //     i1: references to block(s) that extend the \c d array
        for(uint32_t j=0; j<N_INDIRECT; j++) {
            // se o inode estiver a usar o bloco...
            if(inode->i1[j] != BlockNullReference) {
                // free all blocks between positions ffbn and RPB - 1
                bool clear_block = grpFreeIndirectFileBlocks(inode, inode->i1[j], ffbn);
                // libertar i[i] caso todas as referências sejam BlockNullReference
                if(clear_block) {
                    soFreeDataBlock(inode->i1[j]);
                    inode->i1[j] = BlockNullReference;
                    inode->blkcnt--;
                }
            }

            // atualizar número de ffbn
            if (ffbn > RPB) {
                ffbn -= RPB;
            } else {
                ffbn = 0;
            }

        }

        // libertar File Blocks em i2, começando no bloco ffbn
        //     i2: references to block(s) that extends the \c i1 array
        for(uint32_t j=0; j<N_DOUBLE_INDIRECT; j++) {
            // se o inode estiver a usar o bloco...
            if(inode->i2[j] != BlockNullReference) {
                // free all blocks between positions ffbn and RPB*RPB - 1
                bool clear_block = grpFreeDoubleIndirectFileBlocks(inode, inode->i2[j], ffbn);
                // libertar i[j] caso todas as referências sejam BlockNullReference
                if(clear_block) {
                    soFreeDataBlock(inode->i2[j]);
                    inode->i2[j] = BlockNullReference;
                    inode->blkcnt--;
                }
            }

            // atualizar número de ffbn
            if (ffbn > RPB*RPB) {
                ffbn -= RPB*RPB;
            } else {
                ffbn = 0;
            }
        }

        //binFreeFileBlocks(ih, ffbn);
    }

    /* ********************************************************* */

    static bool grpFreeIndirectFileBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn)
    {
        soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i1, ffbn);

        // array de referências a file blocks
        uint32_t ref[RPB];
        // ler as referências do bloco
        soReadDataBlock(i1, ref);
        
        // referência para a posição seguinte. Todos os blocos de data seguintes vão ser libertados...
        // ... esta referência vai ser lida em reverse; se exitir algum bloco não null então retorna false; 
        uint32_t reverse = ffbn+1;
        
        // percorre bloco de referências ...
        for(; ffbn<RPB; ffbn++) {
            // se referência for ocupada ...
            if(ref[ffbn] != BlockNullReference) {
                // liberta bloco ...
                soFreeDataBlock(ref[ffbn]);
                // atualiza referência
                ref[ffbn] = BlockNullReference;
                // atualiza inode
                ip->blkcnt--;
            }
        }

        // escrever novo buffer    
        soWriteDataBlock(i1, ref);
        
        // percorre direção oposta do array
        for(; reverse>0; reverse--) {
            // se alguma posição estiver ocupada então array não está todo livre
            if(ref[reverse-1] != BlockNullReference) {
                return false;
            }
        }
        // retorna true se todas as posições para a frente e traz de ffbn forem BlockNullReference        
        return true;

        //throw SOException(ENOSYS, __FUNCTION__); 
    }

    /* ********************************************************* */

    static bool grpFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn)
    {
        soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i2, ffbn);

        // array de referências a file blocks
        uint32_t ref[RPB];
        
        // ler as referências do bloco
        soReadDataBlock(i2, ref);

        // referência para a posição seguinte. Todos os blocos de data seguintes vão ser libertados...
        // ... esta referência vai ser lida em reverse; se exitir algum bloco não null então retorna false;
        uint32_t reverse = (ffbn/RPB)+1;

        // libertar File Blocks em i2, começando no bloco ffbn
        //     i2: references to block(s) that extends the \c i1 array


        // #########################################################################

        // libertar File Blocks em i2
        for(uint32_t j=0; j<RPB; j++) {
            if(ref[j] != BlockNullReference and ffbn<RPB) {
                bool clear_block = grpFreeIndirectFileBlocks(ip, ref[j], ffbn);
                // libertar i2[i] caso necessário
                if(clear_block) {
                    soFreeDataBlock(ref[j]);
                    ref[j] = BlockNullReference;
                    ip->blkcnt--;
                }
            }
            // atualizar número de ffbn
            if (ffbn > RPB) {
                ffbn -= RPB;
            } else {
                ffbn = 0;
            }
        }

        // #########################################################################

        // escrever novo buffer    
        soWriteDataBlock(i2, ref);

        // percorre direção oposta do array
        for(; reverse>0; reverse--) {
            // se alguma posição estiver ocupada então array não está todo livre
            if(ref[reverse-1] != BlockNullReference) {
                return false;
            }
        }       
        return true;

        throw SOException(ENOSYS, __FUNCTION__); 
    }

    /* ********************************************************* */
};

