hello:
	echo "Hello, World"

ttyo: ttyo.cpp
	g++ ttyo.cpp -lncurses -o ttyo -O3

run: ttyo
	./ttyo
