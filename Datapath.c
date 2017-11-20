#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "logical_components.c"
#include "generic_functions.c"

#define MEMBASE 0
#define MEMSIZE 65536
#define AND 0
#define OR  1
#define ADD 2
#define SUB 6
#define SLT 7

/*   =================================================
     ||Implementing MIPS,Data_path and Control Unit ||
     =================================================
            Single-cycle execution.
            Mouzakitis Nikolaos,Crete 2017.
*/

struct label
{
    char lname[12];
    int address;
};

struct instruction32
{
    int i[32];  //32 -bit instruction.
};

int RAM[MEMSIZE];   //  machine 's  memory.

struct label label_mapping[32]; // mapping between labels and actual addresses in RAM memory.

struct instruction_parts
{
    int i31_26[6];  //opcode.
    int i25_21[5];  //rs.
    int i20_16[5];  //rt.
    int i15_11[5];  //rd.
    int i15_0[16];  //immidiate.
    int i5_0[6];    //funct.
};

struct Register             //  creating the register file.
{
    int regs[32];

}register_file;


void instruction_create(char *buffer,struct instruction32 *instruction)
{
    int k;

    for(k = 0; k < 32; k++)
    {
        instruction->i[k] = char_to_int(buffer[k]);
    }
}
void decode_instruction(struct instruction32 * instruction,struct instruction_parts * parts)
{
    int k,j;

    for(j = 0; j <= 5; j++)                            /*  opcode */
        parts->i31_26[j] = instruction->i[j];

    for(j = 0; j < 5; j++)
        parts->i25_21[j] = instruction->i[6+j];        /*  rs. */

    for(j = 0; j < 5; j++)
        parts->i20_16[j] = instruction->i[11+j];       /* rt. */

    for(j = 0; j < 5; j++)                             /* rd */
        parts->i15_11[j] = instruction->i[16+j];

    for(j = 0; j <= 15;j++)                            /*16 bit immidiete field.*/
        parts->i15_0[j] = instruction->i[16+j];
    for(j = 0; j < 6; j++)                             /* 6 bit funct field. */
        parts->i5_0[j] = instruction->i[26+j];

}

int ALU_control(int *funct,int operation)
{
    if(operation == 0)
        return ADD;
    else if(operation == 1)
        return SUB;
    else
    {
        if(binary_to_decimal(funct,6) == 32)
            return ADD;
        else if(binary_to_decimal(funct,6) == 34)
            return SUB;
        else if(binary_to_decimal(funct,6) == 36)
            return AND;
        else if(binary_to_decimal(funct,6) == 35)
            return OR;
        else if(binary_to_decimal(funct,6) == 42)
            return SLT;
    }
}

/*Function that controls the ALU operation  */
int ALU_function(int input1,int input2,int function_code)
{
    switch(function_code)
    {
        case    AND:
                    return and(input1,input2);
        case    OR:
                    return or(input1,input2);
        case    ADD:
                    return (input1 + input2);
        case    SUB:
                    return (input1 - input2);
        case    SLT:
                    return ( (input2 > input1) ?  1 :  0  );
        default:
                printf("Unexpected code.\n");
                exit(-2);
    }
}

/*prints out the register file */
void print_register_file(struct Register * regfile)
{

    int i;

    for(i = 0; i < 32; i++)
        printf("$%d :%d \n",i,regfile->regs[i]);

}

int main(void)
{
    int dataRead,k;
    int Branch,RegDst,MemRead,MemWrite,MemtoReg,ALU_Op,ALU_Src,RegWrite,PC_Src,ALU_op1,ALU_op0;    //Control Signals.
    struct instruction32    instruction;
    struct instruction_parts parts;
    FILE *fd;
    int ReadReg1,ReadReg2,WriteRegister,WriteData_register_file,RegReadData1,RegReadData2,zero,ALU_Result,Address,WriteData_memory,MemReadData;
    char buffer[33];
    buffer[32] = '\0';
    int run_state = 1;
    int rformat,lw,sw,beq;
    int ALU_in1,ALU_in2,ALU_function_code;

    /*We implement the following instruction in this part:
        add $8,$17,$18    $17 = 2 $18 = 3   */

    register_file.regs[17] = 2;
    register_file.regs[18] = 3;

    fd = fopen("instruction_memory.txt","r");

    if(fd == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    while(run_state)
    {

        dataRead = fread(buffer,sizeof(char),32,fd);
        printf("Instruction fetched: ");
        instruction_create(buffer,&instruction);

        for(k = 0; k < 32; k++)
            printf("%d ",instruction.i[k]);

        putchar('\n');
        decode_instruction(&instruction,&parts);

        /*      Register file */

        ReadReg1 = binary_to_decimal(parts.i25_21,5);
        ReadReg2 = binary_to_decimal(parts.i20_16,5);
        WriteRegister = binary_to_decimal(parts.i15_11,5);
        RegReadData1 = register_file.regs[ReadReg1];
        RegReadData2 = register_file.regs[ReadReg2];


        /*  Code for the Control Logic Signals   */

        rformat = and6( not(parts.i31_26[0]), not(parts.i31_26[1]), not(parts.i31_26[2]), not(parts.i31_26[3]), not(parts.i31_26[4]), not(parts.i31_26[5]) ) ;
        RegDst = ALU_op1 = rformat;
        lw = and6( parts.i31_26[0] , not(parts.i31_26[1]), not(parts.i31_26[2]), not(parts.i31_26[3]), parts.i31_26[4], parts.i31_26[5] ) ;
        MemtoReg = MemRead = lw;
        sw = and6( parts.i31_26[0] , not(parts.i31_26[1]), parts.i31_26[2], not(parts.i31_26[3]), parts.i31_26[4], parts.i31_26[5] ) ;
        MemWrite = sw;
        beq = and6( not(parts.i31_26[0]), not(parts.i31_26[1]), not(parts.i31_26[2]), not(parts.i31_26[3]), parts.i31_26[4], not(parts.i31_26[5] ) ) ;
        Branch = ALU_op0 = beq;
        ALU_Src = or(lw,sw);
        RegWrite = or(rformat,lw);

        ALU_Op = 1*ALU_op0 + 2*ALU_op1;

        printf("Control logic signals:\n");
        printf("RegDst ALUSrc MemtoReg RegWrite MemRead MemWrite Branch ALUop1 ALUop0\n");
        printf("  %d       %d       %d       %d        %d         %d      %d      %d      %d  \n",RegDst,ALU_Src,MemtoReg,RegWrite,MemRead,MemWrite,Branch,ALU_op1,ALU_op0);

        WriteRegister = mux2_1( binary_to_decimal(parts.i20_16,5),binary_to_decimal(parts.i15_11,5),RegDst);

        /*ALU   control part    */
        ALU_function_code = ALU_control(parts.i5_0,ALU_Op);



        /*  ALU     */
        ALU_in1 = RegReadData1;
        ALU_in2 = mux2_1(RegReadData2,binary_to_decimal(parts.i15_0,16),ALU_Src);
        ALU_Result = ALU_function(ALU_in1,ALU_in2,ALU_function_code);

        zero = RegReadData1 - RegReadData2;

        PC_Src = and(zero,Branch);







        print_register_file(&register_file);

        printf("Fetch next instruction?(0-exit)\n");

        scanf("%d",&run_state);
    }

    return 0;
}
