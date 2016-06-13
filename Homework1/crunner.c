#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "reference.h"


char get_opcode( char keyword[]);
char get_regno( char keyword[]);
int get_indirect_mem_ref( char keyword[]);

extern int yylex();
extern int yylineno;
extern char *yytext;


int get_inden_loc(char iden[]);
char alloc_indentifier_check(char iden[]);
int alloc_instr_func(int i);
int opcode_mem_func(int i);
int opcode_reg_func(int i);
int opcode_brn_func(int i);
int opcode_lab_func(int i);
int opcode_io_func(int i);
int build_sym_tbl_lb( char iden[]);
int get_label_loc(char iden[]);
int get_label_stat(char iden[]);


struct flex_var
{
    int token;
    char text[20];
};

struct symbol_tbl
{
    char lab[20];
    int lab_point;
    char status[];
    // status = 'r' is resolved
    // status = 'u' is unresolved
};

typedef struct
{
    unsigned char op_code;
    unsigned char mode: 5;
    unsigned char reg1no: 3;
    union
    {
        unsigned short loc;
        short imm_const;
        char reg2no;
    } arg2;
}instr;

struct flex_var flex_op[1000];
struct symbol_tbl symbol_array[1000];
instr instr_array[MAX_SIZE];

int mem_point =0 , instr_point =1 , symbol_tbl_point =0;

enum mode_ref {register_mode ,direct_mode ,indirect_mode ,imm_constant_mode, branch_mode };
enum mode_ref mode_ref1;

int main()
{
    int ntoken , vtoken , i=0 , array_len;
    char *ref[] = {NULL , "ALLOC_INSTR 1","OPCODE_MEM 2", "OPCODE_REG 3", "OPCODE_BRN 4", "OPCODE_IO 5", "REGISTER 6",
                   "INDIRECT 7", "INTEGER 8", "IDENTIFIER 9", "LABEL 10", "COMMA 11", "COLON 12"};


    ntoken = yylex();

    while(ntoken != 0)
    {
        if(ntoken == COMMENT) // code to remove comments
        {
            ntoken = yylex();
            while(ntoken != NEW_LINE && ntoken != 0)
            {
                ntoken = yylex();
            }
        }
        if (ntoken != NEW_LINE  && ntoken != 0) // code to remove only newline
        {
            flex_op[i].token = ntoken;
            strcpy(flex_op[i].text, yytext);
            i++;
        }
        ntoken = yylex();
    }

    printf("Printing all the tokens from flex \n");

    array_len = i;

    // Testing to see if all tokens are identified
    for (i=0; i<array_len ; i++)
    {
        vtoken =flex_op[i].token;
        printf(" %d = %s  which is %s \n", flex_op[i].token, flex_op[i].text,ref[vtoken]);
    }

    // Loop through the token to arrive at the instruction set
    for (i=0; i<array_len ; i++)
    {
        switch(flex_op[i].token)
        {
            case ALLOC_INSTR:
            {
                i=alloc_instr_func(i);
                break;
            }
            case OPCODE_MEM:
            {
                i=opcode_mem_func(i);
                break;
            }
            case OPCODE_REG:
            {
                i=opcode_reg_func(i);
                break;
            }
            case OPCODE_BRN:
            {
                i=opcode_brn_func(i);
                break;
            }
            case LABEL:
            {
                i=opcode_lab_func(i);
                break;
            }
            case OPCODE_IO:
            {
                i=opcode_io_func(i);
                break;
            }
            default:
            {
                fprintf(stderr,"\n Syntax error : incorrect opcode! : %s \n",flex_op[i].text);
            }
        }
    }
    // print symbol table
    printf("\n Printing symbol table details");
    printf("\n Symbol   mem_pointer  status \n");
    for (i=0; i<symbol_tbl_point;i++)
    {
        printf("%s    %d    %s \n", symbol_array[i].lab,symbol_array[i].lab_point, symbol_array[i].status);
    }

    // print instruction table
    printf("\n Printing instruction table details");
    printf("\n opcode   mode   reg1  location \n");
    for (i=1; i<instr_point;i++)
    {
        printf("%d    %d    %d   %d \n", instr_array[i].op_code , instr_array[i].mode , instr_array[i].reg1no , instr_array[i].arg2.loc);
    }
    return 0;
}

