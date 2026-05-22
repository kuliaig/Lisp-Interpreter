CC = gcc
CFLAGS = -Wall -Wextra -g
OBJS = main.o object.o parcer.o printer.o hash.o array.o eval.o func.o reader.o

lisp: $(OBJS)
	$(CC) -o lisp $(OBJS)

main.o: main.c object.h parcer.h printer.h hash.h eval.h func.h reader.h library.h
	$(CC) $(CFLAGS) -c main.c

object.o: object.c object.h
	$(CC) $(CFLAGS) -c object.c

parcer.o: parcer.c parcer.h object.h
	$(CC) $(CFLAGS) -c parcer.c

printer.o: printer.c printer.h object.h
	$(CC) $(CFLAGS) -c printer.c

hash.o: hash.c hash.h object.h
	$(CC) $(CFLAGS) -c hash.c

array.o: array.c array.h object.h
	$(CC) $(CFLAGS) -c array.c

eval.o: eval.c eval.h object.h hash.h
	$(CC) $(CFLAGS) -c eval.c

func.o: func.c func.h object.h hash.h
	$(CC) $(CFLAGS) -c func.c

reader.o: reader.c reader.h object.h hash.h
	$(CC) $(CFLAGS) -c reader.c

clean:
	rm -f *.o lisp.exe

vectorlib.dll: vectorlib.c array.c object.c hash.c library.h
	$(CC) -shared vectorlib.c array.c object.c hash.c -o vectorlib.dll -I.