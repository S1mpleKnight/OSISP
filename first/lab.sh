#!/bin/bash
#  Вариант 9
#  Написать скрипт, находящий все каталоги и подкаталоги начиная с заданного каталога и ниже на заданной глубине
#  вложенности (аргументы 1 и 2 командной строки). Скрипт выводит результаты в файл (третий аргумент командной строки)
#  в виде полный путь, количество файлов в каталоге. На консоль выводиться общее число просмотренных каталогов

touch /tmp/err.txt
if [ $# -ne 3 ];
then
echo "error: invalid number of argumets" > /tmp/err.txt
else
	touch $3
        directories=0
        for dir in $(find $1 -maxdepth $2 -mindepth $2 -type d 2>>/tmp/err.txt);
                do
                directories=$(($directories + 1))
                count=$(find ${dir} -maxdepth 1 -type f 2>>/tmp/err.txt | wc -l)
                printf "%s %d\n" ${dir} $count
                done > $3
        echo $directories
fi

IFS=$(printf '\n\t')

for error in $(cat /tmp/err.txt);
	do
	echo $(basename $0): ${error} >&2
	done

rm /tmp/err.txt
