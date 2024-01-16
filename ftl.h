#include "queue.h"
#define REAL_PAGE_SIZE 4096 // 4kb
#define REAL_BLOCK_SIZE 4194304 // 4mb
#define REAL_DEVICE_SIZE 8589934592 // 8gib

#define PAGE_SIZE 1
#define BLOCK_SIZE 1024
#define DEVICE_SIZE 2097152

#define PAGE_NUM 1024
#define BLOCK_NUM 2048

#define ERASED 0
#define VALID 1
#define INVALID -1

#define MAX50GB 13107200

typedef struct tmp{
    int erase_cnt;
    int write_cnt;
    int gc_cnt;
    int valid_cnt; 
}tmp;

typedef struct last50gib{
    tmp tmp_lst[13107200];
    int size;
}last50gib;

typedef struct dram{
    int mtable[DEVICE_SIZE];
    int recent_log;
    int fifo_block;
    int state;
    Queue *free_block_q;
    int num_of_write;
    int num_of_write_request;
    int gc_count;
    int erase_count;
    last50gib tmp_counts;
} DRAM;

typedef struct block{
    int pages[PAGE_NUM];
    int ptable[PAGE_NUM];
    int num_of_invalid;
}BLOCK;

typedef struct disk{
    BLOCK blocks[BLOCK_NUM];
}DISK;


int gc_greedy_1(DRAM* dram, DISK* disk);
int gc_FIFO(DRAM* dram, DISK* disk);
int p_read();
int p_write(int LBA, int io_size, DRAM* dram, DISK* disk);
int p_write_no_gc(int LBA, int io_size, DRAM* dram, DISK* disk);
int p_erase(int b_num, DRAM* dram, DISK* disk);
double check_utilization(DISK* disk);
