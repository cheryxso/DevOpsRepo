#!/bin/bash

SERVER_URL="http://127.0.0.1/compute"
MAX_PARALLEL_REQUESTS=15  # Limit the number of parallel requests

# Function to send an HTTP request
send_request() {
  while true; do
    # Generate a random sleep time between 5 and 10 seconds
    sleep_time=$((RANDOM % 6 + 5))

    # Send the HTTP request
    echo "$(date '+%Y-%m-%d %H:%M:%S') Sending request to $SERVER_URL"
    curl -s "$SERVER_URL" > /dev/null

    # Sleep for the generated time before sending the next request
    sleep "$sleep_time"
  done
}

# Start multiple asynchronous request loops
for i in {1..5}; do
  send_request &  # Start each request loop in the background
done

# Wait for all background processes
wait
