#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/dir.h>

void printfname(int cnt, char *name, struct stat * const statbuf)
{
    for (int i = 0; i < (cnt - 2) / 2; ++i)
        printf("| ");
    if (cnt != 0)
        printf("└─");
    printf("%s [ino=%lu]\n", name, statbuf->st_ino);
}

static int walk(char *nextf, int offset)
{
    int ret = 0;
    struct stat statbuf;
    DIR *dp;
    struct dirent *dirp;

    if (lstat(nextf, &statbuf) == -1)
    {
        printf("Ошибка вызова lstat: ");
        switch (errno)
        {
            case EACCES:
                printf("нет доступа к директории\n");
                break;

            case EIO:
                printf("ошибка чтения из файловой системы\n");
                break;

            case ELOOP:
                printf("произошло зацикливание символьной ссылки\n");
                break;

            case ENAMETOOLONG:
                printf("слишком длинное имя\n");
                break;

            case ENOTDIR:
                printf("не директория\n");
                break;

            case ENOENT:
                printf("директории не существует или передана пустая строка\n");
                break;

            case EOVERFLOW:
                printf("переполнение буфера\n");
                break;
        }

        return errno;
    }

    printfname(offset, nextf, &statbuf);
    if (S_ISDIR(statbuf.st_mode) == 0)
    {
        return 0; // Не директория
    }

    /* Условия выхода из рекурсии
     * - возврат ненулевого значения (ошибка вызова lstat или opendir)
     * - достижение конца каталога
     */
    if (chdir(nextf) != 0)
    {
    	printf("Нет доступа к директории %s\n", nextf);
    	return 0;
    }

    if ((dp = opendir(".")) == 0)
    {
        printf("Ошибка при открытии директории %s\n", nextf);
        return -1;
    }

    while ((dirp = readdir(dp)) != 0 && ret == 0)
        if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
            ret = walk(dirp->d_name, offset + 2); // Рекурсия
    chdir("..");

    return ret;
}

int ftw(char *path)
{
    return walk(path, 0);
}

int main(int argc, char *argv[]) {
    int ret;
    if (argc != 2) {
        printf("Использование: main.out <начальный каталог>\n");
        return 1;
    }
    ret = ftw(argv[1]);
    return ret;
}
