#!/bin/bash

echo "There are 3 levels in 'asm_test.'"
read -p "Choose one [1/2/3]: " level

if [[ $level =~ ^[0-9]+$ ]]; then
    if [ $level -le 0 ] || [ $level -gt 3 ]; then
        level=1
    fi
elif [[ $level == "q" ]] || [[ $level == "Q" ]] || [[ $level == "quit" ]] || [[ $level == "Quit" ]]; then
    echo "quit"
    exit
else
    level=1
fi

src_path="test/asm_test/"
src_path=$src_path$level

if ! [ -f "build/main" ]; then
    make run
    if [ $? -ne 0 ]; then
        exit
    fi
fi

num=0
count=`find $src_path -type f -name "*.c" | wc -l`
echo -e "\nThere are $count files in level_$level"

if ! [[ $num =~ ^[0-9]+$ ]]; then
    if [[ $num == "q" ]]; then
        echo "quit"
        exit
    fi
    num=1
fi

while [ $num -eq 0 ]; do
    read -p "Choose one from level_$level [1...$count]: " num
    if ! [[ $num =~ ^[0-9]+$ ]]; then
        if [[ $num == "q" ]] || [[ $num == "Q" ]] || [[ $num == "quit" ]] || [[ $num == "Quit" ]]; then
            echo "quit"
            exit
        fi
        num=1
    elif [ $num -eq 0 ]; then
        ls -l test/asm_test/$level | awk '{print $9}'
    elif [ $num -lt 0 ] || [ $num -gt $count ]; then
        num=1
    fi
done

# find $src_path -type f -name "*.c" | sort | head -n $num | tail -n 1
file=$(find $src_path -type f -name "*.c" | sort | head -n $num | tail -n 1)
echo -e "\nReady to compile '$file'\n"

if ! [ -f "build/main" ]; then
    make run
    if [ $? -ne 0 ]; then
        exit
    fi
fi

echo "src to asm ..."

./build/main $file > "out/lv"$level"_"$num".asm"
if [ $? -ne 0 ]; then
    exit
fi

echo "asm to obj ..."

as -32 "out/lv"$level"_"$num".asm" -o out/result.o
if [ $? -ne 0 ]; then
    echo "failed!"
    exit
fi

echo "obj to final ..."
gcc -m32 out/result.o -o out/result_lv$level_$num
if [ $? -ne 0 ]; then
    echo "failed!"
    exit
fi

echo -e "\n-------------------Result-------------------"
./out/result_lv$level_$num
