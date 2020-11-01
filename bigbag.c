//
// Created by PakShing Kan on 10/28/20.
//

// order: l list => c check => a add => d delete
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include "bigbag.h"

struct bigbag_entry_s *entry_addr(void *hdr, uint32_t offset) {
    if (offset == 0) return NULL;
    return (struct bigbag_entry_s *)((char*)hdr + offset);
}

int main(int argc, char *argv[]){
    int tFlag = 0;
    //char* command = NULL;
    char *line = NULL;
    size_t size = 0;
    size_t line_size;

    if (argc < 2 || argc >3){
        printf("%s", "USAGE: ./bigbag [-t] filename\n");
        exit(1);
    }
    if(argc == 3){
        tFlag =1;
    }

    int fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror(argv[1]);
        return 2;
    }

    void *file_base = mmap(0, BIGBAG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (file_base == MAP_FAILED) {
        perror("mmap");
        return 3;
    }

    struct bigbag_hdr_s *hdr = file_base;

    struct stat stat;
    if (fstat(fd, &stat) == -1){
        perror("couldn't get file size\n");
    }

    if(stat.st_size == 0){
        printf("ftruncate to 64 K now");
        if( ftruncate(fd,BIGBAG_SIZE) ==-1 ){
            perror("ftruncate failed");
        }
    }else{
       // printf("stat.st_size is currently %lld\n ", stat.st_size);
    }



    //printf("size = %lld\n", stat.st_size);
    //printf("magic = %08x\n", htonl(hdr->magic));
    //printf("first_free = %d\n", hdr->first_free);


    line_size = getline(&line,&size,stdin);
    //printf("%zu characters were read.\n",line_size);
    //printf("You typed: %s\n",line);

    int input_size = ((int)line_size)-3;
    char input_string[input_size] ;
    if(line[0] == 'c'){
       strncpy(input_string,line+2,input_size);
        //printf("command and size : %s %lu\n", input_string, sizeof(input_string));
    }

    struct bigbag_entry_s *entry;
    int offset = sizeof(*hdr);
    offset = hdr->first_element;
    int found =0;

    while (offset + sizeof(*entry) < stat.st_size) {
        entry = entry_addr(hdr, offset);
        if (entry == NULL) {
            if(line[0]=='c' && found ==0){
                printf("not found\n");
            }
            //printf("bad entry at offset %d\n", offset);
            break;
        }
        if (entry->entry_magic == BIGBAG_USED_ENTRY_MAGIC) {
            if(line[0] == 'c'){
                if(strcmp(entry->str,input_string) ==0){
                    found =1;
                    printf("found\n");
                }
            }else{
                printf("%s\n", entry->str);
            }

        }
        offset = entry->next;
    }

    free(line);
}
