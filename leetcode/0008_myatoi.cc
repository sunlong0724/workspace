/*************************************************************************
*File Name: 0008_myatoi.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月24日 星期五 22时13分04秒
 ************************************************************************/
#include <libgen.h>
#include <string>
#define INT_MAX 2147483647
#define INT_MIN 2147483648
int myatoi(std::string str){
    //去除前导空格
    //判断第一个字符是否时符号位
    //遍历每个字符是否在‘0’～‘9’之间
    //在的话计算结果,判断是否溢出
    //不在的返回之前计算的结果
    
    int i = 0, len = str.size(), ret = 0;
    bool f = true;
    if (len == 0) return 0;
    while( str[i] == ' ' ) ++i;
    if (str[i] == '-'){
        f = false;
        ++i;
    }else if (str[i] == '+'){
        ++i;
    }

    while( i < len ){
        if (str[i] < '0' || str[i] > '9') break;

        if (ret > INT_MAX/10 || ret == INT_MAX/10 && str[i] - '0' > INT_MAX-ret*10) return f ? INT_MAX:INT_MIN;
        
        ret = ret * 10 + (str[i] - '0');
        ++i;
    }

    return f ? ret : -ret;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stdout,"%s int_str\n", basename( argv[0] ));
        return 1;
    }

    fprintf(stdout,"%d\n", myatoi(argv[1]));
    return 0;
}

