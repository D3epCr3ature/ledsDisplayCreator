
EXE    =qtDisplayDriver
EXE_DBG=$(EXE).dbg

IP   =127.0.0.1
PORT =5000
SRC ?= bmthStar_DisplayDriver.c

.PHONY: clean run run_dbg

all: $(EXE)

$(EXE): $(SRC)
	gcc -Wall -o $@ $<

run: $(EXE)
	./$< $(IP) $(PORT)

$(EXE_DBG): $(SRC)
	gcc -Wall -DENA_DBG=1 -o $@ $<

run_dbg: $(EXE_DBG)
	./$< $(IP) $(PORT)

clean:
	rm -rf $(EXE) $(EXE_DBG)