// rules for input / output instructions

int opcode_io_func(int i)
{
    printf("\n %s ",flex_op[i].text);
    instr_array[instr_point].op_code= get_opcode(flex_op[i].text);
    i=i+1;
    if (flex_op[i].token == REGISTER)// register
    {
        printf("\n %s ",flex_op[i].text);
        instr_array[instr_point].reg1no=get_regno(flex_op[i].text);
        mode_ref1 = register_mode;
        instr_array[instr_point].mode= mode_ref1;
        instr_point = instr_point +1;
    }
    else
    {
        fprintf(stderr,"\n Syntax error : register missing! \n");
    }
    return i;
};

// rules for label instructions

int opcode_lab_func(int i)
{
    printf("\n %s ",flex_op[i].text);
    instr_array[instr_point].op_code= get_opcode(flex_op[i].text);
    i=i+1;
    if (flex_op[i].token == COLON)// label for branching
    {
        printf("\n %c ",flex_op[i].text[0]);
        // decrement the index to point to label and not colon
        i=i-1;
        build_sym_tbl_lb(flex_op[i].text);
        i=i+1;
    }
    else
    {
        fprintf(stderr,"\n Syntax error : colon  missing! \n");
    }
    return i;
};

// rules for branch instructions

int opcode_brn_func(int i)
{
    printf("\n %s ",flex_op[i].text);
    instr_array[instr_point].op_code= get_opcode(flex_op[i].text);
    i=i+1;
    if (flex_op[i].token == LABEL)// label for branching
    {
        printf("\n %s ",flex_op[i].text);
        instr_array[instr_point].arg2.loc=get_label_loc(flex_op[i].text);

        mode_ref1 = branch_mode;
        instr_array[instr_point].mode= mode_ref1;
        instr_point = instr_point +1;
    }
    else
    {
        fprintf(stderr,"\n Syntax error : label missing! \n");
    }
    return i;
};

// rules for register instructions

int opcode_reg_func(int i)
{
    printf("\n %s ",flex_op[i].text);
    instr_array[instr_point].op_code= get_opcode(flex_op[i].text);
    i=i+1;
    if (flex_op[i].token == REGISTER)//1st register
    {
        printf("\n %s ",flex_op[i].text);
        instr_array[instr_point].reg1no=get_regno(flex_op[i].text);
        i=i+1;
        if (flex_op[i].token == COMMA)
        {
            printf("\n %s ",flex_op[i].text);
            i=i+1;
            if (flex_op[i].token == REGISTER) //2nd register
            {
                printf("\n %s ",flex_op[i].text);
                instr_array[instr_point].arg2.reg2no=get_regno(flex_op[i].text);
                mode_ref1 = register_mode;
                instr_array[instr_point].mode= mode_ref1;
                instr_point = instr_point +1;
            }
            else if (flex_op[i].token == INTEGER) //immediate constant
            {
                printf("\n %s ",flex_op[i].text);
                instr_array[instr_point].arg2.imm_const=atoi(flex_op[i].text);
                mode_ref1 = imm_constant_mode;
                instr_array[instr_point].mode= mode_ref1;
                instr_point = instr_point +1;
            }
            else
            {
                fprintf(stderr,"\n register or immediate constant missing! \n");
            }
        }
        else
        {
            fprintf(stderr,"\n Syntax error : comma missing! \n");
        }
    }
    else
    {
        fprintf(stderr,"\n Syntax error : register missing! \n");
    }
    return i;
};

// rules for memory access instructions

