a.out : main.o helper.o
	gcc -o a.out main.o helper.o
main.o : main.c
	gcc -c main.c
helper.o : helper.c
	gcc -c helper.c
clean:
	rm -f a.out main.o helper.o