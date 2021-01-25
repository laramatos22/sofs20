#!/bin/bash

# Dário Notas:
# arg1 -> path até ao disco;
# usar -> source alloc_inode.sh; cd bin/; alloc_inode dsk;

function alloc_inode()
{
	for i in {1..62}
	do
		echo -e "ai\n1\n777\n" | ./testtool -g -q1 $1
		((i++))
	done

	echo 0
}