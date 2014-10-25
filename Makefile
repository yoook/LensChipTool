all: lenschiptool

#export LD_LIBRARY_PATH=/usr/local/lib
#export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig

GP_LIBS   = `pkg-config --libs   libgphoto2`
GP_CFLAGS = `pkg-config --cflags libgphoto2`
#GP_LIBS   = -L/home/ich/bin/lib -lgphoto2 -lgphoto2_port -lm -lexif
#GP_CFLAGS = -I/home/ich/bin/include/gphoto2 -I/usr/include/libexif

CFLAGS    =  -Wall -g $(GP_CFLAGS)
LFLAGS    = -Wall -g $(GP_LIBS)



main.o: main.c helper.h chips.h checks.h
	gcc -c main.c $(CFLAGS) -o main.o
lenschiptool: main.o helper.o chips.o checks.o
	gcc main.o helper.o chips.o checks.o $(LFLAGS) -o lenschiptool


helper.o: helper.c helper.h
	gcc -c helper.c $(CFLAGS) -o helper.o

chips.o: chips.c chips.h helper.h
	gcc -c chips.c $(CFLAGS) -o chips.o

checks.o: checks.c checks.h helper.h
	gcc -c checks.c $(CFLAGS) -o checks.o


clean:
	rm -f *.o lenschiptool
