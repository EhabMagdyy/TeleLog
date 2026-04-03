# Telemetry & Logging System(TeleLog)

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                config.json                                          │
│                  (enable/disable sources & sinks, set rates)                        │
└────────────────────────────────────┬────────────────────────────────────────────────┘
                                     │ parsed at startup & on file change
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                      TeleLogApp  (Façade)                                           │
│  constructor:        loads config, builds LogManager, launches ThreadPool           │
│  start():            blocks until SIGINT, then shuts down                           │
│  ConfigWatcher_Task: polls last_write_time → re-applies config                      │
└────────────────────────────────────┬─────────────────────────────────────────────── ┘
                                     │                  
                                     ▼                  
                              pool->push() × 5
┌───────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                          ThreadPool  (5 worker threads)                                               │
│                                                                                                       │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌────────────────────────┐  ┌──────────────┐  │
│   │   CPU_Task   │  │   RAM_Task   │  │   GPU_Task   │  │  ConfigWatcher_Task    │  │ Routing_Task │  │
│   └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  └───────────┬────────────┘  └──────┬───────┘  │
│          │                 │                 │                      │                      │          │
│          │                 │                 │            polls last_write_time   route logs to sinks │
│          │                 │                 │                      │                (consol/file)    │
│          │                 │                 │                      │                                 │
│          │                 │                 │                  config.json                           │
└──────────┼─────────────────┼─────────────────┼────────────────────────────────────────────────────────┘
           │                 │                 │
           ▼                 ▼                 ▼
  ┌──────────────┐  ┌──────────────────┐  ┌──────────────────────────────────────┐
  │  FileTelSrc  │  │   SockTelSrc     │  │       SomeIPTelemetrySourceImpl      │
  │  reads from  │  │  Unix domain     │  │         (Singleton + Adapter)        │
  │    a file    │  │    socket        │  │  subscribes to SomeIP events         │
  │              │  │                  │  │  + polls requestGpuUsageData()       │
  │  SafeFile    │  │  SafeSocket      │  │                                      │
  │  (RAII)      │  │  (RAII)          │  │                                      │
  └──────┬───────┘  └──────┬───────────┘  │  GpuService fires events / replies   │
         │                 │              └──────────────────┬───────────────────┘
         │                 │                                 │
         └─────────────────┘                                 │  TeleLog requests from
                  │    (RAII)                                │  GpuService & receives
                  │                                          │  broadcasted events 
                  ▼                                          ▼  on warning/critical
┌────────────────────────────────────────────────────────────────────────────────────┐
│                        LogFormatter<Policy>  (Strategy)                            │
│                       (CpuPolicy / RamPolicy / GpuPolicy)                          │
│             takes raw data → infer severity → construct LogMessage                 │
└──────────────────────────────────────┬─────────────────────────────────────────────┘
                                       │ submitLog()
                                       ▼
┌────────────────────────────────────────────────────────────────────────────────────┐
│                             LogManager  +  RingBuffer                              │
│                    addLog() → stores logs in circular buffer                       │
│                    notifies Routing_Task via condition_variable                    │
└──────────────────────────────────────┬─────────────────────────────────────────────┘
                                       │ cv.notify_one()
                                       ▼
┌────────────────────────────────────────────────────────────────────────────────────┐
│                                  Routing_Task                                      │
│                     wakes on newLog flag → routeLogsForAllSinks()                  │
└──────────────────────┬────────────────────────────────┬─────────────────────────── ┘
                       │                                │
                       ▼                                ▼
            ┌──────────────────────┐            ┌───────────────────────┐
            │     ConsoleSink      │            │       FileSink        │
            │  writes to stdout    │            │  appends to .txt file │
            └──────────────────────┘            └───────────────────────┘
```

---

## Output
> [Demo Video](https://github.com/user-attachments/assets/e8c511ca-4b33-4725-9d5c-dd5b2ebc9246)

---
![RPi VSOME/IP GPU](output/rpi-gpu-vsomeip.png)

---

## Build & Run

### 1. Generate capi vsome/ip files
``` bash
ccgen -sk vsomeip/gpu.fidl
csgen vsomeip/gpu.fdepl
# Note:
# alias ccgen='~/Downloads/commonapi_core_generator/commonapi-core-generator-linux-x86_64'
# alias csgen='~/Downloads/commonapi_someip_generator/commonapi-someip-generator-linux-x86_64'
```

### 2. Build the application
``` bash
cmake -S . -B build
cmake --build build
```

### 3. Cross build the capi vsome/ip gpu service
``` bash 
cd Handlers/vsomeip_server/
mkdir build-rpi && cd build-rpi

