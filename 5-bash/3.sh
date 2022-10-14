#! /bin/bash
# Написать подробные комментарии
i=1
while [ $i -lt 5 ] #пока i < 5
do
  #cd "Directory/Folder $i" #перейти в diri
  rev Directory/"Folder $i"/"File $i" | tee Directory/"Folder $i"/"File $i" #инвертировать строки i-го файла i-q строки
  i=$(( $i + 1 )) #i++
done

# echo $1
mkdir "Directory" #создать директорию Directory
cd "Directory" #перейти в Directory
for ((i = 1; i <= $1; i++)) #цикл от 1 до $1
do
    mkdir "Folder $i" #создать директорию с именем "Folder i" (i -- значение переменной i)
    for ((j = 1; j <= $2; j++)) #цикл от 1 до $2 (2-го аргумента командной строки)
    do
        touch "Folder $i/File $j" #создать пустой файл в созданной директории
        for ((k  = 1; k <= $3; k++)) #цикл от 1 до $3
        do
            echo You have come to the File number $j of the Folder number $i >> "Folder $i/File $j"	 #дописать текст в конец созданного файла
        done	  
    done
done
