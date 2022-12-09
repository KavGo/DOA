findMedian: findMedian.o 234tree.o
	gcc -Wall -o findMedian findMedian.o 234tree.o -g

findMedian.o: 234tree.h findMedian.c
	gcc -Wall -c findMedian.c -g

234tree.o: 234tree.h 234tree.c
	gcc -Wall -c 234tree.c -g
