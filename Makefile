CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-ljpeg
#SOURCES= mandel.c jpegrw.c mandel_movie.c
# OBJECTS=$(SOURCES:.c=.o)
EXES= mandel movie

#all: $(SOURCES) $(EXES) 
all: $(EXES)
# pull in dependency info for *existing* .o files
# -include $(OBJECTS:.o=.d)

-include *.d

mandel: mandel.o jpegrw.o
	$(CC) $^ $(LDFLAGS) -o $@

movie: mandel_movie.o jpegrw.o
	$(CC) $^ $(LDFLAGS) -o $@

#$(EXES): $(OBJECTS)
#	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

#.c.o: 
%.o: %.c	
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

clean:
#	rm -rf $(OBJECTS) $(EXES) *.d
	rm -rf *.o $(EXES) *.d
	rm mandel*.jpg
	
	
