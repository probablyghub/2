#!/bin/sh
set -eu

root=${1:-result}

if [ "$#" -gt 1 ]; then
    echo "Использование: $0 [каталог]" >&2
    exit 1
fi

mkdir "$root"
mkdir "$root/a"
mkdir "$root/a/b" "$root/a/c" "$root/a/e"
ln -s c "$root/a/d"
printf 'catts\n' > "$root/a/b/cat.txt"
: > "$root/a/c/dog.txt"
ln "$root/a/b/cat.txt" "$root/a/e/f.txt"
head -c 500 /dev/urandom > "$root/a/e/g.bin"
truncate -s 35 "$root/a/e/h.bin"

printf 'Каталог создан: %s\n' "$root"
