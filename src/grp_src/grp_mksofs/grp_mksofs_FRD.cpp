#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "devtools.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>

//Daniel Fernandes Capitão nºmec 75943

namespace sofs20
{
        /*
       The following nust be considered:

			* The root directory occupies a single data block, the first one after the inode table
			* It contains two entries, filled in with "." and "..", both pointing to inode 0
			* The remaining of the data block must be cleaned, filled with pattern 0x0

        */
       
    void grpFillRootDir(uint32_t itotal)
    {
        soProbe(606, "%s(%u)\n", __FUNCTION__, itotal);
        
        SODirentry directory[DPB]; //DPB (constante) nº de direntries por bloco
        

        strncpy(directory[0].name, ".",SOFS20_FILENAME_LEN +1);
        strncpy(directory[1].name, "..",SOFS20_FILENAME_LEN +1);
        
        directory[0].in=0;
        directory[1].in=0;
        
        uint32_t i;

            for(i = 2; i < DPB; i++){
                directory[i].in = 0x0;
                strncpy(directory[i].name, "\0",SOFS20_FILENAME_LEN +1);
            }
            
            soWriteRawBlock(itotal/IPB+1, directory); //Guardado no 1º bloco livre a seguir á inodetable

        // binFillRootDir(itotal);
    }
};










