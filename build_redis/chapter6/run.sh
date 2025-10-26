mkdir -p bin
g++ -Wall -Wextra -O2 -g server.cpp -o bin/server
g++ -Wall -Wextra -O2 -g client.cpp -o bin/client
./bin/server &         # Start server in background
SERVER_PID=$!          # Capture its PID
sleep 1
./bin/client
kill $SERVER_PID
