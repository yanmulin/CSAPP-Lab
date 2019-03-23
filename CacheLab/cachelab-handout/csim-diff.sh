#!/bin/bash
#
# 读入-s <num> -E <num> -b <num> 参数，用ref-csim和csim
# 按照该参数读取traces文件夹中的输入文件，分别输出到result.txt
# 和ref-result.txt两个文件，然后用diff进行比较，比较结果输出到stdout

if [[ $# -lt 6 ]]; then
    echo "Missing arguments."
    echo "Usage: csim-diff -s <num> -E <num> -b <num>"
    exit
elif [[ ! -d traces ]]; then
    echo "no directory traces."
    exit
elif [[ ! -f csim-ref ]]; then 
    echo "no ./csim-ref."
    exit
elif [[ ! -f csim ]]; then 
    echo "no ./csim."
    exit
fi 


# file=traces/long.trace
# echo $file >ref-result.txt
# ./csim-ref $* -v -t $file >>ref-result.txt

# echo $file >result.txt
# ./csim $* -v -t $file >>result.txt

for file in ./traces/*
do 
    echo $file
    ./csim-ref $* -v -t $file >ref-result.txt
    ./csim $* -v -t $file >result.txt

    diff result.txt ref-result.txt
done

rm result.txt
rm ref-result.txt
