## Telemetry & Logging System (TeleLog)

### System Architecture



### Notes

#### 1. `std::ostream`

`std::ostream` is the **base class for output streams** in C++.

- Examples of `std::ostream` objects:
  - `std::cout` → console output
  - `std::ofstream` → file output

###### 	Why it's useful here?

- Both `std::ofstream` (file) and `std::cout` (console) are **subclasses of `std::ostream`**.
- Because you overloaded `operator<<` for `LogMessage` **on `std::ostream&`**, it **works for any output stream**.
- **No need to duplicate formatting code** for console vs file — the same `operator<<` handles both.

##### 2. Strategy Design Pattern

It is a behavioral design pattern that allows you to define a family of algorithms, encapsulate each one in a separate class, and make them interchangeable. This lets the algorithm vary independently from the clients that use it.

Instead of implementing a single class that handles multiple variations of a task using complex conditional logic (like `if-else` or `switch` statements), you delegate the task to one of several specialized strategy objects.

##### Core Components

* **Strategy (Interface):** A common interface for all supported algorithms. It declares a method that the Context uses to execute a strategy.
* **Concrete Strategies:** Classes that implement the Strategy interface using a specific algorithm (e.g., `ConsoleSink` or `FileSink` in your project).
* **Context:** The class that maintains a reference to a Strategy object. It doesn't know the details of how the strategy works; it simply triggers the strategy’s method.

##### Why Use It?

* **Avoids Conditionals:** You can eliminate massive blocks of `if/else` or `switch` statements used to select different behaviors.
* **Open/Closed Principle:** You can introduce new strategies (like a `DatabaseLogger`) without having to change the Context or other existing strategies.
* **Runtime Flexibility:** The Context can switch its behavior at any time by simply swapping the strategy object it is currently holding.
* **Isolation:** The implementation details of an algorithm are hidden away from the code that uses it.

##### Usage:

| **Class / File**          | **Role in Strategy Pattern** | **Description**                                              |
| ------------------------- | ---------------------------- | ------------------------------------------------------------ |
| **`ILogSink.hpp`**        | **Strategy Interface**       | Defines the common behavior (`write`) that all concrete strategies must implement. |
| **`ConsoleSinkImpl.hpp`** | **Concrete Strategy**        | Implements the `write` method to send output specifically to the console. |
| **`FileSinkImpl.hpp`**    | **Concrete Strategy**        | Implements the `write` method to handle logging to a physical file. |
| **`LogManager`**          | **Context**                  | Holds references to `ILogSink` and delegates the logging task to the active strategy. |