cmake \
    -DCMAKE_TOOLCHAIN_FILE=~/rpi-toolchain.cmake \
    -DCommonAPI_DIR=~/rpi-sysroot/usr/lib/cmake/CommonAPI-3.2.4 \
    -DCommonAPI-SomeIP_DIR=~/rpi-sysroot/usr/lib/cmake/CommonAPI-SomeIP-3.2.4 \
    -Dvsomeip3_DIR=~/rpi-sysroot/usr/lib/cmake/vsomeip3 \
    ..

make -j$(nproc)

scp /home/ehab/Documents/ITI_9Months/CppProject/Handlers/vsomeip_server/build-rpi/gpuService pi@192.168.50.3:
```

#### 4. on Host, run RAM Socket
``` bash
cd Handlers/
g++ socket.cpp -o socket.exe
./socket.exe
```

### 5. on RPi, run service script
``` bash
./runService.sh
```

### 6. on Host, run app (client)
``` bash
./run.sh
```

### Troubleshooting
#### RPi Multicast not reaching Host 

- Run this on your host while the service is running on the RPi to check if the multicast packets are reaching the host:
    ``` bash
    sudo tcpdump -i eno1 udp port 30490 -v
    ```

- if No packets appear at all => the RPi's multicast SD packets aren't reaching eno1. Fix on the RPi:
    ``` bash
    sudo ip route add 224.244.224.245 dev eth0      # This will allow the RPi to send multicast packets to the host
    ```

---

## Design Patterns

### 1. Façade — `TeleLogApp`
**What it is:** A single class that hides an entire subsystem behind a simple interface.

**Why it matters:** Without it, `main.cpp` would manually wire together the `LogManager`, `ThreadPool`, all tasks, the signal handler, the config watcher, and the sink rebuild logic. Instead the entire system is expressed in two calls.

**Where it's used:**
```cpp
TeleLogApp app("config.json");  // builds everything
app.start();                    // runs until SIGINT
```
`TeleLogApp` internally manages `LogManager`, `ThreadPool`, 5 tasks, atomic config flags, and the signal handler — none of which the caller needs to know about.

---

### 2. Builder — `LogManagerBuilder`
**What it is:** Constructs a complex object step by step using a fluent chaining interface, separating construction from representation.

**Why it matters:** `LogManager` needs sinks configured before it starts routing logs. The Builder makes the construction readable and prevents partially-initialised `LogManager` objects from being used.

**Where it's used:**
```cpp
auto lm = LogManagerBuilder()
    .addSink(std::make_unique<ConsoleSink>())
    .addSink(std::make_unique<FileSink>("fileSink.txt"))
    .build();
