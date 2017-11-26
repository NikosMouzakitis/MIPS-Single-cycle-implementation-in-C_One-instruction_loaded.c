/*  Implementing all logical functions needed
    to implement the Data_path and the control
    that controls the flow of execution.    */

int and6(int a1,int a2,int a3,int a4,int a5,int a6)
{

    if( (a1 == 1) && (a2 == 1) && (a3 == 1) && (a4 == 1) && (a5 == 1) && (a6 == 1) )
        return 1;
    return 0;
}

int and(int x,int y)
{
    if((x==1)&&(y==1))
        return 1;
    else
        return 0;
}

int or(int x,int y)
{
    if( (x==0) && (y == 0))
        return 0;
    else
        return 1;
}

int not(int x)
{
    if(x==0)
        return 1;
    return 0;
}

int nand(int x,int y)
{
    if( (x == 1) && (y==1))
        return 0;
    else
        return 1;
}

int mux2_1(int x,int y,int signal)  //Multiplexer 2 to 1.
{
    if(signal)
        return y;
    return x;
}

int nor(int x,int y)
{
    if( (x==0) && (y==0))
        return 1;
    else
        return 0;
}
