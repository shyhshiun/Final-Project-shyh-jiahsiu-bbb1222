all:
#	gcc demo.c utils/toml/toml.c utils/fn.c -o demo
#	gcc -o demo demo_can_show_image.c utils/toml/toml.c utils/fn.c `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf
#	./demo
	gcc -o demo demo.c utils/toml/toml.c utils/fn.c `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf
	./demo
	
clean:
	rm demo