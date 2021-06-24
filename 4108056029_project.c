#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void Trim(char *src){
    char *begin = src;
    char *end   = src;
    while ( *end++ ) {
        ; // Statement is null
    }
    *end--;
    if ( begin == end ) return;
    while ( *begin == ' ' || *begin == '\t' )
        ++begin;
    while ( (*end) == '\n' || (*end) == '\0' || *end == ' ' || *end == '\t'){
        --end;
    }
    if ( begin > end ) {
        *src = '\0';  return;
    }
    //printf("begin=%1.1s; end=%1.1s\n", begin, end);
    while ( begin != end ) {
        *src++ = *begin++;
    }

    *src++ = *end;
    *src = '\0';
    return;
}
int search(char* str,char (* table)[10],int n){
    for(int i=0;i<n;i++){
        //printf("%s\n",table[i] );
        if(strcmp(str,table[i])==0){
            return 1;
        }
    }
    return 0;
}
int opcode_table_read(FILE* fptr_opcode,char (* opcode_table)[10],int* opcode_address){//build opcode_table
    size_t len = 0;
    ssize_t read;
    char *line = NULL;
    char* p;
    int counter=0;
    while ((read = getline(&line, &len, fptr_opcode) != -1)){
        p = strtok(line," ");
        if(p){
            strcpy(opcode_table[counter],p);
            p = strtok(NULL," ");
            if(p){
                opcode_address[counter++]=(int)strtol(p, NULL, 16);
            }
        }
        //printf("%s\t%s\n",opcode_table[counter-1],opcode_address[counter-1]);
    }
    return counter;
}
int source_read(FILE* fptr_source,char (* label)[10],char (* opcode)[10],char (* operand)[10]){
    size_t len = 0;
    ssize_t read;
    int counter = 0;
    char *line = NULL;
    char* p_label;
    char* p_opcode;
    char* p_operand;
    while ((read = getline(&line, &len, fptr_source) != -1)){//other line
        if(line[0]=='/') {continue;}
        if(line[0]=='\t'){              //two block(no label)
            strcpy(label[counter],"\t\0");//label==0
            p_opcode = strtok(line,"\t");
        }
        else{
            p_label = strtok(line,"\t");
            if(p_label){                      //label
                strcpy(label[counter], p_label);
                //printf("%s\n",label[counter] );
                p_opcode=strtok(NULL,"\t");
            }
            else{
                strcpy(label[counter],"\t\0");
                strcpy(opcode[counter],"\t\0");
                strcpy(operand[counter],"\t\0");
            }
        }
        if(p_opcode){                  //opcode
            p_operand=strtok(NULL,"\t");
            if(p_operand){                      //has operand
                strcpy(opcode[counter], p_opcode);
                Trim(p_operand);
                strcpy(operand[counter], p_operand);
            }
            else{                      //no operand , no address
                Trim(p_opcode);
                strcpy(opcode[counter], p_opcode);
                strcpy(operand[counter],"\t\0");//no operand
            }
        }
        else{
            strcpy(opcode[counter],"\t\0");
            strcpy(operand[counter],"\t\0");
        }
        counter++;
    }
    return counter;
}
int main(){
    size_t len = 0;
    ssize_t read;
    char *line = NULL;
    FILE *fptr_source;
    FILE *fptr_opcode;
    FILE *fptr_location;
    FILE *fptr_SYMTAB;
    int location_counter;
    int counter = 0;
    int base=0;

    //symbol table
    char label[100][10];
    char opcode[100][10];
    char operand[100][10];
    int address[100];

    //opcode table
    char opcode_table[100][10];
    int opcode_address[100];

    fptr_source = fopen("SICXEsource.txt","r");
    fptr_opcode = fopen("opcode.txt","r");
    fptr_location= fopen("location.txt","w");
    fptr_SYMTAB = fopen("symbol_table.txt","w");
    if ((fptr_source == NULL || fptr_opcode==NULL) ||(fptr_location==NULL || fptr_opcode==NULL)){
        printf("File Not Found");
        return 0;
    }
    else{
        //fprintf(fptr_SYMTAB,"aaaaa");
        int opcode_table_n =opcode_table_read(fptr_opcode,opcode_table,opcode_address);
        /*for(int i=0;i<opcode_table_n;i++){
            printf("%s\t%X\n",opcode_table[i],opcode_address[i] );
        }*/
        int n = source_read(fptr_source,label,opcode,operand);
        /*for(int i=0;i<n;i++){
            if(strcmp(label[i],"\t")==0) printf("\t%s\t%s\n",opcode[i],operand[i] );
            else printf("%s\t%s\t%s\n",label[i],opcode[i],operand[i] );
        }*/
        //first line
        if(strcmp(opcode[0],"START")==0){
            location_counter=(int)strtol(operand[0], NULL, 16);
            address[0] = location_counter;
        }
        else{
            if(search(opcode[0],opcode_table,opcode_table_n)==0){
                printf("error\n");
                printf("first line : opcode not exsist\n");
                return 0;
            }
            location_counter=0;
            address[0] = 0;
        }
        //other line
        for(int i=1;i<n;i++){

            if(strcmp(opcode[i],"END")==0) break;
            address[i] = location_counter;
            //label ___________________________________________________________label
            if((strcmp(label[i],"\t")!=0) && (search(label[i],label,i)==1)){//label is exsit
                printf("error\n");
                printf("lebal is exsist\n");
                return 0;
            }
            //LOC ___________________________________________________________LOC
            if(search(opcode[i],opcode_table,opcode_table_n)==1){

                location_counter+=3;
            }
            else if (strcmp(opcode[i],"WORD")==0){
                location_counter+=3;
            }
            else if (strcmp(opcode[i],"RESW")==0){
                location_counter+= 3*((int)strtol(operand[i], NULL, 10));
            }
            else if (strcmp(opcode[i],"RESB")==0){
                location_counter+= (int)strtol(operand[i], NULL, 10);
            }
            else if (strcmp(opcode[i],"BYTE")==0){
                int len=0;
                if(operand[i][0]=='C') len = strlen(operand[i])-3;
                else if(operand[i][0]=='X') len = (strlen(operand[i])-3)/2;
                location_counter+= len;
            }
            else if (strcmp(opcode[i],"BASE")==0){
                location_counter+=3;
            }
            else{
                printf("error\n");
                printf("opcode not exsist\n");
                if(strcmp(label[i],"\t")==0) {
                    printf("%X\t%s\t%s\n",address[i],opcode[i],operand[i] );
                    //fprintf(fptr_SYMTAB,"%X\t%s\t%s\n",address[i],opcode[i],operand[i] );
                }
                else {
                    printf("%X\t%s\t%s\t%s\n",address[i],label[i],opcode[i],operand[i] );
                    //fprintf(fptr_SYMTAB,"%X\t%s\t%s\t%s\n",address[i],label[i],opcode[i],operand[i] );
                }
                return 0;
            }
        }
        for(int i=0;i<n;i++){
            if(strcmp(opcode[i],"END")==0){
                printf("\t\t%s\t%s\n",opcode[i],operand[i] );
                fprintf(fptr_SYMTAB,"\t\t%s\t%s\n",opcode[i],operand[i] );
                break;
            }
            if(strcmp(label[i],"\t")==0) {
                printf("%X\t%s\t%s\n",address[i],opcode[i],operand[i] );
                fprintf(fptr_SYMTAB,"%X\t%s\t%s\n",address[i],opcode[i],operand[i] );
            }
            else {
                printf("%X\t%s\t%s\t%s\n",address[i],label[i],opcode[i],operand[i] );
                fprintf(fptr_SYMTAB,"%X\t%s\t%s\t%s\n",address[i],label[i],opcode[i],operand[i] );
            }
        }
        /*
        if((read = getline(&line, &len, fptr_source) != -1)){//first line
            char* p_label,*p_opcode,*p_operand;
            if(line[0]=='\t'){           //two block(no label)
                strcpy(label[counter],"\t");//label==0
                p_opcode = strtok(line,"\t");
            }
            else{                          //three block
                p_label = strtok(line,"\t");
                if(p_label){                      //label
                    if(search(p_label,label,counter)==1){
                        printf("error\n");
                        return 0;
                    }
                    else{
                        strcpy(label[counter], p_label);
                        p_opcode=strtok(NULL,"\t");
                    }
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
                    if(search(p_label,label,counter)==1){
                        printf("error\n");
                        return 0;
                    }
                    else{
                        strcpy(label[counter], p_label);
                        p_opcode=strtok(NULL,"\t");
                    }
                }
            }
            if(p_opcode){                          //opcode
                if(strcmp(p_opcode,"END")==0){
                    address[counter]=location_counter;
                    location_counter=address[counter];
                    counter++;
                    strcpy(opcode[counter], p_opcode);
                    p_operand=strtok(NULL,"\t");
                    if(p_operand){                      //address
                        strcpy(operand[counter], p_operand);
                    }
                    else{                      //no address
                        strcpy(operand[counter],"\t");//no operand
                    }
                    break;
                }
                if(search(p_opcode,opcode_table,counter)==1){
                    address[counter]=location_counter;
                    //location_counter=address[counter];
                    strcpy(opcode[counter], p_opcode);
                    p_operand=strtok(NULL,"\t");
                    if(p_operand){                      //address
                        strcpy(operand[counter], p_operand);
                    }
                    else{                      //no address
                        strcpy(operand[counter],"\t");//no operand
                    }
                    counter++;
                    location_counter+=3;
                }
                else{
                    if(strcmp(p_opcode,"WORD")==0){
                        address[counter]=location_counter;
                        //location_counter=address[counter];
                        strcpy(opcode[counter], p_opcode);
                        p_operand=strtok(NULL,"\t");
                        if(p_operand){                      //address
                            strcpy(operand[counter], p_operand);
                        }
                        else{                      //no address
                            strcpy(operand[counter],"\t");//no operand
                        }
                        counter++;
                        location_counter+=3;
                    }
                    else if(strcmp(p_opcode,"RESW")==0){
                        address[counter]=location_counter;
                        strcpy(opcode[counter], p_opcode);
                        p_operand=strtok(NULL,"\t");
                        if(p_operand){                      //address
                            strcpy(operand[counter], p_operand);
                            location_counter += 3*((int)strtol(p_operand, NULL, 16));
                        }
                        else{                      //no address
                            strcpy(operand[counter],"\t");//no operand
                        }
                        counter++;
                    }
                    else if(strcmp(p_opcode,"RESB")==0){
                        address[counter]=location_counter;
                        strcpy(opcode[counter], p_opcode);
                        p_operand=strtok(NULL,"\t");
                        if(p_operand){                      //address
                            strcpy(operand[counter], p_operand);
                            location_counter += ((int)strtol(p_operand, NULL, 16));
                        }
                        else{                      //no address
                            strcpy(operand[counter],"\t");//no operand
                        }
                        counter++;
                    }
                    else{
                        address[counter]=location_counter;
                        //location_counter=address[counter];
                        counter++;
                        location_counter+=1;
                    }
                }
            }
            else{                      //no opcode and operand
                strcpy(opcode[counter],"\t");
                strcpy(operand[counter],"\t");
                address[counter]=location_counter;
                //location_counter=address[counter];
                counter++;
                location_counter+=3;
            }


            //fflush(stdin);
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
        */
        //printf("\n", );
        fclose(fptr_source);
        fclose(fptr_opcode);
        fclose(fptr_location);
        fclose(fptr_SYMTAB);
        if (line){
            free(line);
        }
        return 0;
    }
}