int opcode_mem_func(int i)
{
    printf("\n %s ",flex_op[i].text);
    instr_array[instr_point].op_code= get_opcode(flex_op[i].text);
    i=i+1;
    if (flex_op[i].token == REGISTER)
    {
        printf("\n %s ",flex_op[i].text);
        instr_array[instr_point].reg1no=get_regno(flex_op[i].text);
        i=i+1;
        if (flex_op[i].token == COMMA)
        {
            printf("\n %s ",flex_op[i].text);
            i=i+1;
            if (flex_op[i].token == IDENTIFIER) //direct memory reference
            {
                printf("\n %s ",flex_op[i].text);
                instr_array[instr_point].arg2.loc=get_inden_loc(flex_op[i].text);
                mode_ref1 = direct_mode;
                instr_array[instr_point].mode= mode_ref1;
                instr_point = instr_point +1;
            }
            else if (flex_op[i].token == INDIRECT) //indirect memory reference
            {
                printf("\n %s ",flex_op[i].text);
                instr_array[instr_point].arg2.loc=get_indirect_mem_ref(flex_op[i].text);
                mode_ref1 = indirect_mode;
                instr_array[instr_point].mode= mode_ref1;
                instr_point = instr_point +1;
            }
            else
            {
                fprintf(stderr,"\n Identifier or indirect memory reference missing! \n");
            }
        }
        else
        {
            fprintf(stderr,"\n Syntax error : comma missing! \n");
        }
    }
    else
    {
        fprintf(stderr,"\n Syntax error : register missing! \n");
    }
    return i;
};


// rules for alloc instruction
int alloc_instr_func(int i)
{
    printf("\n %s ",flex_op[i].text);
    i=i+1;
    if (flex_op[i].token == IDENTIFIER)
    {
        printf("\n %s ",flex_op[i].text);
        if ((alloc_indentifier_check(flex_op[i].text)) == 'y')
        {
            fprintf(stderr,"\n Alloc indentifier already assigned!");
        }
        strcpy(symbol_array[symbol_tbl_point].lab,flex_op[i].text);
        i=i+1;
        if (flex_op[i].token == COMMA)
        {
            printf("\n %s ",flex_op[i].text);
            i=i+1;
            if (flex_op[i].token == INTEGER)
            {
                printf("\n %s ",flex_op[i].text);
                symbol_array[symbol_tbl_point].lab_point=mem_point;
                mem_point = mem_point + atoi(flex_op[i].text);
                symbol_tbl_point = symbol_tbl_point +1;
            }
            else
            {
                fprintf(stderr,"\n Alloc error integer!");
            }
        }
        else
        {
            printf("\n default size 1");
            symbol_array[symbol_tbl_point].lab_point=mem_point;
            mem_point = mem_point + 1;
            symbol_tbl_point = symbol_tbl_point + 1;
            i = i - 1;
        }
    }
    else
    {
        fprintf(stderr,"\n Alloc error identifier!");
    }
    return i;
};

// bulid the symbol table for the label
int build_sym_tbl_lb( char iden[])
{
    int lab_stat , start_loc , i , next_loc;
    char resolved[] ="r";

    lab_stat = get_label_stat(iden);

    switch (lab_stat)
    {
        case 0:
        {
            strcpy(symbol_array[symbol_tbl_point].lab,iden);
            symbol_array[symbol_tbl_point].lab_point=instr_point;
            strcpy(symbol_array[symbol_tbl_point].status,resolved);
            symbol_tbl_point = symbol_tbl_point +1;
            break;
        }
        case 1:
        {
            //fetch the index of the first match from bottom
            for (i=symbol_tbl_point-1;i>=0;i--)
            {
                if (strcmp(iden , symbol_array[i].lab) ==0)
                {
                    break;
                }
            }
            // now backpatch the instruction array
            start_loc = symbol_array[i].lab_point;
            strcpy(symbol_array[i].status,resolved);
            symbol_array[i].lab_point = instr_point;
            i = start_loc;
            do
            {
                next_loc = instr_array[i].arg2.loc;
                instr_array[i].arg2.loc = instr_point;
                i=next_loc;
            }while(i!=0);
            break;
        }
        case 2:
        {
            fprintf(stderr,"\n Error: Label %s already resolved!", iden);
        }
    }
    return 0;
};

