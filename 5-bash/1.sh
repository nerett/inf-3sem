#!/bin/bash

KISA=Киса #создать переменную KISA и присвоить ей значение Киса

echo $KISA #вывод значения переменной KISA
echo $RANDOM #вывод значения переменной RANDOM

mkdir test #создать директорию test
cd test #перетйти в директорию test

n=0 #создать переменную n и присвоить ей значение 0

for ((i = 1; i <= $1; i++)) #цикл от 1 до значения переменной 1 (первая опция командной строки)
do
   mkdir level1.$i #создать директорию с именем level.i, где i = номер итерации
	for ((j = 1; j <= $2; j++)) #аналогичный цикл до переменной 2
	do
		 mkdir level1.$i/level2.$j #создать поддиректорию в текущей с аналогичным именованием
		
		n=$((n+1)) #увеличение n на единицу
					
		touch level1.$i/level2.$j/TextFile$n #создать файл с именем TextFilen в текущей поддиректории, где n - значение переменной n
		echo "This is file $n" > level1.$i/level2.$j/TextFile$n #перезапись файла
		echo $n #вывести значение переменной n
	done
done

#exit 0
pwd

for f in `find . -type f` #перебор всех файлов в текущей директориии
do
  rev $f | tee $f #> $f #переворот всех строк в файле и перезапись самого файла
done