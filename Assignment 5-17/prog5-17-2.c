// Вторая программа. Осуществляет запись в FIFO

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    char *locale = setlocale(LC_ALL, "");
    
    int fd;
    ssize_t size;
    char testString[14];
    char name[]="prog5-17.fifo";

    pid_t pid, ppid;

    fd = open(name, O_WRONLY);

    if (fd < 0) {
        // Если открыть FIFO не удалось, печатаем об этом сообщение и прекращаем работу */

        printf("Can\'t open FIFO for writting\n");
        exit(-1);
    } else {

        size = write(fd, "Program test!", 14);

        if (size < 14) {
                // Если записалось меньшее количество байт, сообщаем об ошибке и завершаем работу

            printf("Can\'t write all string to FIFO\n");
            exit(-1);
        }

        pid = getpid();
        ppid = getppid();

        printf("Запись произведена успешно!\n");
        printf("Proc1 pid = %d, ppid = %d\n", (int)pid, (int)ppid); 

        }

        close(fd);

    return 0;
}