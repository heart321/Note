#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // 输入的 IP 地址字符串
    char ip[] = "255.255.255.255";
    
    // 存储转换后的整数数组
    int ip_array[4];
    
    // 用 "." 分割 IP 字符串
    char *token = strtok(ip, ".");
    
    // 遍历字符串，提取每个部分并转换为整数
    int i = 0;
    while (token != NULL && i < 4) {
        ip_array[i] = atoi(token); // 将每个部分转换为整数
        token = strtok(NULL, "."); // 获取下一个部分
        i++;
    }
    
    // 输出整数数组
    printf("整数数组：\n");
    for (int j = 0; j < 4; j++) {
        printf("%d ", ip_array[j]);
    }
    printf("\n");
    
    return 0;
}
