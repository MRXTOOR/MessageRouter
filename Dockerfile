# Multi-stage Dockerfile for Message Router
FROM ubuntu:22.04 as builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libbenchmark-dev \
    libjsoncpp-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the project
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CXX_FLAGS="-O3 -march=native -DNDEBUG" \
          .. && \
    make -j$(nproc)

# Runtime stage
FROM ubuntu:22.04 as runtime

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libbenchmark1 \
    libjsoncpp25 \
    libatomic1 \
    && rm -rf /var/lib/apt/lists/*

# Create results directory
RUN mkdir -p /app/results

# Copy binaries from builder stage
COPY --from=builder /app/build/message_router /app/
COPY --from=builder /app/build/queue_perf /app/benchmarks/
COPY --from=builder /app/build/routing_perf /app/benchmarks/
COPY --from=builder /app/build/memory_perf /app/benchmarks/
COPY --from=builder /app/build/scaling_perf /app/benchmarks/
COPY --from=builder /app/build/simple_bench /app/benchmarks/
COPY --from=builder /app/configs/ /app/configs/

# Set working directory
WORKDIR /app

# Set environment variables for performance
ENV OMP_NUM_THREADS=1
ENV GOMP_CPU_AFFINITY=0

# Default command
CMD ["./message_router"]
