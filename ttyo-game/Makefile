SOURCES = Field.cpp Grid.cpp ttyo.cpp Puyo.cpp PuyoNode.cpp

ttyo: $(SOURCES)
	g++ -g $(SOURCES) -lncurses -o ttyo -O3 -Werror -Wall

run: ttyo
	./ttyo
