all:
	gcc parse_toml.c utils/toml/toml.c -o parse_toml 

clean:
	rm parse_toml