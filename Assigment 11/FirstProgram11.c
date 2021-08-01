// Первая программа. Её необходимо запускать.
// Данная программа выводит в консоль информацию, которая была занесена в pipe() второй программой.

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
 
int main( int argc, char *argv[]) {
    char *locale = setlocale(LC_ALL, "");

    int fd[2], newProc;
    size_t size;
    char str[14];

    // для передачи значения дескриптора в другую программу необходимо int преобразовывать в char
    char fd0[11], fd1[11];

    if(pipe(fd) < 0) {
        printf("Не получилось создать pipe\n");
        exit(-1);
    }

    newProc = fork();

    if (newProc < 0) {
        printf("Не получилось создать процесс-ребёнка\n");
        exit(-1);

    } else if (newProc > 0) {
        printf ("Внутри родительского процесса\n");
        printf ("Внутри родительского процесса. Закрытие выходного потока данных\n");
        close(fd[1]);
        printf ("Внутри родительского процесса. Чтение строки\n");
        read(fd[0], str ,140);
        printf ("Внутри родительского процесса. Закрытие входного потока данных\n");
        close(fd[0]);
        printf ("Внутри родительского процесса. Результат:\n");
        printf("%s\n", str);

    } else {
        printf ("Внутри порождённого процесса\n");
        printf ("Внутри порождённого процесса. Запись первого аргумента в массив символов\n");
        sprintf(fd0, "%d", fd[0]);
        printf ("Внутри порождённого процесса. Запись второго аргумента в массив символов\n");
        sprintf(fd1, "%d", fd[1]);
        printf ("Внутри порождённого процесса. Функция execl\n");
        execl("./SecondProgram11", fd0, fd1, NULL);
    }

    return 0;
}
