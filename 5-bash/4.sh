# Написать подробные комментарии ? почему нет подсветки? если открыть файл в GEdit
#у меня нет gedit, а расширения это миф

#!/bin/bash

#последовательное создание директорий (по 4 директории f1-f4 с уровнем вложенности 3) и текстовых файлов с аналогичными названиями и содержимым "fx.fx", где fx = f1/f2/f3/f4

for folder1 in "f1" "f2" "f3" "f4"
do
	mkdir $folder1
	cd $folder1
	for folder2 in "f1" "f2" "f3" "f4"
	do
		mkdir $folder2
		cd $folder2
		for folder3 in "f1" "f2" "f3" "f4"
		do
			mkdir $folder3
			cd $folder3
			for file in "f1" "f2" "f3" "f4"
			do
				echo $file.$file > $file.$file
			done
			cd .. #переход в родительскую директорию
		done
		cd ..
	done
	cd ..
done
