all:
	gcc utils/toml/toml.c utils/fn.c -shared -o libmyc.so -fPIC 

clean:
	rm libmyc.so