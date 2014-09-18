all: lenschiptool lenschiptool-cli


GP_LIBS   = `pkg-config --libs   libgphoto2`
GP_CFLAGS = `pkg-config --cflags libgphoto2`
CFLAGS    =  -Wall $(GP_CFLAGS)
LFLAGS    = -Wall $(GP_LIBS)

tmp.o: tmp.c helper.h
	gcc -c tmp.c $(CFLAGS) -o tmp.o
tmp: tmp.o helper.o
	gcc tmp.o helper.o $(LFLAGS) -o tmp


test.o: test.c helper.h
	gcc -c test.c $(CFLAGS) -o test.o
test: test.o helper.o
	gcc test.o helper.o $(CFLAGS) -o lcttest


main.o: main.c helper.h
	gcc -c main.c $(CFLAGS) -o main.o
lenschiptool: main.o helper.o chips.o
	gcc main.o helper.o chips.o $(LFLAGS) -o lct


main-cli.o: main-cli.c helper.h
	gcc -c main-cli.c $(CFLAGS) -o main-cli.o
lenschiptool-cli: main-cli.o helper.o chips.o
	gcc main-cli.o helper.o chips.o $(LFLAGS) -o clct




helper.o: helper.c helper.h
	gcc -c helper.c $(CFLAGS) -o helper.o


chips.o: chips.c chips.h helper.h
	gcc -c chips.c $(CFLAGS) -o chips.o






clean:
	rm -f *.o lenschiptool lenschiptool-cli clct lct lcttest tmp
