#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ftl.h"

int input_control(int type, int LBA, int byte, DISK* disk, DRAM* dram);

int main(){
    DISK* disk = (DISK*)calloc(1, sizeof(DISK));
    DRAM* dram = (DRAM*)malloc(sizeof(DRAM));
    memset(dram, 0, sizeof(DRAM));
    memset(dram, -1, sizeof(int)*DEVICE_SIZE);
    dram->free_block_q = (Queue*)malloc(sizeof(Queue));
    dram->recent_log = 0;
    dram->fifo_block = 0;
    dram->state = 0;
    dram->num_of_free_block = BLOCK_NUM;
    dram->num_of_write = 0;
    dram->num_of_write_request = 0;
    dram->gc_count = 0;
    dram->erase_count = 0;
    dram->tmp_counts.size = 0;
    
    initializeQueue(dram->free_block_q);

    for (int i = 0; i < MAX_QUEUE_SIZE; i++){
        enqueue(dram->free_block_q, i);
    }
    

    double time;
    int type;
    int LBA;
    int byte;
    int stream;
    int counter = -1;
    int loop_counter = 1;
    double WAF = 0;
    double TMP_WAF = 0;
    double Utilization = 0;
    int used_block = 0;

    int tmp_write_cnt = 0;
    int tmp_write_req = MAX50GB;
    int tmp_erase_cnt = 0;
    int tmp_gc_cnt = 0;
    int tmp_valid_cnt = 0;

    double valid_data_ratio_50gb;

    FILE* fp = fopen("test-fio-small", "r");
    if (fp == NULL){printf("file does not opened\n");}
    while (fscanf(fp, "%lf %d %d %d %d", &time, &type, &LBA, &byte, &stream) != EOF){
        dram->tmp_counts.tmp_lst[dram->tmp_counts.size].erase_cnt = 0;
        dram->tmp_counts.tmp_lst[dram->tmp_counts.size].write_cnt = 0;
        dram->tmp_counts.tmp_lst[dram->tmp_counts.size].gc_cnt = 0;
        dram->tmp_counts.tmp_lst[dram->tmp_counts.size].valid_cnt = 0;
        input_control(type, LBA, byte, disk, dram);
        counter++;
        dram->tmp_counts.size++;
        if (dram->tmp_counts.size == MAX50GB){ dram->tmp_counts.size = 0;}
        if (counter == 2097152){
            WAF = (double)dram->num_of_write/(double)dram->num_of_write_request;
            for (int i = 0; i < MAX50GB; i++){
                if(dram->tmp_counts.tmp_lst[i].write_cnt == 0){
                    tmp_write_req = i;
                    break; 
                }
                tmp_write_cnt += dram->tmp_counts.tmp_lst[i].write_cnt;
                tmp_erase_cnt += dram->tmp_counts.tmp_lst[i].erase_cnt;
                tmp_gc_cnt += dram->tmp_counts.tmp_lst[i].gc_cnt;
                tmp_valid_cnt += dram->tmp_counts.tmp_lst[i].valid_cnt;
            }
            TMP_WAF = (double)tmp_write_cnt / (double)tmp_write_req;
            Utilization = check_utilization(disk);
            used_block = MAX_QUEUE_SIZE-getQueueSize(dram->free_block_q);
            valid_data_ratio_50gb = (double)tmp_valid_cnt / (double)(1024 * tmp_gc_cnt);
            printf("[Progress: %dGiB] WAF: %.3lf, TMP_WAF: %.3lf, Utilization: %.3lf\n", loop_counter*8, WAF, TMP_WAF, Utilization);
            printf("GROUP 0[%d]: %.2lf (ERASE: %d)\n", used_block, valid_data_ratio_50gb, tmp_erase_cnt);

            tmp_write_cnt = 0;
            tmp_write_req = MAX50GB;
            tmp_erase_cnt = 0;
            tmp_gc_cnt = 0;
            tmp_valid_cnt = 0;

            counter = -1;
            loop_counter++;
        }
    }

    fclose(fp);

    free(disk);
    free(dram);

    printf("Simulation is successfully finished!\n");

    return 0;
}

int input_control(int type, int LBA, int byte, DISK* disk, DRAM* dram){
    switch (type)
    {
    case 0:
        p_read();
        break;
    case 1:
        p_write(LBA, byte, dram, disk);
        dram->num_of_write_request++;
        break;
    case 2:
        break;
    case 3:
        //p_trim(LBA, dram, disk);
        break;
    default:
        break;
    }
    return 0;
}