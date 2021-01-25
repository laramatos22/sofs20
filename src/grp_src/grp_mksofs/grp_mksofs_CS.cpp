#include "grp_mksofs.h"

#include "core.h"
#include "devtools.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <iostream>

namespace sofs20
{
    void grpComputeStructure(uint32_t ntotal, uint32_t & itotal, uint32_t & dbtotal)
    {
        soProbe(601, "%s(%u, %u, ...)\n", __FUNCTION__, ntotal, itotal);

        // replace the following line with your code 
        //binComputeStructure(ntotal, itotal, dbtotal);

		//if, at entry, the proposed value for itotal is 0, 
		//value ntotal/16 should be used
		if (itotal == 0)
		{
			itotal = (ntotal/16);
		}
		
		//itotal must be lower than or equal to the rounded up value of ntotal/8
		if (itotal > ((ntotal/8) + 1))
		{
			itotal = (ntotal/8) + 1;
		}

		//itotal must be rounded up to a multiple of 32;
		if(itotal % 32 != 0){
			itotal = (itotal/32 + 1 )* 32;
		}

		
		//itotal must be greater than or equal to IPB 
		if(itotal < IPB){
			itotal = IPB;
		}

		//IPB=number of inodes per block
        //itotal must be rounded up to a multiple of IPB;
		if(itotal % IPB != 0)
		{
			itotal = (1 + ((itotal)/ IPB)) * IPB;
		}
		//for every free data block (all but one), 
		//there must exist a reference in the retrieval cache or in the reference table
		uint32_t nref = (ntotal - 1 - (1+itotal/((double)RPB) - itotal/((double)IPB)));
		uint32_t rem = nref % (RPB);
		uint32_t nbref = nref/((RPB + 1 )* 1.0);

		if(rem != 0){
			nbref += 2;
		}		
		dbtotal = nref - (nbref + 1);

		//If, after making the partition, a spare block remains,
		// it should be assigned to the inode table
		if (rem == 1)
		{
			dbtotal = nref - (nbref + 2);
		}
    }
};

