// Программа 1

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <errno.h>


int main() {
    char *locale = setlocale(LC_ALL, "");
    char pathname[] = "test.txt";   // Имя файла, использующееся для генерации ключа. Файл с таким именем должен существовать в текущей директории
    key_t key;                      // IPC ключ

    int shmid;                      // IPC дескриптор для области разделяемой памяти
    int semid;                      // IPC дескриптор для массива IPC семафоров

    int *array;                     // Указатель на разделяемую память
    struct sembuf operationA;       // Структура для задания операции A(semid,1)
    operationA.sem_num = 0;
    operationA.sem_op = 1;
    operationA.sem_flg = 0;

    struct sembuf operationD;       // Структура для задания операции D(semid,1)
    operationD.sem_num = 0;
    operationD.sem_op = -1;
    operationD.sem_flg = 0;
    
    int new = 1;                    // Флаг необходимости инициализации элементов массива

    // Генерируем IPC ключ из имени файла test.txt в текущей директории и номера экземпляра области разделяемой памяти 0
    if((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    // Получение доступа к семафору или его создание 
    if((semid = semget(key, 1, 0666|IPC_CREAT|IPC_EXCL)) < 0) {
        // В случае возникновения ошибки пытаемся определить: возникла ли она из-за того, что семафор уже существует, или по другой причине

        if(errno != EEXIST){
        // Если по другой причине - прекращаем работу

            printf("Can\'t create semaphore\n");
            exit(-1);
        } else {
        // Если из-за того, что семафор уже существует, пытаемся получить его IPC дескриптор

            if((semid = semget(key, 0, 0)) < 0) {
                printf("Can\'t find semaphore\n");
                exit(-1);
            }
        }
    } else {
        semctl(semid, 0, SETVAL, 1);
    }

    // Попытка создать разделяемую память для сгенерированного ключа
    if((shmid = shmget(key, 3*sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
    // В случае возникновения ошибки пытаемся определить: возникла ли она из-за того, что сегмент разделяемой памяти уже существует, или по другой причине

        if(errno != EEXIST){
        // Если по другой причине - прекращаем работу

            printf("Can\'t create shared memory\n");
            exit(-1);
        } else {
        // Если из-за того, что разделяемая память уже существует, пытаемся получить ее IPC дескриптор и, в случае удачи, 
        //сбрасываем флаг необходимости инициализации элементов массива

            if((shmid = shmget(key, 3*sizeof(int), 0)) < 0) {
                printf("Can\'t find shared memory\n");
                exit(-1);
            }

            new = 0;
        }
    }

    // Попытка отобразить разделяемую память в адресное пространство текущего процесса. 

    if((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
        printf("Can't attach shared memory\n");
        exit(-1);
    }

    // -------Критический участок
    // В зависимости от значения флага new либо инициализируем массив, либо увеличиваем соответствующие счетчики

    // На данный момент значение семафора - 1. После выполнения if - будет равно нулю, что заблокирует доступ для остальных процессов
    if(semop(semid, &operationD, 1) < 0) {
        printf("Не удалось выполнить операцию D(semid, 1)\n");
        exit(-1);
    }

    if(new){
        array[0] = 1;
        array[1] = 0;
        array[2] = 1;
    } else {
        array[0] += 1;
        // Создание задержки
        for(long i=0; i<1000000000L; i++);
        for(long i=0; i<1000000000L; i++);
        for(long i=0; i<1000000000L; i++);
        array[2] += 1;
    }

    // На данный момент значение семафора равно нулю. После выполнения следующего оператора будет равно 1. Что разблокирует доступ для остальных процессов
    if(semop(semid, &operationA, 1) < 0) {
        printf("Не удалось выполнить операцию A(semid, 1)\n");
        exit(-1);
    }
    // -------Выход из критического участка

    // Печатаем новые значения счетчиков, удаляем разделяемую память из адресного пространства текущего процесса и завершаем работу
    printf("Program 1 was spawn %d times, program 2 - %d times, total - %d times\n", array[0], array[1], array[2]);

    if(shmdt(array) < 0){
        printf("Can't detach shared memory\n");
        exit(-1);
    }

    return 0;
}
