#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
    size_t len = 0;
    ssize_t read;
    char *line = NULL;
    FILE *fptr_source;
    FILE *fptr_opcaode;
    int location_counter;
    int counter = 0;
    char label[100][10];
    char opcode[100][10];
    char operand[100][10];
    int address[100];

    printf("%s\n",label[0]);
    //char* source[100];
    //char *p;
    fptr_source = fopen("source.txt","r");
    fptr_opcaode = fopen("opcode.txt","r");
    if (fptr_source == NULL || fptr_opcaode==NULL){
        printf("File Not Found");
        return 0;
    }
    else{
        //char* p;
        if((read = getline(&line, &len, fptr_source) != -1)){//first line
            //printf("%d line %s",counter, line);
            char* p_label,*p_opcode,*p_operand;
            //source[counter] = line;
            if(line[0]==' '){           //two block(no label)
                //label[counter][0]='\t';     //label==0
                strcpy(label[counter],"\t");
                p_opcode = strtok(line,"\t");
            }
            else{                          //three block
                p_label = strtok(line,"\t");
                if(p_label){                      //label
                    //printf("1. %s\n",p_label );
                    //label[counter]=p_label;
                    strcpy(label[counter], p_label);
                    //printf("aaa\n");
                    p_opcode=strtok(NULL,"\t");
                }
            }
            if(p_opcode){                  //opcode
                //printf("2. %s\n",p_opcode );
                //opcode[counter]=p_opcode;
                strcpy(opcode[counter], p_opcode);
                if(strcmp(p_opcode,"START")==0){            //START
                    p_operand=strtok(NULL,"\t");
                    if(p_operand){                          //has operand
                        //printf("3. %s\n",p_operand );
                        //operand[counter] = p_operand;
                        strcpy(operand[counter], p_operand);
                        address[counter]=(int)strtol(p_operand, NULL, 16);
                        location_counter=address[counter];
                    }
                    else{                      //no operand , no address
                        //operand[counter][0] = '\t';//no operand
                        strcpy(operand[counter],"\t");
                        address[counter] = 0;
                        location_counter = 0;
                    }
                }
                else{//opcode is not start
                    address[counter]=0;
                    location_counter=0;
                    p_operand=strtok(NULL,"\t");
                    if(p_operand){                      //has operand
                        //printf("3. %s\n",p_operand );
                        //operand[counter] = p_operand;
                        strcpy(operand[counter], p_operand);
                    }
                    else{                      //no operand , no address
                        //operand[counter][0] = '\t';//no operand
                        strcpy(operand[counter],"\t");
                    }
                }
            }
            else{                      //no opcode and operand
                //opcode[counter][0]= '\t';
                strcpy(opcode[counter],"\t");
                //operand[counter][0] = '\t';
                strcpy(operand[counter],"\t");
            }
            //printf("%d : __________________ %x %s %s %s\n",counter,address[counter],label[counter],opcode[counter],operand[counter]);
            //printf("operand %s",operand[counter]);
            counter++;
        }
        //printf("00000000000000000:  %x %s %s %s\n",address[0],label[0],opcode[0],operand[0]);

        while ((read = getline(&line, &len, fptr_source) != -1)){//other line
            //printf("00000000000000000:  %x %s %s %s\n",address[0],label[0],opcode[0],operand[0]);
            //printf("\n%d line %s",counter, line);
            char* p_label;
            char* p_opcode;
            char* p_operand;
            //printf("address %p %p %p %p",p_label,p_opcode,p_operand,*label[0]);
            if(line[0]==' '){           //two block(no label)
                //label[counter][0]='\t';     //label==0
                strcpy(label[counter],"\t");
                p_opcode = strtok(line,"\t");
            }
            else{                          //three block
                p_label = strtok(line,"\t");
                if(p_label){                      //label
                    //printf("1. %s\n",p_label );
                    //label[counter]=p_label;
                    strcpy(label[counter], p_label);
                    p_opcode=strtok(NULL,"\t");
                }
            }
            if(p_opcode){                          //opcode
                //printf("2. %s\n",p_opcode );
                //opcode[counter]=p_opcode;
                //strcpy(label[counter], p_label);
                strcpy(opcode[counter], p_opcode);
                p_operand=strtok(NULL,"\t");
                if(p_operand){                      //address
                    //printf("3. %s\n",p_operand);
                    //operand[counter] = p_operand;
                    strcpy(operand[counter], p_operand);
                }
                else{                      //no address
                    //operand[counter][0] = '\t';//no operand
                    strcpy(operand[counter],"\t");
                }
            }
            else{                      //no opcode and operand
                //opcode[counter][0] = '\t';
                strcpy(opcode[counter],"\t");
                strcpy(operand[counter],"\t");
            }

            address[counter]=location_counter;
            location_counter=address[counter];
            //printf("%d : __________________ %x %s %s %s\n",counter,address[counter],label[counter],opcode[counter],operand[counter]);
            //printf("operand %s",operand[counter]);
            counter++;
            location_counter+=3;
            fflush(stdin);
        }
        //printf("%x %s %s %s\n",address[0],label[0],opcode[0],operand[0]);
        printf("\n-----------------------------------------------------------------------\n");
        char *tmp = NULL;
        for(int i=0;i<counter;i++){
            if ((tmp = strstr(opcode[i], "\n"))){
                *tmp = '\0';
            }
            if ((tmp = strstr(label[i], "\n"))){
                *tmp = '\0';
            }
            if ((tmp = strstr(operand[i], "\n"))){
                *tmp = '\0';
            }
            //printf("\n%d: \nlabel:%s\n",i,label[i] );
            //printf("--------------------------\n");
            //printf("opcode:%s\n",opcode[i]);
            //printf("--------------------------\n");
            //printf("operand:%s\n",operand[i]);
            //printf("--------------------------\n");
            printf("%X %s %s %s\n",address[i],label[i],opcode[i],operand[i]);
        }
        fclose(fptr_source);
        if (line){
            free(line);
        }
        return 0;
    }
}
