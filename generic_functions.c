int char_to_int(char c)
{
    if(c == '0')
        return 0;
    else
        return 1;
}
int binary_to_decimal(int *matrix,int bit_len)  //  converting the binary signals into decimals for our program use.Bit_len is the lenght of the array.
{
    int i,j=0;

    int value = 0;
    int k;
    for(i = bit_len-1; i >= 0; i--)
    {
        k = pow(2.0,(float)i);
        value += matrix[j++] * k ;
    }

    return value;
}
