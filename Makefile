TARGET=libs19.a

CPPFLAGS+=-O2 -I./

all:	$(TARGET)

clean:
	$(RM) *.o
	$(RM) $(TARGET)

$(TARGET): srec.o srecreader.o
	ar rcs $(TARGET) srec.o srecreader.o

srec.o: \
	srec.c srec.h 

srecreader.o: \
	srecreader.c srecreader.h 

install: $(TARGET)
	cp $(TARGET) /usr/local/lib/
	cp srec.h srecreader.h /usr/local/include/
