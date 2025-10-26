g++ -Wall -Wextra -O2 -g server.cpp -o server
g++ -Wall -Wextra -O2 -g client.cpp -o client
./server &         # Start server in background
SERVER_PID=$!      # Capture its PID
sleep 5
./client
kill $SERVER_PID
