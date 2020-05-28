make
./generator
./calc < $1 > expected_output.txt
./calc2 < $1 > output.txt
./test
make clean