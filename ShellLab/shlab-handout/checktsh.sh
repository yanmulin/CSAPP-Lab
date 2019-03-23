#!/bin/bash

if [ -f result.txt ]; then
    rm result.txt
fi

if [ -f result-ref.txt ]; then
    rm result-ref.txt
fi

touch result.txt
touch result-ref.txt

if [ ! -z $1 ]; then 
    if [ $1 -lt 10 ]; then
        filename=trace0$1.txt
    else 
        filename=trace$1.txt
    fi
    echo $filename
    echo $filename >> result.txt
    echo $filename >> result-ref.txt
    ./sdriver.pl -t $filename -s ./tshref -a "-p" >> result-ref.txt
    ./sdriver.pl -t $filename -s ./tsh -a "-p" >> result.txt 
    echo >> result.txt
    echo >> result-ref.txt
    echo >> result.txt
    echo >> result-ref.txt
else 
    for i in {1..16} 
    do 
        if [ $i -lt 10 ]; then
            filename=trace0$i.txt
        else
            filename=trace$i.txt
        fi
        echo $filename
        echo $filename >> result.txt
        echo $filename >> result-ref.txt
        ./sdriver.pl -t $filename -s ./tshref -a "-p" >> result-ref.txt
        ./sdriver.pl -t $filename -s ./tsh -a "-p" >> result.txt
        echo >> result.txt
        echo >> result-ref.txt
        echo >> result.txt
        echo >> result-ref.txt
    done
fi

diff result.txt result-ref.txt
