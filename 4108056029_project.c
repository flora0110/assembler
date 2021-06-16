#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
    size_t len = 0;
    ssize_t read;
    char *line = NULL;
    FILE *fptr_source;
    FILE *fptr_opcaode;
    FILE *fptr_location;
    FILE *fptr_SYMTAB;
    int location_counter;
    int counter = 0;
    char label[100][10];
    char opcode[100][10];
    char operand[100][10];
    int address[100];

    fptr_source = fopen("source.txt","r");
    fptr_opcaode = fopen("opcode.txt","r");
    fptr_location= fopen("location.txt","w");
    fptr_SYMTAB = fopen("symbol_table.txt","w");
    if (fptr_source == NULL || fptr_opcaode==NULL ||fptr_location==NULL){
        printf("File Not Found");
        return 0;
    }
    else{
        if((read = getline(&line, &len, fptr_source) != -1)){//first line
            char* p_label,*p_opcode,*p_operand;
            if(line[0]=='\t'){           //two block(no label)
                strcpy(label[counter],"\t");//label==0
                p_opcode = strtok(line,"\t");
            }
            else{                          //three block
                p_label = strtok(line,"\t");
                if(p_label){                      //label
                    strcpy(label[counter], p_label);
                    p_opcode=strtok(NULL,"\t");
                }
            }
            if(p_opcode){                  //opcode
                strcpy(opcode[counter], p_opcode);
                if(strcmp(p_opcode,"START")==0){            //START
                    p_operand=strtok(NULL,"\t");
                    if(p_operand){                          //has operand
                        strcpy(operand[counter], p_operand);
                        address[counter]=(int)strtol(p_operand, NULL, 16);
                        location_counter=address[counter];
                    }
                    else{                      //no operand , no address
                        strcpy(operand[counter],"\t");//no operand
                        address[counter] = 0;
                        location_counter = 0;
                    }
                }
                else{//opcode is not start
                    address[counter]=0;
                    location_counter=0;
                    p_operand=strtok(NULL,"\t");
                    if(p_operand){                      //has operand
                        strcpy(operand[counter], p_operand);
                    }
                    else{                      //no operand , no address
                        strcpy(operand[counter],"\t");//no operand
                    }
                }
            }
            else{                      //no opcode and operand
                strcpy(opcode[counter],"\t");
                strcpy(operand[counter],"\t");
            }
            counter++;
        }

        while ((read = getline(&line, &len, fptr_source) != -1)){//other line
            char* p_label;
            char* p_opcode;
            char* p_operand;
            if(line[0]=='\t'){           //two block(no label)

                strcpy(label[counter],"\t\0");//label==0
                p_opcode = strtok(line,"\t");
            }
            else{                          //three block
                p_label = strtok(line,"\t");
                if(p_label){                      //label
                    strcpy(label[counter], p_label);
                    p_opcode=strtok(NULL,"\t");
                }
            }
            if(p_opcode){                          //opcode
                strcpy(opcode[counter], p_opcode);
                p_operand=strtok(NULL,"\t");
                if(p_operand){                      //address
                    strcpy(operand[counter], p_operand);
                }
                else{                      //no address
                    strcpy(operand[counter],"\t");//no operand
                }
            }
            else{                      //no opcode and operand
                strcpy(opcode[counter],"\t");
                strcpy(operand[counter],"\t");
            }

            address[counter]=location_counter;
            location_counter=address[counter];
            counter++;
            location_counter+=3;
            fflush(stdin);
        }
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
            fprintf(fptr_location, "%X  %s\t%s\t%s\n",address[i],label[i],opcode[i],operand[i]);
            printf("%X  %s\t%s\t%s\n",address[i],label[i],opcode[i],operand[i]);
            if(strcmp(label[i],"\t")!=0 && strcmp(opcode[i],"START")!=0){
                fprintf(fptr_SYMTAB, "%s\t%X\n",label[i],address[i]);
                //printf("%s\t%X\n",label[i],address[i]);
            }
        }
        //printf("\n", );
        fclose(fptr_source);
        fclose(fptr_opcaode);
        fclose(fptr_location);
        fclose(fptr_SYMTAB);
        if (line){
            free(line);
        }
        return 0;
    }
}
