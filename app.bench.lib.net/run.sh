#!/bin/sh
rm -f bench.output
echo "ID\tConnections\tPackageSize\tPackages\tBytes" > bench.output

./app.bench.lib.net -c 1 -p 1 -s 1 >> bench.output
./app.bench.lib.net -c 50 -p 1 -s 1 >> bench.output
./app.bench.lib.net -c 100 -p 1 -s 1 >> bench.output
./app.bench.lib.net -c 200 -p 1 -s 1 >> bench.output
./app.bench.lib.net -c 400 -p 1 -s 1 >> bench.output
./app.bench.lib.net -c 800 -p 1 -s 1 >> bench.output
./app.bench.lib.net -c 1600 -p 1 -s 1 >> bench.output
./app.bench.lib.net -c 3200 -p 1 -s 1 >> bench.output
