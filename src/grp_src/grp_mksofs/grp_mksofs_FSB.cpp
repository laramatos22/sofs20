#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "devtools.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>

namespace sofs20
{
    void grpFillSuperblock(const char *name, uint32_t ntotal, uint32_t itotal, uint32_t dbtotal)
    {
        soProbe(602, "%s(%s, %u, %u, %u)\n", __FUNCTION__, name, ntotal, itotal, dbtotal);
        
        SOSuperblock sb;
        uint32_t ref = 1;
        sb.magic = 0xFFFF;                                          // Magic Number
        sb.version = VERSION_NUMBER;                                // numero da versao
        strncpy(sb.name, name, PARTITION_NAME_LEN);                 // Nome da partiçao
        sb.mntstat = 0;                                             // Numero de Mounts
        sb.ntotal = ntotal;                                         // Numero total de blocos
        sb.itotal = itotal;                                         // Numero total de inodes
        sb.ifree = itotal-1;                                        // Numero total de inodes livres
        sb.iidx = 0;                                                // Ultimo Inode alocado
        sb.dbp_start = (itotal/IPB)+1;                              // Primeiro bloco do Data Block Pool
        sb.dbtotal = dbtotal;                                       // Numero total de data blocos
        sb.dbfree = dbtotal-1;                                      // Numero total de data blocos livres            
        sb.reftable.blk_idx = 0;                                    // Index do primeiro bloco com referencias
        sb.reftable.ref_idx = 0;                                    // Index da primeira celula dentro do primeiro bloco com referencias
        
        if (sb.dbfree > REF_CACHE_SIZE){                            // Se o numero de data blocos livres for maior que o tamanho da cache:
            sb.retrieval_cache.idx = 0;                             //      -> Define o idx da retrivial cache a 0
            sb.reftable.count = (dbtotal-1)-REF_CACHE_SIZE;         //      -> Define o Numero de referencias dentro da tabela de referencias
            sb.rt_size = (ntotal-1)-dbtotal-(itotal/IPB);           //      -> Define o Tamanho da tabela de referencias
            sb.rt_start = ntotal-sb.rt_size;                        //      -> Define o Bloco onde começa a tabela de referencias
                                                                    //
        }else{                                                      // Se não:
            sb.retrieval_cache.idx = REF_CACHE_SIZE-sb.dbfree;      //      -> Define o idx da retrivial cache na posiçao do bloco mais antigo 
            sb.reftable.count = 0;                                  //      -> Define o Numero de referencias dentro da tabela a 0   
            sb.rt_size = 0;                                         //      -> Define o Tamnho da tabela de referencia a 0
            sb.rt_start = ntotal;                                   //      -> Define o Bloco onde começa a tabela de referencias como o numer total de blocos (isto esta a fucionar como o codigo do prof)
        }
        for (uint32_t i = 0; i < REF_CACHE_SIZE; i++) {             //Ciclo para:
            if (i < sb.retrieval_cache.idx) {                       //
                sb.retrieval_cache.ref[i] = BlockNullReference;     //      -> Define blocos da cache a null caso o idx da cache seja maior que 0, e preenche a null ate ao idx
            }                                                       //
            else {                                                  //    
                sb.retrieval_cache.ref[i] = ref++;                  //      -> se nao, começa definir o numero de identificaçao dos blocos na cache
            }
        }
        
        
        sb.insertion_cache.idx = 0;
        for(uint32_t i=0; i < REF_CACHE_SIZE; i++){                 // Ciclo para definir o blocos da insertion cache a Null
            sb.insertion_cache.ref[i] = BlockNullReference;         //
        }

        sb.ibitmap[0] = 1;                                          // Define a primeira posiçao do ibitmap a 1, porque o primeiro inode esta sempre alocado
        for(uint32_t i = 1;i < (MAX_INODES>>5); i++){               // Ciclo para definir o resto dos inodes a livres
            sb.ibitmap[i] = 0;
        }
        
        soWriteRawBlock(0, &sb);


        // /* replace the following line with your code */
        // binFillSuperblock(name, ntotal, itotal, dbtotal);
    };
};

