#include <stdio.h>

// 去掉开头和结尾的空格、\n、\t
// 中间的空格保留
// 全是空白时返回空串
// 原地修改
char *trim(char *s) {
    if (*s == null || *s == " ") {
        return s;
    }

    char* end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\n' || *end == '\t')) {
        end--;
    }
    *(end + 1) = '\0';
    while (s < end && (*s == ' ' || *s == '\n' || *s == '\t')) {
        s++;
    }
    return s;
}

int main() {
   // char *s = "    \n\thello world      \0";
   char s[] = "                     ";
   char *res = trim(s);

}