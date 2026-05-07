FROM debian:trixie-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
	clang \
	zlib1g-dev \
	make \
	&& rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends git ca-certificates libcurl4-openssl-dev \
	&& rm -rf /app/uWebSockets \
	&& git clone --recurse-submodules https://github.com/uNetworking/uWebSockets.git /app/uWebSockets \
	&& rm -rf /var/lib/apt/lists/*-

WORKDIR /app
COPY . .

RUN make -j

FROM debian:trixie-slim

RUN apt-get update && apt-get install -y --no-install-recommends libcurl4-openssl-dev valgrind && rm -rf /var/lib/apt/lists/*-

WORKDIR /src/game-server
COPY --from=builder /app/game-server .

CMD ["valgrind", "./game-server"]