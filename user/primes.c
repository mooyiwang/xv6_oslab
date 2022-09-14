#include "kernel/types.h"
#include "user.h"

int num[1];

int main(int argc,char* argv[]){
    if(argc != 1){
        fprintf(2, "error input! there is no args in primes\n");
        exit(-1);
    }
    
    int p1[2];
    pipe(p1);
    if(fork() == 0){
        close(p1[1]);
        read(p1[0], num, sizeof(num));
        fprintf(1, "prime %d\n", num[0]);

        int p2[2];
        pipe(p2);
        if(fork() == 0){
            close(p2[1]);
            read(p2[0], num, sizeof(num));
            fprintf(1, "prime %d\n", num[0]);

            int p3[2];
            pipe(p3);
            if(fork() == 0){
                close(p3[1]);
                read(p3[0], num, sizeof(num));
                fprintf(1, "prime %d\n", num[0]);

                int p4[2];
                pipe(p4);
                if(fork() == 0){
                    close(p4[1]);
                    read(p4[0], num, sizeof(num));
                    fprintf(1, "prime %d\n", num[0]);

                    int p5[2];
                    pipe(p5);
                    if(fork() == 0){
                        close(p5[1]);
                        read(p5[0], num, sizeof(num));
                        fprintf(1, "prime %d\n", num[0]);
                        
                        ///
                        int p6[2];
                        pipe(p6);
                        if(fork() == 0){
                            close(p6[1]);
                            read(p6[0], num, sizeof(num));
                            fprintf(1, "prime %d\n", num[0]);

                            ///
                            int p7[2];
                            pipe(p7);
                            if(fork() == 0){
                                close(p7[1]);
                                read(p7[0], num, sizeof(num));
                                int out = num[0];
                                fprintf(1, "prime %d\n", out);
                                
                                ///

                                int p8[2];
                                pipe(p8);
                                if(fork() == 0){
                                    close(p8[1]);
                                    read(p8[0], num, sizeof(num));
                                    int out = num[0];
                                    fprintf(1, "prime %d\n", out);
                                    
                                    ///
                                    
                                    int p9[2];
                                    pipe(p9);
                                    if(fork() == 0){
                                        close(p9[1]);
                                        read(p9[0], num, sizeof(num));
                                        int out = num[0];
                                        fprintf(1, "prime %d\n", out);
                                        
                                        ///

                                        int p10[2];
                                        pipe(p10);
                                        if(fork() == 0){
                                            close(p10[1]);
                                            read(p10[0], num, sizeof(num));
                                            int out = num[0];
                                            fprintf(1, "prime %d\n", out);
                                            
                                            ///
                                            int p11[2];
                                            pipe(p11);
                                            if(fork() == 0){
                                                close(p11[1]);
                                                read(p11[0], num, sizeof(num));
                                                int out = num[0];
                                                fprintf(1, "prime %d\n", out);
                                                close(p11[0]);
                                                exit(0);
                                            }
                                            close(p11[0]);
                                            while(read(p10[0], num, sizeof(num))){
                                                if(num[0]%out != 0){
                                                    write(p11[1], num, sizeof(num));
                                                }
                                            }
                                            close(p11[1]);
                                            wait(0);
                                            close(p10[0]);
                                            exit(0);
                                        }
                                        close(p10[0]);
                                        while(read(p9[0], num, sizeof(num))){
                                            if(num[0]%out != 0){
                                                write(p10[1], num, sizeof(num));
                                            }
                                        }
                                        close(p10[1]);
                                        wait(0);
                                        close(p9[0]);
                                        exit(0);
                                    
                                    }
                                    close(p9[0]);
                                    while(read(p8[0], num, sizeof(num))){
                                        if(num[0]%out != 0){
                                            write(p9[1], num, sizeof(num));
                                        }
                                    }
                                    close(p9[1]);
                                    wait(0);
                                    close(p8[0]);
                                    exit(0);
                                }
                                close(p8[0]);
                                while(read(p7[0], num, sizeof(num))){
                                    if(num[0]%out != 0){
                                        write(p8[1], num, sizeof(num));
                                    }
                                }
                                close(p8[1]);
                                wait(0);
                                close(p7[0]);
                                exit(0);
                            }
                            close(p7[0]);
                            while(read(p6[0], num, sizeof(num))){
                                if(num[0]%13 != 0){
                                    write(p7[1], num, sizeof(num));
                                }
                            }
                            close(p7[1]);
                            wait(0);
                            close(p6[0]);
                            exit(0);
                        }
                        close(p6[0]);
                        while(read(p5[0], num, sizeof(num))){
                            if(num[0]%11 != 0){
                                write(p6[1], num, sizeof(num));
                            }
                        }
                        close(p6[1]);
                        wait(0);
                        close(p5[0]);
                        exit(0);
                    }
                    close(p5[0]);
                    while(read(p4[0], num, sizeof(num))){
                        if(num[0]%7 != 0){
                            write(p5[1], num, sizeof(num));
                        }
                    }
                    close(p5[1]);
                    wait(0);
                    close(p4[0]);
                    exit(0);
                }
                close(p4[0]);
                while(read(p3[0], num, sizeof(num))){
                    if(num[0]%5 != 0){
                        write(p4[1], num, sizeof(num));
                    }
                }
                close(p4[1]);
                wait(0);
                close(p3[0]);
                exit(0);

            }
            close(p3[0]);
            while(read(p2[0], num, sizeof(num))){
                if(num[0]%3 != 0){
                    write(p3[1], num, sizeof(num));
                }
            }
            close(p3[1]);
            wait(0);
            close(p2[0]);
            exit(0);
        }
        close(p2[0]);
        while(read(p1[0], num, sizeof(num))){
            if(num[0]%2 != 0){
                write(p2[1], num, sizeof(num));
            }
        }
        close(p2[1]);
        wait(0);
        close(p1[0]);
        exit(0);
    }
    close(p1[0]);
    for(int i=2; i<=35; i++){
        num[0] = i;
        write(p1[1], num, sizeof(num));
    }
    close(p1[1]);
    wait(0);
    exit(0);
}