```
Also used in `rebuildSinks()` at runtime when the config file changes.

---

### 3. Strategy — `CpuPolicy` / `GpuPolicy` / `RamPolicy` + `LogFormatter<Policy>`
**What it is:** Defines a family of interchangeable algorithms (policies) that can be selected at compile time via templates.

**Why it matters:** CPU, GPU, and RAM all produce a float reading and need a log message — but they have different severity thresholds, units, and context labels. The Strategy pattern captures the differences in a policy struct without duplicating `LogFormatter` logic.

**Where it's used:**
```cpp
LogFormatter<CpuPolicy>::formatDataToLogMsg(data);  // CPU Usage, thresholds 75/90%
LogFormatter<GpuPolicy>::formatDataToLogMsg(data);  // GPU Usage, thresholds 70/90%
LogFormatter<RamPolicy>::formatDataToLogMsg(data);  // RAM Usage, thresholds 80/95%
```
Each policy provides `context`, `unit`, and `inferSeverity()`. `LogFormatter` is the algorithm; the policy is the pluggable behaviour.

---

### 4. Singleton — `SomeIPTelemetrySourceImpl`
**What it is:** Ensures exactly one instance of a class exists for the entire application lifetime, with a global access point.

**Why it matters:** There must be exactly one vsomeip proxy connection to the GPU service. Two instances would create two separate SomeIP connections, two event subscriptions, and duplicate log entries.

**Where it's used:**
```cpp
static SomeIPTelemetrySourceImpl& getInstance(handler) {
    static SomeIPTelemetrySourceImpl instance(std::move(handler)); // once, thread-safe
    return instance;
}
```
Private constructor + deleted copy/move enforce the constraint. C++11 guarantees the static local is initialised exactly once even across threads.

---

### 5. Adapter — `SomeIPTelemetrySourceImpl`
**What it is:** Converts the interface of an existing class into the interface the rest of the system expects, making incompatible interfaces work together.

**Why it matters:** `GpuUsageDataProxy` speaks CommonAPI — floats, `CallStatus`, event subscriptions. `ITelemetrySource` speaks `openSource()` / `readSource(string&)`. The Adapter bridges the two without modifying either.

**Where it's used:** `SomeIPTelemetrySourceImpl` inherits `ITelemetrySource` and wraps `GpuUsageDataProxy` internally:
```
ITelemetrySource (target)        GpuUsageDataProxy (adaptee)
openSource()          →  buildProxy() + isAvailable() + subscribe()
readSource(string&)   →  requestGpuUsageData(CallStatus, float&)
```

---

### 6. Template Method — `GpuUsageDataStub` / `GpuUsageDataStubDefault` *(generated)*
**What it is:** Defines the skeleton of an algorithm in a base class and lets subclasses override specific steps without changing the overall structure.

**Why it matters:** The generated `StubDefault` provides safe no-op implementations for every service method. You only override what you actually care about, and the rest of the lifecycle (adapter init, event dispatch, version negotiation) is inherited.

**Where it's used:**
```cpp
class GpuService : public GpuUsageDataStubDefault {
    void requestGpuUsageData(..., reply_t reply) override {
        reply(randomGpuUsage());   // only this step customised
    }
};
```

---

### 7. RAII — `SafeFile` / `SafeSocket`
**What it is:** Resource Acquisition Is Initialisation — ties the lifetime of a resource (file descriptor, socket) to the lifetime of an object so it is always released correctly.

**Why it matters:** Raw file descriptors and sockets must be closed even when exceptions occur or control paths branch unexpectedly. Wrapping them in RAII objects makes leaks structurally impossible.

**Where it's used:** `FileTelemetrySourceImpl` owns a `std::optional<SafeFile>` — the file descriptor is opened in the constructor and closed automatically in the destructor, with move semantics to allow `std::optional` to hold it without copying.

---

## Design Patterns in Generated CommonAPI Files
**GpuUsageData Interface — omnimetron.gpu**

The CommonAPI code generator produces a set of files from the `.fidl` and `.fdepl` definitions. Each file has a specific architectural role, and together they embody several well-known design patterns.

### 1. Proxy Pattern

Provides a surrogate object that controls access to the remote service running in a separate process.

| File | Role |
|------|------|
| `GpuUsageDataProxyBase.hpp` | **Subject / Abstract Interface** — declares `requestGpuUsageData()` and `getNotifyGpuUsageDataChangeEvent()` as pure virtual methods |
| `GpuUsageDataProxy.hpp` | **Proxy** — implements the Subject interface and forwards every call to the real implementation via `delegate_` |
| `GpuUsageDataSomeIPProxy.hpp` | **Real Subject** — the concrete SomeIP implementation that serialises and sends messages over the network |

`GpuUsageDataProxy` holds a `shared_ptr<GpuUsageDataProxyBase>` called `delegate_`. Every method simply forwards to it:

```cpp
void GpuUsageDataProxy::requestGpuUsageData(...) {
    delegate_->requestGpuUsageData(...);   // forwarded to SomeIPProxy
}
```

The client (`SomeIPTelemetrySourceImpl`) only ever sees `GpuUsageDataProxy` — it has no knowledge of SomeIP, serialisation, or the network transport.

---

### 2. Bridge Pattern

Decouples an abstraction from its implementation so that the two can vary independently.

| File | Role |
|------|------|
| `GpuUsageDataProxy.hpp` | **Abstraction** — the stable interface the application code depends on |
| `GpuUsageDataProxyBase.hpp` | **Implementor** — the abstract base that any binding (SomeIP, D-Bus, …) must implement |
| `GpuUsageDataSomeIPProxy.hpp` | **Concrete Implementor** — the SomeIP binding |

`GpuUsageDataProxy` stores the implementor through a pointer-to-abstract-base (`delegate_`). Swapping the transport from SomeIP to D-Bus requires only a new Concrete Implementor — the Abstraction and all client code remain unchanged. The template parameter pack `<_AttributeExtensions...>` allows additional capability to be mixed in without modifying either side of the bridge.

---

### 3. Observer Pattern (Event / Broadcast)

Defines a one-to-many dependency: when the subject (service) changes state, all registered observers are notified automatically.

| File | Role |
|------|------|
| `GpuUsageDataProxyBase.hpp` | **Subject Interface** — exposes `NotifyGpuUsageDataChangeEvent` and `getNotifyGpuUsageDataChangeEvent()` |
| `GpuUsageDataSomeIPProxy.hpp` | **Concrete Subject** — owns the SomeIP Event object that fires the notification |
| `GpuUsageDataStub.hpp` | **Publisher** — calls `fireNotifyGpuUsageDataChangeEvent(usage)` to broadcast a new value |
| `SomeIPTelemetrySourceImpl` | **Concrete Observer** — calls `.subscribe(lambda)` to register and react to each event |

The service fires the event:
```cpp
service->fireNotifyGpuUsageDataChangeEvent(gpuUsage);
```

The client subscribes at startup and receives every broadcast without polling:
```cpp
proxy->getNotifyGpuUsageDataChangeEvent().subscribe(
    [this](const float gpuUsage) { eventHandler(to_string(gpuUsage)); });
