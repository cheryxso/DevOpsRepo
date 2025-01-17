FROM debian:bookworm-slim AS build
RUN apt-get update && apt-get install -y g++ make cmake git autoconf && rm -rf /var/lib/apt/lists/*
WORKDIR /app
RUN git clone --branch branchHTTPserver https://github.com/cheryxso/DevOpsRepo.git .
RUN autoreconf -i
RUN ./configure
RUN make


FROM alpine:3.18
RUN apk add --no-cache libstdc++ libc6-compat
WORKDIR /app
COPY --from=build /app/http_server .
EXPOSE 8080
ENTRYPOINT ["./http_server"]

