#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "devtools.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <string.h>

namespace sofs20
{
    void grpFillReferenceTable(uint32_t ntotal, uint32_t itotal, uint32_t dbtotal)
    {
        soProbe(605, "%s(%u, %u, %u)\n", __FUNCTION__, ntotal, itotal, dbtotal);
        // uint32_t nbRefTable = ntotal -1 - (itotal / IPB) - dbtotal;
        
		uint32_t nbRefTable = ntotal -1 - (itotal / IPB) - dbtotal;
		uint32_t fbrt = 1 + REF_CACHE_SIZE;
        uint32_t ref[RPB];
        if (dbtotal-1>REF_CACHE_SIZE) {
			for (uint32_t i = 1; i < nbRefTable +1 ; i++){
				for (uint32_t j = 0; j < RPB; j++){
					if (dbtotal>=fbrt+1) 
					{
						ref[j] = fbrt++;
					}else{
						ref[j] = BlockNullReference;
					}
				}
				soWriteRawBlock(ntotal-nbRefTable+i-1, &ref);
			}
		}

	}        
		/* replace the following line with your code */
		// binFillReferenceTable(ntotal, itotal, dbtotal);
};