```

This replaces the polling loop (`requestGpuUsageData` every N seconds) with a true push model — the observer is called only when data actually changes.

---

### 4. Template Method Pattern

Defines the skeleton of an algorithm in a base class, deferring specific steps to subclasses.

| File | Role |
|------|------|
| `GpuUsageDataStub.hpp` | **Abstract Class** — declares `requestGpuUsageData()` as pure virtual; provides `fireNotifyGpuUsageDataChangeEvent()` as a concrete template step |
| `GpuUsageDataStubDefault.hpp` | **Concrete Class (default)** — provides a no-op implementation of every virtual step |
| `GpuService` (gpuService.cpp) | **Concrete Class (application)** — overrides `requestGpuUsageData()` with real GPU logic |

`GpuUsageDataStubDefault` provides safe default implementations so the developer only overrides what they care about:

```cpp
class GpuService : public GpuUsageDataStubDefault {
    void requestGpuUsageData(..., reply_t reply) override {
        reply(randomGpuUsage());   // only this step is customised
    }
};
```

---

### Summary

| Pattern | Where | Purpose |
|---------|-------|---------|
| **Proxy** | `GpuUsageDataProxy` | Hides remote transport from client |
| **Bridge** | `Proxy` + `ProxyBase` + `SomeIPProxy` | Decouples interface from binding |
| **Observer** | `NotifyGpuUsageDataChangeEvent` | Push-based GPU event delivery |
| **Template Method** | `GpuUsageDataStub` / `StubDefault` | Service skeleton — override only what you need |

---

## ThreadPool Workflow
```
main thread
│
│  pool.push(taskA)
│    ├─ lock queue
│    ├─ enqueue taskA
│    ├─ unlock queue
│    └─ notify_one() ──────────────► one sleeping worker wakes up
│                                         │
│  pool.push(taskB)                       ├─ locks mutex
│    ├─ lock queue                        ├─ sees tasks.empty() == false
│    ├─ enqueue taskB                     ├─ pops taskA
│    ├─ unlock queue                      ├─ unlocks mutex
│    └─ notify_one() ──────────► another worker wakes up
│                                    │    │
│                                    │    └─ executes taskA()
│                                    │        (no lock held during execution)
│                                    │
│                                    ├─ locks mutex
│                                    ├─ pops taskB
│                                    ├─ unlocks mutex
│                                    └─ executes taskB()
```

> When stop == true
```
Destructor called
│
├─ stop = true
└─ notify_all()
        │
        ├──► Worker 1 wakes
        │       ├─ sees stop=true  →  doesn't exit yet
        │       ├─ sees tasks.empty()==false
        │       ├─ pops & runs taskX   ← still executes!
        │       ├─ loops back
        │       ├─ sees stop=true AND tasks.empty()==true
        │       └─ returns  ✓
        │
        ├──► Worker 2 wakes
        │       ├─ sees stop=true
        │       ├─ sees tasks.empty()==false
        │       ├─ pops & runs taskY   ← still executes!
        │       ├─ loops back
        │       ├─ sees stop=true AND tasks.empty()==true
        │       └─ returns  ✓
        │
        └──► Worker 3 wakes
                ├─ sees stop=true AND tasks.empty()==true(nothing left)
                └─ returns immediately  ✓

