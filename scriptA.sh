#!/bin/bash

IMAGE_NAME="holksen/http_server:multi"

log() {
  local level=$1
  local message=$2
  echo "[$(date '+%Y-%m-%d %H:%M:%S')] [$level] $message"
}

# Function to start a container on a specific CPU core
start_container() {
  local container=$1
  local core=$2

  if docker ps -a --format '{{.Names}}' | grep -q "$container"; then
    if ! docker ps --format '{{.Names}}' | grep -q "$container"; then
      log "INFO" "Removing stopped container: $container"
      docker rm "$container"
    else
      log "INFO" "Container $container is already running."
      return
    fi
  fi

  log "INFO" "Starting container $container on core $core..."
  docker run -d --name "$container" --cpuset-cpus="$core" "$IMAGE_NAME"
  sleep 10
}

# Function to get the CPU usage of a container
get_cpu_usage() {
  local container=$1
  docker stats "$container" --no-stream --format "{{.CPUPerc}}" | sed 's/%//'
}

# Function to update running containers with the latest image
update_containers() {
  log "INFO" "Checking for new image version of $IMAGE_NAME..."
  if docker pull "$IMAGE_NAME" | grep -q 'Downloaded newer image'; then
    log "INFO" "New image version detected. Updating running containers..."
    declare -A core_map=( ["srv1"]=0 ["srv2"]=1 ["srv3"]=2 )
    for container in srv1 srv2 srv3; do
      if docker ps --format '{{.Names}}' | grep -q "$container"; then
        log "INFO" "Updating $container..."
        docker stop "$container"
        docker rm "$container"
        start_container "$container" "${core_map[$container]}"
      fi
    done
  else
    log "INFO" "No new image version detected. All containers are up-to-date."
  fi
}

# Main loop
log "INFO" "Starting HTTP server monitoring script..."
last_check=$(date +%s)
while true; do
  # Ensure srv1 is always running
  start_container srv1 0
  log "DEBUG" "Monitoring container: srv1"

  # Monitor srv1 for high CPU usage
  if docker ps --format '{{.Names}}' | grep -q srv1; then
    usage1=$(get_cpu_usage srv1)
    sleep 60
    usage2=$(get_cpu_usage srv1)
    if (( $(echo "$usage1 > 80" | bc -l) && $(echo "$usage2 > 80" | bc -l) )); then
      log "WARNING" "Container srv1 CPU usage exceeded 80%. Starting srv2..."
      start_container srv2 1
    fi
  fi

  # Monitor srv2 for high or low CPU usage
  if docker ps --format '{{.Names}}' | grep -q srv2; then
    usage1=$(get_cpu_usage srv2)
    sleep 60
    usage2=$(get_cpu_usage srv2)
    if (( $(echo "$usage1 > 80" | bc -l) && $(echo "$usage2 > 80" | bc -l) )); then
      log "WARNING" "Container srv2 CPU usage exceeded 80%. Starting srv3..."
      start_container srv3 2
    elif (( $(echo "$usage1 < 10" | bc -l) && $(echo "$usage2 < 10" | bc -l) )); then
      log "INFO" "Container srv2 is idle. Stopping it..."
      docker stop srv2
      docker rm srv2
    fi
  fi

  # Monitor srv3 for low CPU usage
  if docker ps --format '{{.Names}}' | grep -q srv3; then
    usage1=$(get_cpu_usage srv3)
    sleep 60
    usage2=$(get_cpu_usage srv3)
    if (( $(echo "$usage1 < 10" | bc -l) && $(echo "$usage2 < 10" | bc -l) )); then
      log "INFO" "Container srv3 is idle. Stopping it..."
      docker stop srv3
      docker rm srv3
    fi
  fi

  # Check for new image version every 10 minutes
  current_time=$(date +%s)
  if (( current_time - last_check >= 600 )); then
    update_containers
    last_check=$current_time
  fi

  sleep 20
done

