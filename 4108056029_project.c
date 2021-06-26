#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int error_flag=0;
/** Trim()
  * remove char* string's front block,tab ,and end's block,tab,\n,\0 */
void Trim(char *);

/** search()
  * search a char* str in char (* table)[50], if exsit return 1, else return 0; */
int search(char* ,char (* )[50],int);

/** opcode_table_read()
  * turn opcode.txt(fptr_opcode) into char opcode_table[100][50] , return the number of lines in opcode_table */
int opcode_table_read(FILE* ,char (* )[50],int* );

/** pass_1()
  * build a symbol table with location and write in symbol_table.txt(fptr_SYMTAB)
  * symbol table            : char (* label)[50],char (* opcode)[50],char (* operand)[50],char (* opcode_table)[50],int* address
  * int opcode_table_n      : number of lines in opcode table
  * int n                   : number of lines in source code
  * int* location_counter   : after this function, location_counter == last location
  * return                  : the number of lines in opcode_table from START to END */
int pass_1(FILE*,FILE*,FILE*,char (*)[50],char (* )[50],char (*)[50],char (* )[50],int* ,int,int* );
int location_count(FILE* ,FILE* ,char (* )[50],char (*)[50],char (*)[50],char (*)[50],int*,int,int*,int);

/** search_address()
  * int search_address(char* str,char (* table)[50],int* address,int opcode_table_n)
  * search a string(char* str) in a table (char (* table)[50]), if exsit return its location(address[x]), else return -1; */
int search_address(char* ,char (* )[50],int* ,int );

/** object_program_build()
  * void object_program_build(FILE* fptr_object_code,FILE* fptr_object_program,char (* label)[50],char (* opcode)[50],char (* operand)[50],int* address,char (* opcode_table)[50],int* opcode_address,int location_counter,int opcode_table_n,int n)
  * make a source program with location and object code to object_code.txt(FILE* fptr_object_code),
  * and make a object program to object_program.txt(FILE* fptr_object_program)

  * symbol table            : char (* label)[50],char (* opcode)[50],char (* operand)[50],int* address
  * opcode table            : char (* opcode_table)[50],int* opcode_address

  * int location_counter    : last location
  * int n      : number of lines in source code from START to END */
void object_program_build(FILE* ,FILE* ,char (* )[50],char (* )[50],char (* )[50],int*,char (* )[50],int*,int,int,int);


