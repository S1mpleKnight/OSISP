# OSISP/ОСиСП/Операционные системы и структурное программирование

### Лабораторные работы ПОИТ/БГУИР

##### Первая

Написать скрипт, находящий все каталоги и подкаталоги начиная с заданного каталога и ниже на заданной глубине
вложенности (аргументы 1 и 2 командной строки). Скрипт выводит результаты в файл (третий аргумент командной строки)
в виде полный путь, количество файлов в каталоге. На консоль выводиться общее число просмотренных каталогов

##### Вторая

Подсчитать для заданного каталога (первый аргумент командной строки) и всех его подкаталогов (по-отдельности) 
суммарный размер занимаемого файлами на диске пространства в байтах и суммарный размер файлов. 
Вычислить коэффициент использования дискового пространства в %. Для получения размера занимаемого файлами 
на диске пространства использовать команду stat.

##### Третья

Написать программу синхронизации двух каталогов, например, Dir1 и Dir2. Пользователь задаёт имена Dir1 и Dir2.
В результате работы программы файлы, имеющиеся в Dir1, но отсутствующие в Dir2, должны скопироваться в Dir2 вместе с правами доступа.
Процедуры копирования должны запускаться в отдельном процессе для каждого копируемого файла. Каждый процесс выводит на экран свой pid,
полный путь к копируемому файлу и число скопированных байт. Число одновременно работающих процессов не должно превышать N (вводится пользователем).
Скопировать несколько файлов из каталога /etc в свой домашний каталог. Проверить работу программы для каталога /etc и домашнего каталога.

##### Четвёртая 

Создать дерево процессов согласно варианта индивидуального задания.
Процессы непрерывно обмениваются сигналами согласно табл. 2. Запись в таблице 1 вида: 1->(2,3,4,5) означает, что исходный процесс 0 создаёт дочерний процесс 1, который,
в свою очередь, создаёт дочерние процессы 2,3,4,5. Запись в таблице 2 вида: 1->(2,3,4) SIGUSR1 означает, что процесс 1 посылает дочерним процессам 2,3,4 одновременно
(т.е. за один вызов kill() ) сигнал SIGUSR1.
Каждый процесс при получении или посылке сигнала выводит на консоль информацию в следующем виде:
N pid ppid послал/получил USR1/USR2 текущее время (мксек),
где N-номер сына по табл. 1
Процесс 1, после получения 101 –го по счету сигнала SIGUSR, посылает сыновьям сигнал SIGTERM и ожидает завершения всех сыновей, после чего завершается сам. Процесс 0 ожидает завершения работы процесса 1 после чего завершается сам. Сыновья, получив сигнал SIGTERM, завершают работу с выводом на консоль сообщения вида:
pid ppid завершил работу после X-го сигнала SIGUSR1 и Y-го сигнала SIGUSR2
где X, Y – количество посланных за все время работы данным сыном сигналов SIGUSR1 и SIGUSR2

###### Дерево: 1->2 2->(3,4,5) 4->6 3->7 5->8
###### Сигналы: 1->(2,3) SIGUSR1 3->4 SIGUSR2 4->(5,6,7) SIGUSR1 7->8 SIGUSR1 8->1 SIGUSR2
