// Вторая программа. Данная программа ответственна за занесение данных в pipe()

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
 
int main(int argc, char* argv[])
{
    char *locale = setlocale(LC_ALL, "");

    int fd[2];
    ssize_t size;
    char str[14]={"Program test!"};
 
    // Аргументы приходят в виде const char*. Необходимо преобразовать в число. Заведомо известно, что там кроме чисел ничего быть не может
    fd[0] = atoi(argv[0]);
    fd[1] = atoi(argv[1]);
 
    // Закрытие входного потока данных
    close(fd[0]);
    size=write(fd[1], str, 14);
    if (size!=14)
    {
        printf("Строка не может быть прочитана полностью\n");
        exit(-1);
    }
    
    // Закрытие выходного потока данных
    close(fd[1]);
    return 0;
}
