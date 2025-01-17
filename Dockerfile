FROM debian:bullseye-slim
WORKDIR /app

RUN apt-get update && \
    apt-get install -y g++ make cmake && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

COPY . /app

RUN make
EXPOSE 8080
CMD ["/app/http_server"]

