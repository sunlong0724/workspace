#include <stdio.h>
void byteorder(){
    union{
        short value;
        char union_byte[ sizeof ( short ) ];
    }test;
    test.value = 0x0102;
    if ( ( test.union_byte[0] == 0x01 ) && test.union_byte[1] == 0x02) {
        fprintf(stdout,"big endian\n");
    }
    else{
        fprintf(stdout,"little endian\n");
    }
}

    int main(int argc, char** argv){
        byteorder();
        return 0;
    }