main thread: all workers joined → pool destroyed safely
```

---

## Notes

### 1. `std::ostream`

`std::ostream` is the **base class for output streams** in C++.

- Examples of `std::ostream` objects:
  - `std::cout` → console output
  - `std::ofstream` → file output

#### 	Why it's useful here?

- Both `std::ofstream`(file) and `std::cout`(console) are **subclasses of `std::ostream`**.
- Because you overloaded `operator<<` for `LogMessage` **on `std::ostream&`**, it **works for any output stream**.
- **No need to duplicate formatting code** for console vs file — the same `operator<<` handles both.

---

### 2. Strategy Design Pattern

It is a behavioral design pattern that allows you to define a family of algorithms, encapsulate each one in a separate class, and make them interchangeable. This lets the algorithm vary independently from the clients that use it.

Instead of implementing a single class that handles multiple variations of a task using complex conditional logic(like `if-else` or `switch` statements), you delegate the task to one of several specialized strategy objects.

#### Core Components

* **Strategy(Interface):** A common interface for all supported algorithms. It declares a method that the Context uses to execute a strategy.
* **Concrete Strategies:** Classes that implement the Strategy interface using a specific algorithm(e.g., `ConsoleSink` or `FileSink` in your project).
* **Context:** The class that maintains a reference to a Strategy object. It doesn't know the details of how the strategy works; it simply triggers the strategy’s method.

#### Why Use It?

* **Avoids Conditionals:** You can eliminate massive blocks of `if/else` or `switch` statements used to select different behaviors.
* **Open/Closed Principle:** You can introduce new strategies(like a `DatabaseLogger`) without having to change the Context or other existing strategies.
* **Runtime Flexibility:** The Context can switch its behavior at any time by simply swapping the strategy object it is currently holding.
* **Isolation:** The implementation details of an algorithm are hidden away from the code that uses it.

#### Usage:

| **Class / File**          | **Role in Strategy Pattern** | **Description**                                              |
| ------------------------- | ---------------------------- | ------------------------------------------------------------ |
| **`ILogSink.hpp`**        | **Strategy Interface**       | Defines the common behavior(`write`) that all concrete strategies must implement. |
| **`ConsoleSinkImpl.hpp`** | **Concrete Strategy**        | Implements the `write` method to send output specifically to the console. |
| **`FileSinkImpl.hpp`**    | **Concrete Strategy**        | Implements the `write` method to handle logging to a physical file. |
| **`LogManager`**          | **Context**                  | Holds references to `ILogSink` and delegates the logging task to the active strategy. |

---

### 3. `SafeFile`/`SafeSocket` Classes
- They are purely a RAII wrapper for file descriptors/sockets. Its job is ownership management: open/socket,connect, close, move-only semantics. 
- It should not implement higher-level logic like reading, that’s the responsibility of the telemetry source classes `FileTelemetrySourceImpl`/`SocketTelemetrySourceImpl`.

---

### 4. `std::optional<SafeFile>`
- Allows delayed initialization, so `FileTelemetrySourceImpl` can have a default constructor, satisfying Rule-of-Zero.
- `file.emplace("source.txt")`: This constructs in-place and avoids an unnecessary move, used with std::optional

---

### 5. `noexcept`
- on move = enables optimizations and fits standard container requirements.
- Without it, moves might silently degrade into copies in std::vector, std::map, etc.
- Rule: If your move cannot throw, always mark it noexcept.

---

### 6. UNIX Domain Socket
##### A mechanism for inter-process communication(IPC) on the same host, using the file system namespace instead of network addresses.
##### Key points:
- Works only within one machine
- Uses filesystem paths to identify endpoints, not IP addresses or ports
- Supports stream(SOCK_STREAM) or datagram(SOCK_DGRAM) semantics
- Faster than TCP/IP sockets for local IPC(no network stack overhead)
##### Typical workflow
- For client-side:
  - socket(AF_UNIX, SOCK_STREAM, 0) → create socket
  - connect() → connect to server using sockaddr_un
  - read() / write() → send/receive data
  - close() → cleanup

- For server-side:
  - socket(AF_UNIX, SOCK_STREAM, 0) → create socket
  - bind() → bind to a path(like /tmp/telemetry.sock)
  - listen() → listen for clients
  - accept() → accept connections
  - read() / write() → communicate
  - close() → cleanup

---

### 7. UML Relations
#### UML Relationship Comparison

| Relationship | UML Notation | Meaning | Ownership | Lifetime Dependency | Typical C++ Usage |
|-------------|-------------|--------|-----------|---------------------|------------------|
| **Inheritance** | `--|>` | “Is-a” relationship between base and derived classes | ❌ No | ❌ No | `class A : public B` |
| **Interface Implementation** | `..|>` | Class implements an interface | ❌ No | ❌ No | `class Impl : public Interface` |
| **Composition** | `*--` | Strong ownership(part cannot exist without whole) | ✅ Yes | ✅ Yes | RAII member objects |
| **Aggregation** | `o--` | Weak ownership(part can exist independently) | ⚠️ Shared | ❌ No | Containers of pointers / smart pointers |
| **Association** | `--` | General relationship / knows about | ❌ No | ❌ No | References, pointers |
| **Dependency(Uses)** | `..>` | Temporary usage | ❌ No | ❌ No | Function parameters, return values |
| **Enum Association** | `o--` | Class uses an enum | ❌ No | ❌ No | Enum as data member |

![](./diagrams/relations.jpg)

---

### 8. Policy

A policy is a template parameter that defines rules or behavior for a generic class—in this case, LogFormatter.
Instead of hardcoding logic for CPU, RAM, GPU, etc., a policy:
- Encapsulates context, units, and thresholds.
- Provides a static method to infer severity based on a value.
- Lets LogFormatter work generically for any telemetry source.
So the policy is a way of customizing the behavior of the formatter without modifying the formatter code itself.

---

### 9. `magic_enum`

1. No manual mapping

You don’t need to write and maintain:

``` cpp
switch(src) {
    case TelemetrySrc_enum::CPU: return "CPU Usage";
    ...
}
```

`magic_enum` converts `enums` → `strings` automatically.

2. Refactor-safe

If you add a new enum value(e.g. DISK),
you won’t forget to update the switch(a very common bug).

3. Cleaner code

``` cpp
magic_enum::enum_name(TelemetrySrc_enum::CPU); // "CPU"`
```

4. Compile-time, zero runtime cost

It’s header-only and mostly constexpr

No performance penalty

---

### 10. Ring-Buffer

It's like a `circular-queue`

| Feature          | Ring Buffer        | std::vector        |
|------------------|--------------------|--------------------|
| Memory usage     | Fixed              | Grows dynamically  |
| Real-time safe   | ✅ Yes             | ❌ No(reallocations) |
| Overwrites old   | Optional           | ❌ No              |
| Embedded use     | 🔥 Very common     | Meh                |

---

### 11. ThreadPool

A **Thread Pool** is a concurrency pattern where a fixed set of worker threads is created once and reused to execute multiple tasks, rather than spawning a new thread for every task.

#### - How It Works

```
push(task)
    │
    ▼
