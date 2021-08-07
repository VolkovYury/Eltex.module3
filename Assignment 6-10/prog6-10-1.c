// Первая программа должна создавать сегмент разделяемой памяти и копировать туда свой собственный исходный текст

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <errno.h>


int main() {
    char *locale = setlocale(LC_ALL, "");

    char pathname[] = "test.txt";   // данный файл должен существовать в дирректории
    char testText[] = "This text is transferred from one program to another via shared memory!\0";

    char *str;      // указатель на массив символов в области разделяемой памяти
    int shmID; 

    key_t key;
    key = ftok(pathname, 0);
    if(key < 0){
        printf("Не получается сгенерировать ключ\n");
        exit(-1);
    }

    // Создание области разделяемой памяти
    shmID = shmget(key, sizeof(testText), 0666|IPC_CREAT|IPC_EXCL);
    if (shmID < 0) {
        
        if (errno != EEXIST) {
            printf("Не получилось создать разделяемую память\n");
            exit(-1);
        } else {
            
            shmID = shmget(key, sizeof(testText), 0);
            if (shmID < 0) {
                printf("Не получилось найти разделяемую память\n");
                exit(-1);
            }
        }

    }

    // Включение области разделяемой памяти в адресное пространство текущего процесса
    str = (char *) shmat (shmID, NULL, 0);
    if (str == (char*)(-1)) {
        printf("Не получилось включить разделяемую память в адресное пространство текущего процесса\n");
        exit(-1);       
    }

    str = testText;

    printf("Удалось успешно записать тестовую строку в область разделяемой памяти!\n");
    printf("Записано содержимое: %s\n", str);

    return 0;
}
