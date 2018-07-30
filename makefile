
CPP      = g++.exe -D__DEBUG__
CC       = gcc.exe -D__DEBUG__

LIBS     = -L"D:/Program Files/Dev-Cpp/MinGW64/lib" -L"D:/Program Files/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib" -static-libgcc -g3
INCS     = -I"D:/Program Files/Dev-Cpp/MinGW64/include" -I"D:/Program Files/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"D:/Program Files/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include"
CXXINCS  = -I"D:/Program Files/Dev-Cpp/MinGW64/include" -I"D:/Program Files/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"D:/Program Files/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include" -I"D:/Program Files/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include/c++"

CXXFLAGS = $(CXXINCS) -g3
CFLAGS   = $(INCS) -g3
RM       = rm.exe -f

BIN      = main.exe

.PHONY: all clean

all: $(BIN)

clean:
	${RM} $(OBJS) $(BIN)


OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN) $(LIBS)
