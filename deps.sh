# ./deps.sh linux
# ./deps.sh osx

cd bx
make $1

cd ../bimg
make $1

cd ../bgfx
make $1
