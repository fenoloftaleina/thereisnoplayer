# ./deps.sh linux-debug64
# ./deps.sh osx-debug64

cd bx
make $1

cd ../bimg
make $1

cd ../bgfx
make $1
