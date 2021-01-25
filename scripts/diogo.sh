#2 layer
    # first argument: first inode to be freed
    # second argument: last indoe to be freed
    # third argument: path to the disk
    function free_inode() {
        i=$1
        while [ $i -le $2 ]
        do
            echo -e "fi\n$i\n"  | ./testtool -g -q1 $3
            ((i++))
        done 
        echo "done"
        }
    # first argument: number of blocks to be alocated
    # second argument: path to the disk
    function alloc_data_blocks(){
        i=0
        while [ $i -lt $1 ]
        do
            echo -e "adb"  | ./testtool -g -q1 $2
            ((i++))
        done 
        echo "done"
        }
    # first argument: first block to be freed
    # second argument: last block to be freed
    # third argument: path to the disk
    function free_data_blocks(){
        i=$1
        while [ $i -le $2 ]
        do
            echo -e "fdb\n$i\n"  | ./testtool -g -q1 $3
            ((i++))
        done 
        echo "done"
        }
    # before using deplete and replenish function, format the disk 
    #first argument: path to the disk
    function deplete(){
        free_data_blocks 1 68 $1
        echo -e "dic\n" | ./testtool -g -q1 $1 
        }
    #first argument: path to the disk
    function replenish(){
        alloc_data_blocks 68 $1
        echo -e "rrc\n" | ./testtool -g -q1 $1 
        }
#3layer
    # first argument: first inode to be writen
    # second argument: last indoe to be writen
    # third argument: first fileblock to be writen in each inode
    # fourth argument: last fileblock to be writen in each inode
    # fifth argument: path to the disk
    function write_file_block(){
        i=$1
        while [ $i -le $2 ]
        do
            j=$3
            while [ $j -le $4 ]
            do
                echo -e "wfb\n$mini\n$minj\n69\n"  | ./testtool -g -q1 $5
                ((j++))
            done
            ((i++))
        done 
        echo "done"
        }
    # first argument: first inode to be freed
    # second argument: last indoe to be freed
    # third argument: first fileblock to be freed in each inode
    # fourth argument: last fileblock to be freed in each inode
    # fifth argument: path to the disk
    function free_file_block(){
        mini=$1
        while [ $mini -le $2 ]
        do
            minj=$3
            while [ $minj -le $4 ]
            do
                echo -e "ffb\n$mini\n$minj\n"  | ./testtool -g -q1 $5
                ((minj++))
            done
            ((mini++))
        done 
        echo "done"
        }
    # first argument: inode to be read
    # second argument: fbn to be read
    # third argument: path to the disk
    function read_file_block(){
        echo -e "rfb\n$1\n$2\n" | ./testtool -g -q1 $3
        }
    # first argument: inode to get
    # second argument: fbn to get
    # third argument: path to the disk
    function get_file_block(){
        echo -e "gfb\n$1\n$2\n" | ./testtool -g -q1 $3
        }