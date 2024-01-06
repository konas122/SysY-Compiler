#!/bin/bash

echo "There are 3 levels in 'asm_test.'"
read -p "Choose one [1/2/3]: " level


if ! [[ $level =~ ^[0-9]+$ ]]; then
    if [[ $level == "q" ]] || [[ $level == "Q" ]] || [[ $level == "quit" ]] || [[ $level == "Quit" ]]; then
        echo "quit"
        exit
    fi
    level=1
elif [ $level -le 0 ] || [ $level -gt 3 ]; then
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

count=0

for file in "$src_path"/*.c; do
    if [ -f "$file" ]; then
        echo "compile file level$level/$count ..."
        ./build/main $file > "out/batch_lv"$level"_"$count".asm"

        if [ $? -ne 0 ]; then
            rm -f "out/batch_lv"$level"_"$count".asm"
        fi

        ((count++))
    else
        echo $file" is not a 'c' file!"
        continue
    fi
done
