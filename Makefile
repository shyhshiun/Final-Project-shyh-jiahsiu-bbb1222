all:
	gcc demo.c utils/toml/toml.c utils/fn.c -o demo

clean:
	rm demo