int main(){
    FILE *fptr_source;
    FILE *fptr_opcode;
    FILE *fptr_location;
    FILE *fptr_SYMTAB;
    FILE *fptr_object_code;
    FILE *fptr_object_program;

    //location counter
    //after call symbol_table_build() , location_counter represent last location
    int location_counter;

    //symbol table
    char label[100][50];
    char opcode[100][50];
    char operand[100][50];
    int address[100];

    //opcode table
    char opcode_table[100][50];
    int opcode_address[100];



    fptr_source = fopen("source.txt","r");
    fptr_opcode = fopen("opcode.txt","r");
    fptr_location= fopen("location.txt","w");
    fptr_SYMTAB = fopen("symbol_table.txt","w");
    fptr_object_code = fopen("object_code.txt","w");
    fptr_object_program = fopen("object_program.txt","w");
    if ((fptr_source == NULL || fptr_opcode==NULL) ||(fptr_location==NULL || fptr_opcode==NULL)||(fptr_object_code==NULL || fptr_object_program==NULL)){
        printf("File Not Found");
        return 0;
    }
    else{
        //pass 1
        //lines in opcode_table
        int opcode_table_n =opcode_table_read(fptr_opcode,opcode_table,opcode_address);
        //lines in source code
        //int n = source_read(fptr_source,label,opcode,operand);
        //lines in source code from START to END
        //int n = symbol_table_build(fptr_SYMTAB,fptr_location,label,opcode,operand,opcode_table,address,opcode_table_n,n,&location_counter);
        //int n = pass_1();
        int  n =pass_1(fptr_source, fptr_SYMTAB,fptr_location,label,opcode,operand,opcode_table,address,opcode_table_n,&location_counter);
        if(n<=0) return 0;
        //printf("%d\n",n );
        //printf("pass 2\n");
        //pass 2

        //object code
        int object_code[n];
        char object_program[n][100];
        object_program_build(fptr_object_code,fptr_object_program,label,opcode,operand,address,opcode_table,opcode_address,location_counter,opcode_table_n,n);
        //printf("pass over\n");
        fclose(fptr_source);
        fclose(fptr_opcode);
        return 0;
    }
}
void Trim(char *src){
    char *begin = src;
    char *end   = src;
    while ( *end++ ) ;
    end--;
    if ( begin == end ) return;

    //remove ' ','\t'
    while ( *begin == ' ' || *begin == '\t' )
        ++begin;
    while ( (*end) == '\n' || (*end) == '\0' || (*end)== ' ' || (*end) == '\t'){
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
int search(char* str,char (* table)[50],int n){
    for(int i=0;i<n;i++){
        if(strcmp(str,table[i])==0){
            return 1;
        }
    }
    return 0;
}
int opcode_table_read(FILE* fptr_opcode,char (* opcode_table)[50],int* opcode_address){//build opcode_table
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
    if (line){
        free(line);
    }
    return counter;
}
int pass_1(FILE* fptr_source,FILE* fptr_SYMTAB,FILE* fptr_location,char (* label)[50],char (* opcode)[50],char (* operand)[50],char (* opcode_table)[50],int* address,int opcode_table_n,int* location_counter){
    size_t len = 0;
    ssize_t read;
    int counter = 0;
    char *line = NULL;
    char* p_label;
    char* p_opcode;
    char* p_operand;
    //turn opcode.txt(fptr_opcode) into label[][], opcode[][],operand[][]
    //first line need to check whether START is exsit
    while ((read = getline(&line, &len, fptr_source) != -1)){
        if(line[0]=='/') {continue;}          //comment line
        else{
            if(line[0]=='\t'){                //no label => label = tab
                strcpy(label[counter],"\t\0");
                p_opcode = strtok(line,"\t"); //split p_opcode by tab
            }
            else{                             //label exist
                p_label = strtok(line,"\t");
                if(p_label){
                    Trim(p_label);            //remove somthing at head and tail
                    strcpy(label[counter], p_label);
                    p_opcode=strtok(NULL,"\t");
                }
            }
            if(p_opcode){                           //opcode
                p_operand=strtok(NULL,"\t");
                if(p_operand){                      //has operand
                    Trim(p_opcode);
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
            else{                         //no operand , no address
                strcpy(opcode[counter],"\t\0");
                strcpy(operand[counter],"\t\0");
            }
            if(strcmp(opcode[0],"START")==0){//opcode ==START
                *location_counter=(int)strtol(operand[0], NULL, 16);
                address[0] = *location_counter;
            }
            else{                            //NO START
                *location_counter=0;
                address[0] = 0;
                int result = location_count(fptr_SYMTAB,fptr_location,label,opcode,operand, opcode_table,address,opcode_table_n,location_counter,counter);
                printf("location_counter %X\n",*location_counter );
                if(result==-1) return counter+1;
            }
            if(strcmp(label[0],"\t")==0) {
                fprintf(fptr_location,"%04X\t\t%s\t%s\n",address[0],opcode[0],operand[0] );
            }
            else {
                fprintf(fptr_location,"%04X\t%s\t%s\t%s\n",address[0],label[0],opcode[0],operand[0] );
            }
            counter++;
            break;
        }
    }
    //other lines
    while ((read = getline(&line, &len, fptr_source) != -1)){
        if(line[0]=='/') {continue;}
        if(line[0]=='\t'){              //two block(no label)
            strcpy(label[counter],"\t\0");//label==0
            p_opcode = strtok(line,"\t");
        }
        else{
            p_label = strtok(line,"\t");
            if(p_label){                      //label
                Trim(p_label);
                strcpy(label[counter], p_label);
                //printf("%s\n",label[counter] );
                p_opcode=strtok(NULL,"\t");
            }
        }
        if(p_opcode){                  //opcode
            p_operand=strtok(NULL,"\t");
            if(p_operand){                      //has operand
                Trim(p_opcode);
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
        int result = location_count(fptr_SYMTAB,fptr_location,label,opcode,operand, opcode_table,address,opcode_table_n,location_counter,counter);
        if(result==-1) return counter+1;
        //if(result==-2) return 0;
        counter++;
    }
    fclose(fptr_location);
    fclose(fptr_SYMTAB);
    return counter;
}
int location_count(FILE* fptr_SYMTAB,FILE* fptr_location,char (* label)[50],char (* opcode)[50],char (* operand)[50],char (* opcode_table)[50],int* address,int opcode_table_n,int* location_counter,int i){
        if(strcmp(opcode[i],"END")==0) {
            fprintf(fptr_location,"\t\t%s\t%s\n",opcode[i],operand[i] );
            return -1;
        }
        address[i] = *location_counter;
        //printf("%s\n",opcode[i] );
        //printf("address %d %X\n",i,*location_counter );
        //label ___________________________________________________________label
        //if label[i] not tab -> write label[i] and address[i] into symbol_table.txt
        if(strcmp(label[i],"\t")!=0 ) fprintf(fptr_SYMTAB,"%s\t%X\n",label[i],address[i]);
        if((strcmp(label[i],"\t")!=0) && (search(label[i],label,i)==1)){//label is already exsit in front
            printf("error\n");
            printf("lebal is exsist\n");
            error_flag=1;
            return -2;
        }

        //LOC ___________________________________________________________LOC
        //search opcode[i] in opcode_table
        if(search(opcode[i],opcode_table,opcode_table_n)==1){//exsit
            *location_counter+=3;
        }
        else if (strcmp(opcode[i],"WORD")==0){//location_counter+3
            *location_counter+=3;
        }
        else if (strcmp(opcode[i],"RESW")==0){//location_counter + 3*operand
            *location_counter+= 3*((int)strtol(operand[i], NULL, 10));
        }
        else if (strcmp(opcode[i],"RESB")==0){//location_counter + operand
            *location_counter+= (int)strtol(operand[i], NULL, 10);
        }
        else if (strcmp(opcode[i],"BYTE")==0){
            int len=0;
            if(operand[i][0]=='C') len = strlen(operand[i])-3; //number of char in ' '
            else if(operand[i][0]=='X') len = (strlen(operand[i])-3)/2;//two hex==1 byte
            *location_counter+= len;
        }
        else{
            error_flag=1;
            printf("error : opcode %s not exsist\n",opcode[i]);
            return -2;
        }

        if(strcmp(label[i],"\t")==0) {//label==tab
            fprintf(fptr_location,"%04X\t\t%s\t%s\n",address[i],opcode[i],operand[i] );
        }
        else {
            fprintf(fptr_location,"%04X\t%s\t%s\t%s\n",address[i],label[i],opcode[i],operand[i] );
        }
        return 0;
}
int search_address(char* str,char (* table)[50],int* address,int opcode_table_n){
    for(int i=0;i<opcode_table_n;i++){
        if(strcmp(str,table[i])==0){
            return address[i];
        }
    }
    return -1;//str not exsit in table
}
void object_program_build(FILE* fptr_object_code,FILE* fptr_object_program,char (* label)[50],char (* opcode)[50],char (* operand)[50],int* address,char (* opcode_table)[50],int* opcode_address,int location_counter,int opcode_table_n,int n){
    /** start           : start line, if first line's opcode is START,then start=1
      * lines           : at object program's which line
      * opcode_num      : use search_address() search opcode in opcode table
      * operand_num     : use search_address() search operand in symbol table
      * object_program  : final object object_program
      * object_code     : each comment line's object code
      * newline         : 1 =>is the head of line in object program
    */
    int start=0,lines=0;
    int opcode_num,operand_num;
    char object_program[n][100];
    int object_code[n];
    int newline=0;

    //first line
    if(strcmp(opcode[0],"START")==0){
        start=1;    //other line is start at 1
        lines++;    //move to next line
        //col 1 :  Header record
        object_program[0][0] = 'H';
        //col 1-6 : name
        for(int j=1;j<=6;j++){
            if(j<=strlen(label[0]))object_program[0][j] = label[0][j-1];
            else object_program[0][j] = ' ';
        }
        //col 7-12 : starting address
        sprintf(&object_program[0][7],"%06X",address[0]);

        //col 13-18 : length
        int len = location_counter-address[0];//location_counter == last location
        sprintf(&object_program[0][13], "%06X",len );

        //print to object_program.txt and object_code.txt
        fprintf(fptr_object_program, "%s\n",object_program[0]);
        fprintf(fptr_object_code,"%04X\t%s\t%s\t%s\n",address[0],label[0],opcode[0],operand[0]);
    }

    //initialize first text record
    int col;    //now is at which col
    int start_address = address[start]; //this line's starting address
    //col 1 : Text record
    object_program[lines][0] = 'T';
    //col 1-6 : starting address
    sprintf(&object_program[lines][1],"%06X",address[start]);
    //col 7-8 : Length of object code in this record in bytes
    sprintf(&object_program[lines][7],"%02X",0);//initialize length to 00, later count it real length
    //col 9-68 : Object code (hex) (2 columns per byte)
    col=9;
    for(int i=start;i<n;i++){
        //last line (END)
        if(strcmp(opcode[i],"END")==0) {
            if(strcmp(label[i],"\t")==0) {//no label
                fprintf(fptr_object_code,"\t\t%s\t%s\n",opcode[i],operand[i]);
            }
            else fprintf(fptr_object_code,"\t%s\t%s\t%s\n",label[i],opcode[i],operand[i]);

            //col 7-8 : Length of object code in this record in bytes
            //turn length from int into string, so it can insert in object_program[lines]
            char string_copy_temp[100];              //a temp string to save length
            int len = location_counter-start_address;//length
            sprintf(string_copy_temp,"%02X",len);    //turn length from int into string
            for(int k=0;k<2;k++){                    //insert in object_program[lines]
                object_program[lines][k+7] = string_copy_temp[k];
            }

            //print to object_program.txt
            fprintf(fptr_object_program, "%s\n",object_program[lines]);
            lines++;

            //End record
            //col 1
            object_program[lines][0] = 'E';

            //col 1-6 : starting address
            sprintf(&object_program[lines][1],"%06X",address[0]);

            //print to object_program.txt
            fprintf(fptr_object_program, "%s\n",object_program[lines]);
            return;
        }

        //search opcode in opcode table
        opcode_num = search_address(opcode[i],opcode_table,opcode_address,opcode_table_n);
        // if opcode exist in opcode table
        if(opcode_num!=-1){
            //is the head of this line
            if(newline==1){
                //col 1 : Text record
                object_program[lines][0] = 'T';
                //col 1-6 : starting address
                sprintf(&object_program[lines][1],"%06X",start_address);
                //col 7-8 : Length of object code in this record in bytes
                sprintf(&object_program[lines][7],"%02X",0);//initialize length to 00, later count it real length
                col=9;
                newline=0;//not the head
            }

            //operand != "\t"
            if(strcmp(operand[i],"\t")!=0){
                //search operand in symbol table
                operand_num = search_address(operand[i],label,address,opcode_table_n);
                //operand not exsit in symbol table
                if(operand_num==-1) {

                    //check if there has ,
                    int node=-1;
                    for(int j=0;j<strlen(operand[i]);j++){
                        if(operand[i][j]==','){
                             node = j;
                        }
                    }
                    //there has ','
                    if(node!=-1) {
                        char temp[100];
                        strcpy(temp,operand[i]);//copy operand[i] to temp
                        char *p = strtok(temp, ",");//split temp by ,
                        //search first part of operand[i] in symbol table
                        operand_num = search_address(p,label,address,opcode_table_n);
                        //operand not exsit in symbol table
                        if(operand_num==-1){
                            error_flag=1;
                            printf("error : operand not exsit in symbol table\n");
                        }

                        //'X add 8000h
                        object_code[i] = (opcode_num<<16) + (operand_num+8*4096);

                        if(strcmp(label[i],"\t")==0) {//label is tab
                            fprintf(fptr_object_code,"%04X\t\t%s\t%s\t%06X\n",address[i],opcode[i],operand[i],object_code[i]);
                        }
                        else fprintf(fptr_object_code,"%04X\t%s\t%s\t%s\t%06X\n",address[i],label[i],opcode[i],operand[i],object_code[i]);
                    }
                    else{//operand not exsit in symbol table
                        printf("%d %X\t%s\t%s\n",i,address[i],label[i],opcode[i]);
                        printf("1 error\n");
                        return ;
                    }
                }
                //operand exsit in symbol table
                else{
                    object_code[i] = (opcode_num<<16) + operand_num;
                    if(strcmp(label[i],"\t")==0) {
                        fprintf(fptr_object_code,"%04X\t\t%s\t%s\t\t%06X\n",address[i],opcode[i],operand[i],object_code[i]);
                    }
                    else fprintf(fptr_object_code,"%04X\t%s\t%s\t%s\t\t%06X\n",address[i],label[i],opcode[i],operand[i],object_code[i]);
                }
                sprintf(&object_program[lines][col], "%06X",object_code[i]);
                col+=6;
            }
            //opcode is in opcode table, operand == "\t"
            else{
                object_code[i] = (opcode_num<<16) ;//set operand address=0
                sprintf(&object_program[lines][col], "%06X",object_code[i]);
                if(strcmp(label[i],"\t")==0) {
                    fprintf(fptr_object_code,"%04X\t\t%s\t\t\t%06X\n",address[i],opcode[i],object_code[i]);
                }
                else fprintf(fptr_object_code,"%04X\t%s\t%s\t\t\t%06X\n",address[i],label[i],opcode[i],object_code[i]);
                col+=6;
                //if opcode != RSUB
                if(strcmp(opcode[i],"RSUB")!=0) error_flag=1;
            }
        }

        //not exsit in opcode table , opcode == BYTE
        else if(strcmp(opcode[i],"BYTE")==0){
            if(newline==1){
                //col 1 : Text record
                object_program[lines][0] = 'T';
                //col 1-6 : starting address
                sprintf(&object_program[lines][1],"%06X",start_address);
                //col 7-8 : Length of object code in this record in bytes
                sprintf(&object_program[lines][7],"%02X",0);//initialize length to 00, later count it real length
                col=9;
                newline=0;//not the head
            }
            if(operand[i][0]=='C'){
                object_code[i]=0;
                int shift=0;

                char char_temp[2];//each char's ascii code in two byte
                char object_code_string[100];//word's object code in string
                int k= ((strlen(operand[i])-3)<<1) -1;//last char's position
                for(int j=strlen(operand[i])-2;operand[i][j]!='\'';j--){//char in ' '
                    sprintf(char_temp,"%02X",operand[i][j]);            //operand[i][j](char) -> int(hex) -> string(char_temp)
                    object_program[lines][k+col] = char_temp[1];
                    object_code_string[k--] = char_temp[1];
                    object_program[lines][k+col] = char_temp[0];
                    object_code_string[k--] = char_temp[0];

                    object_code[i] += operand[i][j]<<shift;
                    shift+=8;//one char two byte
                    col+=2;
                }
                if(strcmp(label[i],"\t")==0) {
                    fprintf(fptr_object_code,"%04X\t\t%s\t%s\t\t%s\n",address[i],opcode[i],operand[i],object_code_string);
                }
                else fprintf(fptr_object_code,"%04X\t%s\t%s\t%s\t\t%s\n",address[i],label[i],opcode[i],operand[i],object_code_string);
            }
            else if(operand[i][0]=='X') {
                char temp[100];
                strcpy(temp,operand[i]);//copy operand[i] to temp
                //move int in ' '
                char *src = operand[i];
                char *begin = src;
                char *end   = src;
                while ( *end++ ) {
                    ; // Statement is null
                }
                end--;
                begin+=2;//remove X'
                end-=2;//remove' \0
                if ( begin > end ) {
                    *src = '\0';
                }
                while ( begin != end ) {
                    *src++ = *begin++;
                }
                *src++ = *end;
                *src = '\0';

                object_code[i] = ((int)strtol(operand[i], NULL, 16));
                for(int k=0;k<strlen(operand[i]);k++){
                    object_program[lines][k+col] = operand[i][k];
                }
                col+=strlen(operand[i]);
                if(strcmp(label[i],"\t")==0) {                                         //temp == origin operand
                    fprintf(fptr_object_code,"%04X\t\t%s\t%s\t\t%s\n",address[i],opcode[i],temp,operand[i]);
                }
                else fprintf(fptr_object_code,"%04X\t%s\t%s\t%s\t\t%s\n",address[i],label[i],opcode[i],temp,operand[i]);
                strcpy(operand[i],temp);
            }
        }
        else if(strcmp(opcode[i],"WORD")==0){
            if(newline==1){
                //col 1 : Text record
                object_program[lines][0] = 'T';
                //col 1-6 : starting address
                sprintf(&object_program[lines][1],"%06X",start_address);
                //col 7-8 : Length of object code in this record in bytes
                sprintf(&object_program[lines][7],"%02X",0);//initialize length to 00, later count it real length
                col=9;
                newline=0;//not the head
            }
            //turn operand into int
            object_code[i] = ((int)strtol(operand[i], NULL, 10));
            //insert object_code[i] in object_program[lines][col]
            sprintf(&object_program[lines][col],"%06X",object_code[i]);
            col+=6;
            if(strcmp(label[i],"\t")==0) {
                fprintf(fptr_object_code,"%04X\t\t%s\t%s\t\t%06X\n",address[i],opcode[i],operand[i],object_code[i]);
            }
            else fprintf(fptr_object_code,"%04X\t%s\t%s\t%s\t\t%06X\n",address[i],label[i],opcode[i],operand[i],object_code[i]);

        }
        //opcode[i] not exsit in opcode table, not WORD,BYTE
        else{
            if(strcmp(label[i],"\t")==0) {
                fprintf(fptr_object_code,"%04X\t\t%s\t%s\n",address[i],opcode[i],operand[i]);
            }
            else fprintf(fptr_object_code,"%04X\t%s\t%s\t%s\n",address[i],label[i],opcode[i],operand[i]);
            if(col==9){//this lines still is empty -> next line
                printf("start_address %06X\n",start_address );
                start_address = address[i+1];
                continue;
            }
            //col 7-8 : Length of object code in this record in bytes(hex)
            char length[2];
            int len = address[i]-start_address;
            sprintf(length,"%02X",len);//copy len(int) to length(string)
            for(int k=0;k<2;k++){      //insert length in object_program
                object_program[lines][k+7] = length[k];
            }
            fprintf(fptr_object_program, "%s\n",object_program[lines]);
            lines++;
            start_address = address[i+1];//set start_address = next Statement's address
            newline=1;//before next source Statement is head
            col=9;    //next object code's col is 9
            continue;
        }
        //col 9-68 : Object code (hex) (2 columns per byte)
        if(col+6>69){
            //length (7-8)
            char string_copy_temp[100];
            int len = address[i+1]-start_address;
            sprintf(string_copy_temp,"%02X",len);
            for(int k=0;k<2;k++){
                object_program[lines][k+7] = string_copy_temp[k];
            }
            fprintf(fptr_object_program, "%s\n",object_program[lines]);
            lines++;
            start_address = address[i+1];
            newline=1;//before next source Statement is head
            col=9;    //next object code's col is 9
        }
    }
    fclose(fptr_object_code);
    fclose(fptr_object_program);
}
