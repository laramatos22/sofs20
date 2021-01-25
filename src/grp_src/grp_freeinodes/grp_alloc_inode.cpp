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
#include <sys/stat.h>
#include <string.h>

#include <iostream>

#include "core.h"
#include "devtools.h"
#include "daal.h"

namespace sofs20
{
	uint16_t grpAllocInode(uint32_t mode)
	{
		soProbe(401, "%s(0x%x)\n", __FUNCTION__, mode);

		// Nota: Argumento `mode` é bitwise-OR entre `file type` e `permissões`

		/* Tipo de Ficheiros no GNU Library: 
			Octal -> 
				S_IFREG -> File
				S_IFDIR -> Diretório
				S_IFLNK -> Symbolic Link
		*/
		// Descompacta as Permissões
		uint32_t perm  = mode & ( ~(S_IFREG | S_IFDIR | S_IFLNK));

		/* Permission-Bits no GNU Library: 
			Octal ->
				S_IRWXU -> owner  rwx : 00700
				S_IRWXG -> group  rwx : 00070
				S_IRWXO  -> others r_x : 00007
		*/
		// Descompacta o Tipo de ficheiros
		uint32_t ftype = mode & ( ~(S_IRWXU | S_IRWXG | S_IRWXO));

		// Nota: `type` tem que representar um ficheiro, directório ou symlink
		// 		 caso contrário usar a excepção EINVAL
		if(ftype!=S_IFREG && ftype!=S_IFDIR && ftype!=S_IFLNK)
			throw SOException(EINVAL, __FUNCTION__);

		// Nota: permissões têm que existir no range de 0000 a 0777
		// 		 caso contrário usar a excepção EINVAL
		if(perm<0 || perm > (S_IRWXU | S_IRWXG | S_IRWXO) )
			throw SOException(EINVAL, __FUNCTION__);

		// Aceder ao Superblock
		SOSuperblock *superblock = soGetSuperblockPointer();

		// Nota: verificar se existem inodes livre; else usar ENOSPC
		if(superblock->ifree == 0)
			throw SOException(ENOSPC, __FUNCTION__);
	
		// Obter o indice da ultima posição ocupada no ibitmap
		uint32_t iidx = superblock->iidx;

		// Inicializar a referência ao inode;
		uint16_t inumber = iidx;

		uint16_t inode_max = (MAX_INODES/32 * 32);

		// Pesquisa circular no bitmap pelo primeiro inode livre
		do {
			// Muda ponteiro para o inicio caso tenha chegado ao final do bitmap ...
			// ... e não tenha achado inode livre
			if(inumber == inode_max-1 || inumber >= superblock->itotal-1)
				inumber = 0;
			else
				inumber = inumber+1;

			uint32_t bloco = inumber / 32;
			uint32_t pos = inumber % 32;
			uint32_t bit = 0x01 << pos;

			// verifica se o bit no bitmap está disponível
			if( ((superblock->ibitmap[bloco] & bit) ^ bit) != 0) {
				superblock->ibitmap[bloco] = superblock->ibitmap[bloco] | bit;
				break;
			}
		} while(inumber!=iidx);

		// atualiza nº de inodes livres
		superblock->ifree--;
		// atualiza ponteiro para o último inode allocado
		superblock->iidx = inumber;

		// guardar alteraçãos ao superblock
		soSaveSuperblock();		

		// obter Inode handler
		int ihandler = soOpenInode(inumber);
		// obter Inode Pointer
		SOInode *inode = soGetInodePointer(ihandler);

		/*
            // Percorrer todas as referências a data blocks
            // Nota: Em "clean state", inicializar todas as referências com BlockNullReference; 
            uint32_t i;
            for(i=0; i<N_DIRECT; i++)
                inodeBlock[inode].d[i] = BlockNullReference;
            for(i=0; i<N_INDIRECT; i++)
                inodeBlock[inode].i1[i] = BlockNullReference;
            for(i=0; i<N_DOUBLE_INDIRECT; i++)
                inodeBlock[inode].i2[i] = BlockNullReference;
        */     
		
		// Mudar info do inode (voltei a colocar alguns destes a 0 por redundância, mas possivelmente é desnecessário porque o inode já era livre)
		inode->mode = ftype | perm;
		inode->lnkcnt = 0;
		inode->owner = getuid();
		inode->group = getgid();
		inode->size = 0;
		inode->blkcnt = 0;
		
		time_t system_time;
		time(&system_time);
		uint32_t time = system_time;
		inode->atime=time;
		inode->mtime=time;
		inode->ctime=time;

		// guardar alterações ao inode
		soSaveInode(ihandler);
		// fechar inode
		soCloseInode(ihandler);
		
		return inumber;

		//return binAllocInode(mode);
	}
};

