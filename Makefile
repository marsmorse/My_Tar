# C Compiler
CC = gcc
# C Compiler flags
CFLAGS = -Wall -Wextra -Wpedantic -Wshadow
#Target executable name 
TARGET = my_tar
#ARCHIVETarget
ARCHIVE = archive

ARCHIVEHEADER = ArchiveHeader
PARSEINPUT = parseInput
PARSERHELPER = parserHelper
MYSTR = my_str
BYTEDIFF = findDifferentBytes

all: $(TARGET)

$(TARGET): $(TARGET).o $(ARCHIVE).o $(ARCHIVEHEADER).o $(PARSEINPUT).o $(PARSERHELPER).o $(MYSTR).o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o $(ARCHIVE).o $(ARCHIVEHEADER).o $(PARSEINPUT).o $(PARSERHELPER).o $(MYSTR).o

$(TARGET).o: $(TARGET).c $(PARSEINPUT).o
	$(CC) $(CFLAGS) -c $(TARGET).c

$(ARCHIVE).o: $(ARCHIVE).c $(ARCHIVE).h 
	$(CC) $(CFLAGS) -c $(ARCHIVE).c

$(ARCHIVEHEADER).o: $(ARCHIVEHEADER).c $(ARCHIVEHEADER).h
	$(CC) $(CFLAGS) -c $(ARCHIVEHEADER).c

$(PARSEINPUT).o: $(PARSEINPUT).c $(PARSEINPUT).h
	$(CC) $(CFLAGS) -c $(PARSEINPUT).c

$(MYSTR).o: $(MYSTR).c $(MYSTR).h
	$(CC) $(CFLAGS) -c $(MYSTR).c

$(PARSERHELPER).o: $(PARSERHELPER).c $(PARSERHELPER).h 
	$(CC) $(CFLAGS) -c $(PARSERHELPER).c


clean:
	rm *.o $(TARGET)

spotless:
	clean rm $(TARGET).exe

testing:  $(TARGET)
	cp $(TARGET) ./test && cd test && ./test.sh && rm my_tar && cd ..

testBytes: $(BYTEDIFF)
	cp $(BYTEDIFF) ./test && cd test && ./$(BYTEDIFF) test_results my_tar_result.tar tar_result.tar
	
$(BYTEDIFF): $(BYTEDIFF).o
	$(CC) $(CFLAGS) -o $(BYTEDIFF) $(BYTEDIFF).o

$(BYTEDIFF).o: $(BYTEDIFF).c
	$(CC) $(CFLAGS) -c $(BYTEDIFF).c
