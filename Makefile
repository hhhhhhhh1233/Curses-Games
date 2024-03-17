hello:
	echo "Hello, World"

ttyo: ttyo.cpp
	g++ ttyo.cpp -lncurses -o ttyo

run: ttyo
	./ttyo
