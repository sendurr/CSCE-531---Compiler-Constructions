%{
#include "reference.h"
%}

reg     r[0-7]


%%
[ \t]                               {
                                        ;
                                    }

#                                   {
                                        return COMMENT;
                                    }

(\n)                                {
                                        return NEW_LINE;
                                    }

.alloc                              {
                                        return ALLOC_INSTR;
                                    }

(load|loada|store)                  {
                                        return OPCODE_MEM;
                                    }

(move|add|sub|mul|div|mod|cmp)      {
                                        return OPCODE_REG;
                                    }

(b|blt|ble|bne|beq|bge|bgt)         {
                                        return OPCODE_BRN;
                                    }

(read|write)                        {
                                        return OPCODE_IO;
                                    }

r[0-7]                              {
                                        return REGISTER;
                                    }

("("{reg}")")                        {
                                        return INDIRECT;
                                    }

$[a-zA-Z_][a-zA-Z_0-9]*             {
                                        return LABEL;
                                    }

[a-zA-Z_][a-zA-Z_0-9]*	            {
                                        return IDENTIFIER;
                                    }

,                                   {
                                        return COMMA;
                                    }

:                                   {
                                        return COLON;
                                    }

[0-9][0-9]*				            {
                                        return INTEGER;
                                    }

.					                {
                                           printf("Unexceptional Charachter \n");
                                    }

%%

int yywrap(void)
	{
	return 1;
	}