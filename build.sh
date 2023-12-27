if [ ! -L $(pwd)/lib/libraylib.so.450 ]; then
    ln -s $(pwd)/lib/libraylib.so $(pwd)/lib/libraylib.so.450
fi
rm ./bin/*
mkdir -p bin 
gcc -o ./bin/minesweeper minesweeper.c game.c font.c asset.c -L./lib/  -I./include -lm -Wl,-rpath=./lib/ -lraylib
