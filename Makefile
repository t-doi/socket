CC=g++
INCLUDE= -I/usr/include
all:
	$(CC)  -ggdb  test_c.cpp mysock.cpp -o client.exe
	$(CC)  -ggdb  test_s.cpp mysock.cpp -o server.exe
#	$(CC)  -ggdb  test_s.cpp mysock.cpp -o server.exe -lpthread
#	$(CC)  -ggdb  test_s.cpp mysock.cpp -o server.exe -lcygipc
