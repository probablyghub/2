#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    const char *root = argc > 1 ? argv[1] : "result";
    char cwd[4096];

    if (argc > 2) {
        fprintf(stderr, "Использование: %s [каталог]\n", argv[0]);
        return 1;
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return 1;
    }

    if (chdir(root) == -1) {
        perror(root);
        return 1;
    }

    if (unlink("a/b/cat.txt") == -1 ||
        unlink("a/c/dog.txt") == -1 ||
        unlink("a/d") == -1 ||
        unlink("a/e/f.txt") == -1 ||
        unlink("a/e/g.bin") == -1 ||
        unlink("a/e/h.bin") == -1) {
        perror("unlink");
        return 1;
    }

    if (rmdir("a/b") == -1 || rmdir("a/c") == -1 ||
        rmdir("a/e") == -1 || rmdir("a") == -1) {
        perror("rmdir");
        return 1;
    }

    if (chdir(cwd) == -1 || rmdir(root) == -1) {
        perror("rmdir");
        return 1;
    }

    printf("Каталог удален: %s\n", root);
    return 0;
}
