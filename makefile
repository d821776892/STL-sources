STL:STL.o
	g++ STL.o -o STL
STL.o:STL.cpp
	g++ -c STL.cpp

.PHONY:STL STL.o

clean:
	rm *.o
	rm STL