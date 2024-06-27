CC = g++
CFLAGS = -c -O3 -std=c++11

BIN = CGR

all: clean $(BIN)

$(BIN): main.o jsonParser.o block.o net.o defParser.o output.o input.o netadd.o congestion.o oasg.o oarsmt.o
	$(CC) $^ -o $@

main.o: src/main.cpp src/nlohmann/json.hpp
	$(CC) $(CFLAGS) $< -o $@

oarsmt.o: src/oarsmt.cpp src/oarsmt.h
	$(CC) $(CFLAGS) $< -o $@

# oasg.o: src/oasg.cpp src/oasg.h
# 	$(CC) $(CFLAGS) $< -o $@

oasg.o: src/oasgnobox.cpp src/oasg.h
	$(CC) $(CFLAGS) $< -o $@

congestion.o: src/congestion.cpp src/congestion.h
	$(CC) $(CFLAGS) $< -o $@

input.o: src/input.cpp src/input.h 
	$(CC) $(CFLAGS) $< -o $@

netadd.o: src/netadd.cpp src/netadd.h
	$(CC) $(CFLAGS) $< -o $@

output.o: src/output.cpp src/output.h 
	$(CC) $(CFLAGS) $< -o $@

block.o: src/block.cpp src/block.h
	$(CC) $(CFLAGS) $< -o $@

net.o: src/net.cpp src/net.h
	$(CC) $(CFLAGS) $< -o $@

jsonParser.o: src/jsonParser.cpp src/jsonParser.h
	$(CC) $(CFLAGS) $< -o $@

defParser.o: src/defParser.cpp src/defParser.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN) *.o

.PHONY: all clean
