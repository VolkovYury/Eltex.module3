// Первая программа. Осуществляет чтение из FIFO

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    char *locale = setlocale(LC_ALL, "");

    int fd, proc1, proc2;
    size_t size;
    char testString[14];
    char name[]="prog5-17.fifo";

    pid_t pid, ppid;

    (void)umask(0);

    // Попытка создать FIFO с именем prog5-17.fifo в текущей директории
    if(mknod(name, __S_IFIFO | 0666, 0) < 0) {
        // Если создать FIFO не удалось:

        printf("Can\'t create FIFO\n");
        exit(-1);
    }

    proc1 = fork();

    if (proc1 < 0) {
        // Если создать процесс не удалось, сообщаем об этом и завершаем работу

        printf("Can\'t fork child proc1\n");
        exit(-1);
    } else if (proc1 > 0) {
        // Внутри родительского процесса

        // Создание второго процесса. Следование условию. Второй процесс не будет напрямую родственен первому (только через общего родителя)
        proc2 = fork();

        if (proc2 < 0) {
            // Если создать процесс не удалось, сообщаем об этом и завершаем работу

            printf("Can\'t fork child proc2\n");
            exit(-1);
        } else if (proc2 == 0) {
            // Внутри второго порождённого процесса. В этом процессе будет производиться запись
            
            execl("/home/volkov/Загрузки/progs/5-17/prog5-17-2", "prog5-17-2", NULL);
        }

    } else {
        // Внутри первого порождённого процесса. В этом процессе будет производиться чтение
        fd = open(name, O_RDONLY);

            if (fd < 0) {
                // Если открыть FIFO не удалось, печатаем об этом сообщение и прекращаем работу */

                printf("Can\'t open FIFO for writting\n");
                exit(-1);
            } else {

                size = read(fd, testString, 14);

                if (size < 0) {
                    // Если записалось меньшее количество байт, сообщаем об ошибке и завершаем работу

                    printf("Can\'t read string\n");
                    exit(-1);
                }

                printf("Результат чтения: %s\n", testString);

                pid = getpid();
                ppid = getppid();

                printf("Proc2 pid = %d, ppid = %d\n", (int)pid, (int)ppid); 
            }

            close(fd);
    }

    return 0;
}

