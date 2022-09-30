#!/bin/bash
# Написать подробные комментарии
# echo   "Кису любишь!"

KISA=Киса

echo $KISA

echo $RANDOM

mkdir test
cd  test

n=0

for i in `seq 1 $1` #цикл от 1 до значения параматра командной строки $1
do
    echo "i = $i"
   mkdir level1.$i
	for ((j = 1; j <= $2; j++))
	do
		 mkdir level1.$i/level2.$j
		
		n=$((n+1))
					
		touch level1.$i/level2.$j/TextFile$n
		echo "This is file $n" > level1.$i/level2.$j/TextFile$n
		echo $n
	done
done

for f in `find . -type f`
do
  rev $f | tee $f
done
