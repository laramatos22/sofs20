#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "devtools.h"
#include "bin_mksofs.h"

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <inttypes.h>

namespace sofs20
{
    void grpFillInodeTable(uint32_t itotal, bool date)
    {
        soProbe(604, "%s(%u)\n", __FUNCTION__, itotal);

        // Calcular número de Blocks no Inode Table
        uint32_t nBLocks = itotal/IPB;

        // Iterar pela Inode Table 
        // Nota: Inode Table começa no block number 1
        for(uint32_t block=1; block<=nBLocks; block++) {
            
            // Inicializar 1 Block Array de Inodes
            SOInode inodeBlock[IPB];

            // Percorrer todos os Inodes do Array 
            // Nota: inicializar Inodes em "clean state"
            for(uint32_t inode=0; inode<IPB; inode++) {
                
                /*	"Clean State": inodes com valores default
                		-> Referências são BlockNullReference
                		-> Outros fields são 0
					Mais informações sobre os campos em inode.h
				*/
                inodeBlock[inode].mode = 0;
                inodeBlock[inode].lnkcnt = 0;
                inodeBlock[inode].owner = 0;
                inodeBlock[inode].group = 0;
                inodeBlock[inode].size = 0;
                inodeBlock[inode].blkcnt = 0;
                inodeBlock[inode].atime = 0;
                inodeBlock[inode].mtime = 0;                
                inodeBlock[inode].ctime = 0;

                // Percorrer todas as referências a data blocks
                // Nota: Em "clean state", inicializar todas as referências com BlockNullReference; 
                uint32_t i;
                for(i=0; i<N_DIRECT; i++)
                    inodeBlock[inode].d[i] = BlockNullReference;
                for(i=0; i<N_INDIRECT; i++)
                    inodeBlock[inode].i1[i] = BlockNullReference;
                for(i=0; i<N_DOUBLE_INDIRECT; i++)
                    inodeBlock[inode].i2[i] = BlockNullReference;
            }

            /*	Nota: Todos os Inodes são livres excepto Inode nº 0 
            		-> Nota: Esse corresponde ao "Root Dir" do filesystem 
            */
            if(block==1) {
                // Nota: Permissões devem ser 0755
                
                /* Permission-Bits no GNU Library: 
                	Octal -> 
                    	S_IFDIR -> directory  : 0040000
                    	S_IRWXU -> owner  rwx : 00700
                    	S_IRGRP | S_IXGRP -> group  r_x : 00050
          				S_IROTH | S_IXOTH -> others r_x : 00005
                */
               	// Nota: Permissões para 775, como no binário do prof -_- (Nota: Deixar enquanto não esclareço com ele)
               	inodeBlock[0].mode = S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP  | S_IROTH | S_IXOTH;
                
                /* Permission-Bits no GNU Library: 
                	Octal -> 
                    	S_IFDIR -> directory  : 0040000
                    	S_IRWXU -> owner  rwx : 00700
                    	S_IRGRP | S_IXGRP -> group  rwx : 00050
          				S_IROTH | S_IXOTH -> others r_x : 00005
                */
            	// Nota: Permissões para 755, como PEDIDO na Documentação!! (Nota: Falar com o Artur)
                //inodeBlock[0].mode = S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP  | S_IROTH | S_IXOTH;

                // Nota: 2 dir entries para o "Root Dir", seja: ./ ../ 
                inodeBlock[0].lnkcnt = 2;
                // Nota: Root user owns "Root Dir"
                inodeBlock[0].owner = getuid();
                // Nota: "Root Dir" pertence ao grupo Root 
                inodeBlock[0].group = getuid();
                // Nota: Root dir ocupa só 1 **Data** Block no disco recentement formatad; 
                // -> E possui 2 entradas ./ e ../, ambas apontando para ele próprio
                inodeBlock[0].size = BlockSize/DPB*2;
                inodeBlock[0].blkcnt = 1;
                
                // Nota: Se data true, date é set para a data corrente
                if(date==true) {
                    time_t system_time;
                    time(&system_time);
                    uint32_t time = system_time;
                    
                    inodeBlock[0].atime=time;
					inodeBlock[0].mtime=time;
                    inodeBlock[0].ctime=time;
                }
                // reference to root dir
                inodeBlock[0].d[0] = 0;
            }

            // Escreve bloco para disco, formatando-o
            soWriteRawBlock(block, inodeBlock);
        }

        //binFillInodeTable(itotal, date);
    }
};

