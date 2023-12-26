if [ ! -L $(pwd)/lib/libraylib.so.450 ]; then
    ln -s $(pwd)/lib/libraylib.so $(pwd)/lib/libraylib.so.450
fi
gcc -o minsweeper game.c asset.c -L./lib/  -I./include -lm -Wl,-rpath=./lib/ -lraylib
