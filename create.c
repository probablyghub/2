#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int write_all(int fd, const void *data, size_t size)
{
    const char *ptr = data;

    while (size > 0) {
        ssize_t written = write(fd, ptr, size);
        if (written < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (written == 0)
            return -1;
        ptr += written;
        size -= (size_t)written;
    }

    return 0;
}

static int create_file(const char *path, const char *content, size_t size)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);
    if (fd == -1) {
        perror(path);
        return -1;
    }

    if (size > 0 && write_all(fd, content, size) == -1) {
        perror(path);
        close(fd);
        return -1;
    }

    if (close(fd) == -1) {
        perror(path);
        return -1;
    }

    return 0;
}

static int create_random_file(const char *path, size_t size)
{
    int random_fd = open("/dev/urandom", O_RDONLY);
    if (random_fd == -1) {
        perror("/dev/urandom");
        return -1;
    }

    int file_fd = open(path, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);
    if (file_fd == -1) {
        perror(path);
        close(random_fd);
        return -1;
    }

    char buffer[128];
    size_t left = size;

    while (left > 0) {
        size_t count = left < sizeof(buffer) ? left : sizeof(buffer);
        ssize_t received = read(random_fd, buffer, count);

        if (received < 0) {
            if (errno == EINTR)
                continue;
            perror("read");
            close(random_fd);
            close(file_fd);
            return -1;
        }
        if (received == 0) {
            fprintf(stderr, "Не удалось прочитать случайные данные\n");
            close(random_fd);
            close(file_fd);
            return -1;
        }

        if (write_all(file_fd, buffer, (size_t)received) == -1) {
            perror("write");
            close(random_fd);
            close(file_fd);
            return -1;
        }

        left -= (size_t)received;
    }

    if (close(random_fd) == -1) {
        perror("close");
        close(file_fd);
        return -1;
    }
    if (close(file_fd) == -1) {
        perror(path);
        return -1;
    }

    return 0;
}

static int create_zero_file(const char *path, off_t size)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);
    if (fd == -1) {
        perror(path);
        return -1;
    }

    if (ftruncate(fd, size) == -1) {
        perror("ftruncate");
        close(fd);
        return -1;
    }

    if (close(fd) == -1) {
        perror(path);
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    const char *root = argc > 1 ? argv[1] : "result";

    if (argc > 2) {
        fprintf(stderr, "Использование: %s [каталог]\n", argv[0]);
        return 1;
    }

    if (mkdir(root, S_IRWXU) == -1) {
        perror(root);
        return 1;
    }
    if (chdir(root) == -1) {
        perror(root);
        return 1;
    }

    if (mkdir("a", S_IRWXU) == -1 || chdir("a") == -1 ||
        mkdir("b", S_IRWXU) == -1 || mkdir("c", S_IRWXU) == -1 ||
        mkdir("e", S_IRWXU) == -1) {
        perror("mkdir/chdir");
        return 1;
    }

    if (symlink("c", "d") == -1) {
        perror("d");
        return 1;
    }

    if (chdir("b") == -1 || create_file("cat.txt", "catts\n", 6) == -1) {
        perror("b/cat.txt");
        return 1;
    }

    if (chdir("../c") == -1 || create_file("dog.txt", "", 0) == -1) {
        perror("c/dog.txt");
        return 1;
    }

    if (chdir("../e") == -1 || link("../b/cat.txt", "f.txt") == -1) {
        perror("e/f.txt");
        return 1;
    }

    if (create_random_file("g.bin", 500) == -1 ||
        create_zero_file("h.bin", 35) == -1) {
        return 1;
    }

    printf("Каталог создан: %s\n", root);
    return 0;
}
