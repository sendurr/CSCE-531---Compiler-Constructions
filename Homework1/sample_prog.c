#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "reference.h"

extern char get_opcode( char keyword[]);
extern char get_regno( char keyword[]);

int main() {
 
   int rad ,i=23;
   float PI = 3.14, area, ci;
 
   printf("\nEnter radius of circle: ");
   scanf("%d", &rad);
 
   area = PI * rad * rad;
   printf("\nArea of circle : %f ", area);
 
   ci = 2 * PI * rad;
   printf("\nCircumference : %f ", ci);



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

   instr instr_array[MAX_SIZE];

   char ip[] ="bgt" , x[12] , y , reg_ip[]="r7";
   printf("\n The size of instr is %li \n", sizeof(x));


   //snprintf(x, 12,"%d",get_opcode(ip));

   //printf("\n  after conversion %s \n" , x );


   //y =  get_opcode(ip)+'a';
   instr_array[0].op_code = get_opcode(ip);
   printf("\n  asci code %d \n" , instr_array[0].op_code );
   instr_array[0].reg1no= get_regno(reg_ip);
   printf("\n  asci code %d \n" , instr_array[0].reg1no);

   int p =12;

   char z;

   z = 23;
   printf("\n converted into string %c  \n", z );



   return (0);
}