// get the location of label from symbol table
int get_label_loc( char iden[])
{
    int lab_stat , temp_loc , i;
    char unresolved[] ="u";

    lab_stat = get_label_stat(iden);

    switch (lab_stat)
    {
        case 0:
        {
            strcpy(symbol_array[symbol_tbl_point].lab,iden);
            symbol_array[symbol_tbl_point].lab_point=instr_point;
            strcpy(symbol_array[symbol_tbl_point].status,unresolved);
            symbol_tbl_point = symbol_tbl_point +1;
            return 0;
        }
        case 1:
        {
            //fetch the index of the first match from bottom
            for (i=symbol_tbl_point-1;i>=0;i--)
            {
                if (strcmp(iden , symbol_array[i].lab) ==0)
                {
                    break;
                }
            }
            temp_loc = symbol_array[i].lab_point;
            symbol_array[i].lab_point=instr_point;
            return temp_loc;
        }
        case 2:
        {
            //fetch the index of the first match from bottom
            for (i=symbol_tbl_point-1;i>=0;i--)
            {
                if (strcmp(iden , symbol_array[i].lab) ==0)
                {
                    break;
                }
            }
            return symbol_array[i].lab_point;
        }
    }
    return 0;
};

// get the status of label from symbol table
int get_label_stat( char iden[])
{
    int i , lab_stat = 0 ;
    char resolved[] = "r";
    // lab_stat = 0 = not found
    // lab_stat = 1 = found & unresolved
    // lab_stat = 2 = found & resolved

    for (i=0;i<symbol_tbl_point;i++)
    {
        if (strcmp(iden , symbol_array[i].lab) ==0)
        {
            if(strcmp(resolved, symbol_array[i].status) ==0)
            {
                lab_stat =2;
            }
            else
            {
                lab_stat =1;
            }
            break;
        }
    }
    return lab_stat;
}

// get the location of identifier from symbol table
int get_inden_loc( char iden[])
{
    int i;
    for (i=0;i<symbol_tbl_point;i++)
    {
        if (strcmp(iden , symbol_array[i].lab) ==0)
        {

            return symbol_array[i].lab_point;
        }
    }
    fprintf(stderr,"\n Indetifier %s not declared!", iden);
    return 0;
};

// check to see if alloc identifier is declareds
char alloc_indentifier_check(char iden[])
{
    int i;
    for (i=0;i<symbol_tbl_point;i++)
    {
        if (strcmp(iden , symbol_array[i].lab) ==0)
        {

            return 'y';
        }
    }
    return 'n';
};


char get_opcode( char keyword[])
{
    int len;
    char i = 0;
    char *op_code_ref[]= {"load","loada","store", "move" , "add" , "sub" , "mul" , "div" , "mod"  , "cmp" , "b" , "blt" , "ble" , "bne" , "beg" , "bgt","read","write"};
    char x;

    for (i=0;i<18;i++)
    {
        if (strcmp(keyword , op_code_ref[i]) ==0)
        {
            break;
        }
    }
    //x=i+'a';
    return i;
};

char get_regno( char keyword[])
{
    int  len;
    char *reg_ref[]= {"r0" , "r1", "r2" , "r3" , "r4" , "r5" , "r6" , "r7"};
    char x , i;

    for (i=0;i<8;i++)
    {
        if (strcmp(keyword , reg_ref[i]) ==0)
        {
            break;
        }
    }
    //x=i+'0';
    return i;
};


int get_indirect_mem_ref( char keyword[])
{
    int  len , i;
    char *reg_ref[]= {"(r0)" , "(r1)", "(r2)" , "(r3)" , "(r4)" , "(r5)" , "(r6)" , "(r7)"};
    char x;

    for (i=0;i<8;i++)
    {
        if (strcmp(keyword , reg_ref[i]) ==0)
        {
            break;
        }
    }
    //x=i+'0';
    return i;
};




