#compile options
# -lsubunit -lm
CFLAGS := -std=c99 -Wall -pedantic

#object files
OBJS_MAIN := out/main.o
.PHONY: release debug
release: CFLAGS += -DNDEBUG -g0
debug: CFLAGS += -g3

release: app.out
debug: app.out
all: app.out

app.out: $(OBJS_MAIN)
	gcc $(CFLAGS) -o $@ $^

out/%.o : src/%.c inc/*.h
	gcc $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm out/*.o *.out