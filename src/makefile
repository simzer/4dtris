
cc=gcc
lint=splint

bin = 4dtris

sources = 4dt_cptr.c \
          4dt_ai.c   \
          4dt_eng.c  \
          4dt_main.c \
          4dt_menu.c \
          4dt_scn.c  \
          4dt_m3d.c  \
          4dt_m4d.c  \
          4dt_g3d.c  \
          4dt_gtxt.c \
          4dt_g4d.c  \
          4dt_hst.c  \
          4dt_ui.c

objects = $(sources:.c=.o) 

cflags = -Wall -Wextra --pedantic -g

libs = -lGLU -lGL -lSDL -lSDL_ttf -lfontconfig

all : $(bin)

lint : $(sources)
	$(lint) $< | perl -pe 'chomp; s/^([^\s])/\n\1/' 2>&1

clean:
	rm -rf $(bin) $(objects)

$(bin) : $(objects)
	$(cc) $(cflags) -o $(bin) $(objects) $(libs)

%.o : %.c
	$(cc) $(cflags) -c -o $@ $< 
