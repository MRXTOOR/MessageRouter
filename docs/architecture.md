# Architecture Diagrams

## System Architecture

```mermaid
graph TB
    subgraph "Producers"
        P1[Producer 1]
        P2[Producer 2]
        P3[Producer 3]
        P4[Producer 4]
    end
    
    subgraph "Stage1 Router"
        S1R[Stage1 Router]
    end
    
    subgraph "Processors"
        PR1[Processor 1]
        PR2[Processor 2]
        PR3[Processor 3]
        PR4[Processor 4]
    end
    
    subgraph "Stage2 Router"
        S2R[Stage2 Router]
    end
    
    subgraph "Strategies"
        ST1[Strategy 1]
        ST2[Strategy 2]
        ST3[Strategy 3]
    end
    
    P1 --> S1R
    P2 --> S1R
    P3 --> S1R
    P4 --> S1R
    
    S1R --> PR1
    S1R --> PR2
    S1R --> PR3
    S1R --> PR4
    
    PR1 --> S2R
    PR2 --> S2R
    PR3 --> S2R
    PR4 --> S2R
    
    S2R --> ST1
    S2R --> ST2
    S2R --> ST3
```

## Message Flow

```mermaid
sequenceDiagram
    participant P as Producer
    participant S1 as Stage1 Router
    participant PR as Processor
    participant S2 as Stage2 Router
    participant ST as Strategy
    
    P->>P: Generate Message
    Note over P: msg_type, producer_id,<br/>sequence_number, timestamp
    
    P->>S1: Send Message
    S1->>S1: Determine Processor
    S1->>PR: Route Message
    
    PR->>PR: Process Message
    Note over PR: Add processor_id,<br/>processing_timestamp
    
    PR->>S2: Send Processed Message
    S2->>S2: Determine Strategy
    S2->>ST: Route Message
    
    ST->>ST: Validate Ordering
    ST->>ST: Process Strategy Logic
    Note over ST: Message delivered
```

## Queue Architecture

```mermaid
graph LR
    subgraph "SPSC Queues"
        Q1[Producer Queue 1]
        Q2[Producer Queue 2]
        Q3[Producer Queue 3]
        Q4[Producer Queue 4]
    end
    
    subgraph "Stage1 Queues"
        SQ1[Stage1 Queue 1]
        SQ2[Stage1 Queue 2]
        SQ3[Stage1 Queue 3]
        SQ4[Stage1 Queue 4]
    end
    
    subgraph "Processor Queues"
        PQ1[Processor Queue 1]
        PQ2[Processor Queue 2]
        PQ3[Processor Queue 3]
        PQ4[Processor Queue 4]
    end
    
    subgraph "Stage2 Queues"
        S2Q1[Stage2 Queue 1]
        S2Q2[Stage2 Queue 2]
        S2Q3[Stage2 Queue 3]
    end
    
    Q1 --> SQ1
    Q2 --> SQ2
    Q3 --> SQ3
    Q4 --> SQ4
    
    SQ1 --> PQ1
    SQ2 --> PQ2
    SQ3 --> PQ3
    SQ4 --> PQ4
    
    PQ1 --> S2Q1
    PQ2 --> S2Q2
    PQ3 --> S2Q3
    PQ4 --> S2Q1
```

## Lock-Free Queue Implementation

```mermaid
graph TB
    subgraph "LockFreeSPSCQueue"
        subgraph "Buffer"
            B1[Node 0]
            B2[Node 1]
            B3[Node 2]
            BN[Node N-1]
        end
        
        H[Head Pointer]
        T[Tail Pointer]
        
        subgraph "Node Structure"
            D[Data]
            R[Ready Flag]
        end
    end
    
    H --> B1
    T --> B2
    
    B1 --> D
    B1 --> R
```

## Performance Characteristics

```mermaid
graph TB
    subgraph "Performance Metrics"
        T[Throughput<br/>19M+ msg/sec]
        L[Latency<br/>< 5μs p99]
        Z[Zero Loss<br/>100% delivery]
        O[Ordering<br/>Guaranteed]
    end
    
    subgraph "Optimization Techniques"
        LF[Lock-Free Design]
        CA[Cache Alignment]
        BW[Busy Waiting]
        ZC[Zero Copy]
    end
    
    subgraph "Hardware Requirements"
        CPU[Multi-Core CPU]
        RAM[Sufficient RAM]
        CACHE[CPU Cache]
    end
    
    LF --> T
    CA --> L
    BW --> L
    ZC --> T
    
    CPU --> T
    RAM --> Z
    CACHE --> L
```

## Configuration Flow

```mermaid
graph LR
    CF[Config File<br/>JSON] --> SC[SystemConfig]
    
    SC --> PC[ProducerConfig]
    SC --> PRC[ProcessorConfig]
    SC --> STC[StrategyConfig]
    SC --> RC[RoutingConfig]
    
    PC --> P[Producers]
    PRC --> PR[Processors]
    STC --> ST[Strategies]
    RC --> S1R[Stage1 Router]
    RC --> S2R[Stage2 Router]
```

## Error Handling

```mermaid
graph TB
    subgraph "Error Types"
        QF[Queue Full]
        RO[Routing Error]
        OV[Ordering Violation]
        TO[Timeout]
    end
    
    subgraph "Handling Strategies"
        RT[Retry Logic]
        ER[Error Reporting]
        FL[Failover]
        AB[Abort]
    end
    
    QF --> RT
    RO --> ER
    OV --> ER
    TO --> FL
    
    RT --> SUCCESS[Success]
    ER --> LOG[Log Error]
    FL --> RECOVER[Recover]
    AB --> STOP[Stop System]
```

## Monitoring and Metrics

```mermaid
graph TB
    subgraph "Real-time Metrics"
        TP[Throughput]
        QD[Queue Depth]
        LT[Latency]
        ER[Error Rate]
    end
    
    subgraph "Validation"
        OV[Ordering Validation]
        ML[Message Loss Check]
        PC[Performance Check]
    end
    
    subgraph "Reporting"
        RT[Real-time Display]
        FR[Final Report]
        BR[Benchmark Results]
    end
    
    TP --> RT
    QD --> RT
    LT --> RT
    ER --> RT
    
    OV --> FR
    ML --> FR
    PC --> FR
    
    RT --> BR
    FR --> BR
```
