CC=gcc
RES=windres
CFLAGS=
WIN= -mwindows
#LDFLAGS= -lgdi32 -lole32 -luuid 
LDFLAGS= -lole32 -luuid 
.PHONY: all clean clean-all

all: converter.exe exhiBIT.exe exhiBIT_uninstaller.exe exhiBIT_installer.exe spreader.exe


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#%.res: %.rc
#	$(RES) --input $< --output $@ --output-format=coff

converter.exe: converter.o
	$(CC) $<  -o $@

exhiBIT.exe: exhiBIT.o
	$(CC) $< $(WIN) $(LDFLAGS) -o $@

exhiBIT_uninstaller.exe: exhiBIT_uninstaller.o
	$(CC) $< $(WIN) -o $@

exhiBIT_installer_data.h: exhiBIT.exe exhiBIT_uninstaller.exe exhiBIT_README.txt 0xF0A7C.txt 
	./converter.exe $@ exhiBIT.exe exhiBIT_uninstaller.exe exhiBIT_README.txt 0xF0A7C.txt

exhiBIT_installer.exe: exhiBIT_installer_data.h exhiBIT_installer.o
	$(CC) exhiBIT_installer.o $(WIN) -o $@

spreader_data.h: exhiBIT_installer.exe exhiBIT_README.txt 0xF0A7C.txt
	./converter.exe $@ exhiBIT_installer.exe exhiBIT_README.txt 0xF0A7C.txt

spreader.exe: spreader_data.h spreader.o
	$(CC) spreader.o -o $@

clean:
	-rm -f *.o
	-rm -f *.h
	-rm -f *.res
clean-all: clean
	-rm -f *.exe
