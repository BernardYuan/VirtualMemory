include <stdio.h>
typedef unsigned long word;
typedef unsigned short half_word;
typedef unsigned char byte;
#define MAXPAGE 16
#define PAGE_SIZE 4096

typedef struct {
    word hdIdx;
    int used;
} pageInfo;

byte page[4096];
byte mem[65536];    // 512 bytes/block, 8 blocks/cluster, 16 clusters, 16 pages
byte tmpVM[262144]; // used to store virtual memory when modifying
pageInfo pages[MAXPAGE];

void init() {
    int i;
    FILE* fp=fopen("vm256","rb+");
    for (i = 0; i < 4; i++)
        fwrite(mem, 1, MAXPAGE * PAGE_SIZE, fp);
    fclose(fp);

    for (i = 0; i < 16; i++) {
        pages[i].hdIdx = i;
        pages[i].used = 0;
    }
}

byte lb(word addr) {
    int i;
    int minUse = 65535, minPage = 17;
    word pageID = addr / PAGE_SIZE;
    word offSet = addr % PAGE_SIZE;

    for (i = 0; i < MAXPAGE; i++) {
        if (pages[i].used < minUse) {
            minUse = pages[i].used;
            minPage = i;
        } 
        if (pages[i].hdIdx == pageID) {
            return mem[i * PAGE_SIZE + offSet];
        }
    }
    FILE* fp=fopen("vm256","rb+");
    if (pages[minPage].used > 0) {
        fseek(fp, PAGE_SIZE * (pages[minPage].hdIdx + 1), SEEK_SET);
        fread(tmpVM, 1, PAGE_SIZE * (MAXPAGE - pages[minPage].hdIdx), fp);
        fseek(fp, PAGE_SIZE * pages[minPage].hdIdx, SEEK_SET);
        fwrite(mem + PAGE_SIZE * minPage, 1, PAGE_SIZE, fp);
        fseek(fp, PAGE_SIZE * (pageID + 1), SEEK_SET);
        fwrite(tmpVM, 1, PAGE_SIZE * (MAXPAGE - pages[minPage].hdIdx), fp);
    }
    fseek(fp, PAGE_SIZE * pageID, SEEK_SET);
    fread(mem + PAGE_SIZE * minPage, 1, PAGE_SIZE, fp);
    pages[minPage].hdIdx = pageID;
    pages[minPage].used = 0;
    fclose(fp);
    return mem[minPage * PAGE_SIZE + offSet];
}

half_word lh(word addr) {
    return (lb(addr + 1) << 8) + lb(addr);
}

word lw(word addr) {
    return (lh(addr + 2) << 16) + lh(addr);
}

void sb(word addr, byte val) {
    int i;
    int minUse = 65535, minPage = 17;
    word pageID = addr / PAGE_SIZE;
    word offSet = addr % PAGE_SIZE;
    for (i = 0; i < MAXPAGE; i++) {
        if (pages[i].used < minUse) {
            minUse = pages[i].used;
            minPage = i;
        } 
        if (pages[i].hdIdx == pageID) {
            mem[i * PAGE_SIZE + offSet] = val;
            return;
        }
    }
    FILE* fp=fopen("vm256","rb+");
    if (pages[minPage].used > 0) {
        fseek(fp, PAGE_SIZE * (pages[minPage].hdIdx + 1), SEEK_SET);
        fread(tmpVM, 1, PAGE_SIZE * (MAXPAGE - pages[minPage].hdIdx), fp);
        fseek(fp, PAGE_SIZE * pages[minPage].hdIdx, SEEK_SET);
        fwrite(mem + PAGE_SIZE * minPage, 1, PAGE_SIZE, fp);
        fseek(fp, PAGE_SIZE * (pageID + 1), SEEK_SET);
        fwrite(tmpVM, 1, PAGE_SIZE * (MAXPAGE - pages[minPage].hdIdx), fp);
    }
    fseek(fp, PAGE_SIZE * pageID, SEEK_SET);
    fread(mem + PAGE_SIZE * minPage, 1, PAGE_SIZE, fp);
    pages[minPage].hdIdx = pageID;
    pages[minPage].used = 0;
    fclose(fp);
    mem[minPage * PAGE_SIZE + offSet] = val;
}

void sh(word addr, half_word val) {
    sb(addr, (byte)val);
    sb(addr + 1, (byte)(val >> 8));
}

void sw(word addr, word val) {
    sh(addr, (half_word)val);
    sh(addr + 2, (half_word)(val >> 16));
}


int main() {
    init();
    sw(16000,1);
    sb(16004,2);
    sh(16005,3);
    printf("%X\n",lw(16000));
    printf("%X\n",lb(16004));
    printf("%X\n",lh(16005));
}
