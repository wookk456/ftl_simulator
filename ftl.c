#include "ftl.h"
#include <stdio.h>

int count_free_blocks(DISK* disk){
    int not_free_blocks = 0;
    for (int i = 0; i< BLOCK_NUM; i++){
        for (int j = 0; j < PAGE_NUM; j++){
            if (disk->blocks[i].pages[j] != 0){
                not_free_blocks++;
                break;
            } 
        }
    }
    return BLOCK_NUM - not_free_blocks;
}

int gc_totally(DRAM* dram, DISK* disk){
    for (int i = 0; i< BLOCK_NUM; i++){
        for (int j = 0; j < PAGE_NUM; j++){
            if (disk->blocks[i].pages[j] == -1){
                p_erase(i, dram, disk);
                break;
            }
        }
    }
}

int gc_greedy_1(DRAM* dram, DISK* disk){
    dram->gc_count++;
    int max_invalid = -1;
    int max_block = -1;
    for (int i = 0; i < BLOCK_NUM; i++){
        if (disk->blocks[i].num_of_invalid > max_invalid){
            max_invalid = disk->blocks[i].num_of_invalid;
            max_block = i;
        }
    }
    return max_block;
}

int gc_FIFO(DRAM* dram, DISK* disk){
    dram->gc_count++;
    int fifo = dram->fifo_block;
    dram->fifo_block++;
    if (dram->fifo_block >= BLOCK_NUM){
        dram->fifo_block = 0;
    }
    return fifo;
}

int p_read(){
    return 0;
}

int p_write(int LBA, int io_size, DRAM* dram, DISK* disk){
    
    int block_num;
    int page_num;
    int temp;

    if (dram->state == 1){
        block_num = dram->recent_log / BLOCK_SIZE;
        page_num = dram->recent_log - block_num * BLOCK_SIZE;
        temp = dram->recent_log;
        dram->recent_log++;
        if (page_num == 1023){
            dram->state = 0;
        }
    } else if (dram->state == 0){
        block_num = dequeue(dram->free_block_q);
        page_num = 0;
        dram->state = 1;
        temp = block_num*BLOCK_SIZE;
        dram->recent_log = block_num*BLOCK_SIZE+1;
    }

    disk->blocks[block_num].pages[page_num] = VALID;
    disk->blocks[block_num].ptable[page_num] = LBA;
    dram->num_of_write++;
    dram->tmp_counts.tmp_lst[dram->tmp_counts.size].write_cnt++;
    if (dram->mtable[LBA] != -1){
        block_num = dram->mtable[LBA] / BLOCK_SIZE;
        page_num = dram->mtable[LBA] - block_num * BLOCK_SIZE;
        disk->blocks[block_num].pages[page_num] = INVALID;
        disk->blocks[block_num].num_of_invalid++;
    }
    
    dram->mtable[LBA] = temp;

    
    if (getQueueSize(dram->free_block_q) < 2) { 
        p_erase(gc_FIFO(dram,disk), dram, disk);
        dram->tmp_counts.tmp_lst[dram->tmp_counts.size].gc_cnt++;
        /*p_erase(gc_greedy_1(dram, disk), dram, disk);
        dram->tmp_counts.tmp_lst[dram->tmp_counts.size].gc_cnt++;*/
        
    }

    return 0;
}

int p_write_no_gc(int LBA, int io_size, DRAM* dram, DISK* disk){
    int block_num;
    int page_num;
    int temp;

    if (dram->state == 1){
        block_num = dram->recent_log / BLOCK_SIZE;
        page_num = dram->recent_log - block_num * BLOCK_SIZE;
        temp = dram->recent_log;
        dram->recent_log++;
        if (page_num == 1023){
            dram->state = 0;
        }
    } else if (dram->state == 0){
        block_num = dequeue(dram->free_block_q);
        page_num = 0;
        dram->state = 1;
        temp = block_num*BLOCK_SIZE;
        dram->recent_log = block_num*BLOCK_SIZE+1;
    }

    disk->blocks[block_num].pages[page_num] = VALID;
    disk->blocks[block_num].ptable[page_num] = LBA;
    dram->num_of_write++;
    dram->tmp_counts.tmp_lst[dram->tmp_counts.size].write_cnt++;
    if (dram->mtable[LBA] != -1){
        block_num = dram->mtable[LBA] / BLOCK_SIZE;
        page_num = dram->mtable[LBA] - block_num * BLOCK_SIZE;
        disk->blocks[block_num].pages[page_num] = INVALID;
        disk->blocks[block_num].num_of_invalid++;
    }
    
    dram->mtable[LBA] = temp;
    return 0;

}



int p_erase(int b_num, DRAM* dram, DISK* disk){
    for (int i = 0; i<BLOCK_SIZE; i++){
        if (disk->blocks[b_num].pages[i] == VALID){
            dram->tmp_counts.tmp_lst[dram->tmp_counts.size].valid_cnt++;
            int LBA = disk->blocks[b_num].ptable[i];
            p_write_no_gc(LBA, 4096, dram, disk);
        }
        if (disk->blocks[b_num].pages[i] == INVALID){
            disk->blocks[b_num].num_of_invalid--;
        }
        disk->blocks[b_num].pages[i] = ERASED;
    }
    dram->erase_count++;
    dram->tmp_counts.tmp_lst[dram->tmp_counts.size].erase_cnt++;
    enqueue(dram->free_block_q, b_num);
    disk->blocks[b_num].num_of_invalid = 0;
    return 0;
}

double check_utilization(DISK* disk){
    double num_of_used_page = 0;

    for (int i = 0; i< BLOCK_NUM; i++){
        for (int j = 0; j < PAGE_NUM; j++){
            if (disk->blocks[i].pages[j] != ERASED){
                num_of_used_page += 1;
            }
        }
    }

    return num_of_used_page/DEVICE_SIZE;

}

/*int p_trim(int LBA, DRAM* dram, DISK* disk){
    int block_num = LBA / BLOCK_SIZE;
    int page_num = LBA - block_num * BLOCK_SIZE;

    if (disk->blocks[block_num].pages[page_num] != INVALID){
        disk->blocks[block_num].pages[page_num] == INVALID;
        disk->blocks[block_num].num_of_invalid++;
    }
    return 0;
}*/