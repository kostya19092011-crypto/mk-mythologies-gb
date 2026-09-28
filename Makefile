CC = lcc
CFLAGS = -Wa-l -Wl-m -Wl-j
OUTDIR = build
TARGET = $(OUTDIR)/mk-subzero.gb
SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
