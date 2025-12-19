srcs := $(wildcard src/*.c) $(wildcard src/utils/*.c)
objs := $(patsubst src/%.c,target/%.o,${srcs})
cflags := -Wall -std=gnu17 -Iinclude

init: target
	mkdir -p target/utils

cdb: ${objs}
	${CC} ${cflags} $^ -o $@

target/%.o: src/%.c
	${CC} ${cflags} $< -c -o $@

format:
	find src include -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format -i -style=file {} +

clean:
	rm -rf target/*

.PHONY: clean init format src include