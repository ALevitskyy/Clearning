rm -rf bin
mkdir -p bin
g++ -Wall -Wextra -O2 -g server.cpp -o bin/server
g++ -Wall -Wextra -O2 -g client.cpp -o bin/client
./bin/server &         # Start server in background
SERVER_PID=$!          # Capture its PID
sleep 1
echo Command: asdf
./bin/client asdf
echo Command: get asdf
./bin/client get asdf
echo Command: set k v
./bin/client set k v
echo Command: get k
./bin/client get k
echo Command: keys
./bin/client keys
echo Command: del k
./bin/client del k
echo Command: del k
./bin/client del k
echo Command: keys
./bin/client keys
kill $SERVER_PID
