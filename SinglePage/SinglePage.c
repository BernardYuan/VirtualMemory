#include <stdio.h>
typedef unsigned int word;
typedef unsigned short halfword;
typedef unsigned char byte;
byte Mem[4096];
word page=0;
byte coMem[16384];
int used=0;
word lw(word addr) {
    word data0,data1,data2,data3;
    word pag=addr/4096;
    word ofs=addr%4096;
    
    if (pag==page) {
        data0=Mem[ofs];
        data1=Mem[ofs+1];
        data2=Mem[ofs+2];
        data3=Mem[ofs+3];
    }else {
        FILE* fp=fopen("virtual4K","rb+");
        if (used) {
            fseek(fp,4096*(page+1),SEEK_SET);
            fread(coMem,1,4096*(3-page),fp);
            fseek(fp,4096*page,SEEK_SET);
            fwrite(Mem,1,4096,fp);
            fseek(fp,4096*(page+1),SEEK_SET);
            fwrite(coMem,1,4096*(3-page),fp);
        }
        fseek(fp,4096*pag,SEEK_SET);
        fread(Mem,1,4096,fp);
        used=0;
        page=pag;
        fclose(fp);
        data0=Mem[ofs];
        data1=Mem[ofs+1];
        data2=Mem[ofs+2];
        data3=Mem[ofs+3];
    }
    return (data0<<24)|(data1<<16)|(data2<<8)|(data3);
}

halfword lh(word addr) {
    halfword data0, data1;
    word pag=addr/4096;
    word ofs=addr%4096;
    if(pag==page) {
        data0=Mem[ofs];
        data1=Mem[ofs+1];
    }else {
        FILE* fp=fopen("virtual4K","rb+");
        if (used) {
            fseek(fp,4096*(page+1),SEEK_SET);
            fread(coMem,1,4096*(3-page),fp);
            fseek(fp,4096*page,SEEK_SET);
            fwrite(Mem,1,4096,fp);
            fseek(fp,4096*(page+1),SEEK_SET);
            fwrite(coMem,1,4096*(3-page),fp);
        }
        fseek(fp,4096*pag,SEEK_SET);
        fread(Mem,1,4096,fp);
        used=0;
        page=pag;
        fclose(fp);
        data0=Mem[ofs];
        data1=Mem[ofs+1];
    }
    return (data0<<8)|(data1);
}

byte lb(word addr) {
    byte data0;
    
    word pag=addr/4096;
    word ofs=addr%4096;
    if(pag==page) {
        data0=Mem[ofs];
    }else {
        FILE* fp=fopen("virtual4K","rb+");
        if (used) {
            fseek(fp,4096*(page+1),SEEK_SET);
            fread(coMem,1,4096*(3-page),fp);
            fseek(fp,4096*page,SEEK_SET);
            fwrite(Mem,1,4096,fp);
            fseek(fp,4096*(page+1),SEEK_SET);
            fwrite(coMem,1,4096*(3-page),fp);
        }
        fseek(fp,4096*pag,SEEK_SET);
        fread(Mem,1,4096,fp);
        used=0;
        page=pag;
        fclose(fp);
        data0=Mem[ofs];
    }
    return data0;
}

void sw(word addr,word data) {
    
    word pag=addr/4096;
    word ofs=addr%4096;
    
    if (pag==page) {
        Mem[ofs]=data>>24;
        Mem[ofs+1]=data>>16&0x000000ff;
        Mem[ofs+2]=data>>8&0x000000ff;
        Mem[ofs+3]=data&0x000000ff;
    }else {
        FILE* fp=fopen("virtual4K","rb+");
        if (used) {
            fseek(fp,4096*(page+1),SEEK_SET);
            fread(coMem,1,4096*(3-page),fp);
            fseek(fp,4096*page,SEEK_SET);
            fwrite(Mem,1,4096,fp);
            fseek(fp,4096*(page+1),SEEK_SET);
            fwrite(coMem,1,4096*(3-page),fp);
        }
        fseek(fp,4096*pag,SEEK_SET);
        fread(Mem,1,4096,fp);
        page=pag;
        fclose(fp);
        Mem[ofs]=data>>24;
        Mem[ofs+1]=data>>16&0x000000ff;
        Mem[ofs+2]=data>>8&0x000000ff;
        Mem[ofs+3]=data&0x000000ff;
        used=1;
    }
}

void sh(word addr,halfword data) {
    word pag=addr/4096;
    word ofs=addr%4096;
    
    if (pag==page) {
        Mem[ofs]=data>>8;
        Mem[ofs+1]=data&0x00ff;
    }else {
        FILE* fp=fopen("virtual4K","rb+");
        if (used) {
            fseek(fp,4096*(page+1),SEEK_SET);
            fread(coMem,1,4096*(3-page),fp);
            fseek(fp,4096*page,SEEK_SET);
            fwrite(Mem,1,4096,fp);
            fseek(fp,4096*(page+1),SEEK_SET);
            fwrite(coMem,1,4096*(3-page),fp);
        }
        fseek(fp,4096*pag,SEEK_SET);
        fread(Mem,1,4096,fp);
        page=pag;
        fclose(fp);
        Mem[ofs]=data>>8;
        Mem[ofs+1]=data&0x00ff;
        used=1;
    }
}
void sb(word addr,byte data) {
    word pag=addr/4096;
    word ofs=addr%4096;
    
    if (pag==page) {
        Mem[ofs]=data;
    }else {
        FILE* fp=fopen("virtual4K","rb+");
        if (used) {
            fseek(fp,4096*(page+1),SEEK_SET);
            fread(coMem,1,4096*(3-page),fp);
            fseek(fp,4096*page,SEEK_SET);
            fwrite(Mem,1,4096,fp);
            fseek(fp,4096*(page+1),SEEK_SET);
            fwrite(coMem,1,4096*(3-page),fp);
        }
        fseek(fp,4096*pag,SEEK_SET);
        fread(Mem,1,4096,fp);
        page=pag;
        fclose(fp);
        Mem[ofs]=data;
        used=1;
    }
    
}
int main(void) {
    FILE* fp=fopen("virtual4K","rb+");
    for (int i=0; i<=3; i++) {
        fwrite(Mem,1,16384,fp);
    }
    fclose(fp);     //initialize the files
    
    sw(16000,1);
    sb(16004,2);
    sh(16005,3);
    printf("%X\n",lw(16000));
    printf("%X\n",lb(16004));
    printf("%X\n",lh(16005));
    /*byte A[3]={1,2,4};
    fseek(fp,3,SEEK_SET);
    fwrite(A,1,3,fp);
    Mem[8]=1;
    Mem[9]=2;
    Mem[10]=3;
    fseek(fp,0,SEEK_SET);
    fseek(fp,8,SEEK_SET);
    fwrite(A,1,3,fp);
    fseek(fp,0,SEEK_SET);
    fread(Mem,1,4096,fp);
    Mem[11]=9;
    printf("%d\n%d\n%d\n%d\n%d\n%d\n",Mem[0],Mem[1],Mem[2],Mem[3],Mem[4],Mem[5]);
    printf("%d\n%d\n%d\n%d\n%d\n%d\n",Mem[6],Mem[7],Mem[8],Mem[9],Mem[10],Mem[11]);
    fclose(fp);*/
    
}