[ Task Queue ] ──► Worker Thread 1
               ──► Worker Thread 2
               ──► Worker Thread 3
```

Tasks are submitted to a shared queue. Idle workers compete to pick them up and execute them. When a worker finishes, it goes back to waiting for the next task.

#### - Core Components

| Component | Role |
|---|---|
| `vector<thread>` | The fixed pool of reusable worker threads |
| `queue<function<void()>>` | Pending tasks waiting to be picked up |
| `mutex` | Protects the queue from concurrent access |
| `condition_variable` | Puts idle workers to sleep and wakes them when work arrives |
| `push()` | Submits a new task into the queue |
| `stop` flag | Signals workers to exit cleanly on destruction |

---

#### - Advantages Over Raw Threads

##### No thread creation overhead
Creating a thread is expensive — it allocates a stack and registers with the OS. With a pool, threads are created **once** at startup and reused for every task thereafter.

##### Automatic lifecycle management
Raw threads require manual `.join()` on every thread, and forgetting one causes undefined behavior. The pool destructor handles all joins automatically in one place.

##### Controlled resource usage
Raw threads let you accidentally spawn hundreds of threads under load. The pool **caps** the number of concurrent threads, preventing resource exhaustion and excessive context switching.

##### Scales easily with more tasks
Adding a new task with raw threads means declaring a new `std::thread`, managing its lifetime, and joining it manually. With a pool, it's one `push()` call — the pool absorbs it with no extra management.

##### Clean, centralized shutdown
With raw threads, stopping everything cleanly requires careful coordination across all threads. The pool centralizes this — one `stop` flag, one `notify_all()`, one destructor.
