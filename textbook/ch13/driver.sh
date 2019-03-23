if [ ! -f data.txt ]; then
	touch data.txt
	for i in {1..10} 
	do	
		echo Hello world >>data.txt
	done
fi

port=$1
#./echoserver $port >/dev/null &

for i in {1..10}
do
	echo $i
	(
		./echoclient localhost $port <data.txt
	) &
done

#pid=$(ps -ef | grep echoserver | head -1 | cut -d" " -f4)

wait
#echo $pid
#kill $pid 
