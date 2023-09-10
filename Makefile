all: project1
CPP=g++
CFLAGS=-std=c++11

project1: main.o SocialNetwork.o User.o
	$(CPP) -o project1 main.o SocialNetwork.o User.o

main.o: main.cpp SocialNetwork.h
	$(CPP) -c main.cpp

User.o: User.cpp User.h
	$(CPP) -c User.cpp

SocialNetwork.o: SocialNetwork.cpp SocialNetwork.h User.h
	$(CPP) -c SocialNetwork.cpp

clean:
	rm -f *.o *~ project1
