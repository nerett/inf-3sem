# !/bin/bash
# Написать подробные комментарии

if [ $# -le 0 ]; #если не передан диск, выбираем sda по умолчанию; иначе берём из 1-го аргумента командной строки
then
    disk="sda"
else
    disk=$1
fi

String=$(iostat -dhx /dev/$disk 1 2) #получить развёрнутый отчёт о нагрузке на ввыбранный диск 2 раза с интервалом в 1 секунду, последний записать в $String
echo ${String: -30} #вывести последние 30 символов из $String (т.е. только последнюю строку с данными о нагрузке)
