# FLAGS = -g -lm -Wall -Wextra -std=c99
all: build

build: 
	 gcc -g -std=c99 main.c tree.c -o sd_fs

clean:
	rm *.o sd_fs

run:
	# time timeout 50 valgrind --leak-check=full --show-leak-kinds=all ./sd_fs
	valgrind --leak-check=full --show-leak-kinds=all ./sd_fs 
