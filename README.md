# cppCoroutine

## Introduction
Welcome to the `cppCoroutine` project! This project demonstrates the implementation and usage of coroutines in C++.

## Features
- **Asynchronous Programming**: Simplify asynchronous code with coroutines.
- **Performance**: Efficient context switching and low overhead.
- **Readability**: Write asynchronous code that looks like synchronous code.

## Getting Started
### Prerequisites
- C++20 or later
- CMake 3.10 or later (Currently used in CMake 3.21)

### Installation
1. Clone the repository:
    ```sh
    git clone https://github.com/zhangxuan9812/cppCoroutine.git
    ```
2. Setting Up Your Development Environment:
   ```
   # Linux
   $ sudo build_support/packages.sh
   # macOS(Not Implemented Completely Now)
   $ build_support/packages.sh
   ```
3. Navigate to the project directory:
    ```sh
    cd cppCoroutine
    ```
4. Build the project using CMake:
    ```sh
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make -j`nproc`
    ```
5. Test every individual test, for example, test the thread module
    ```
    cd build
    make cppCoroutine_thread_test -j$(nproc)
    ./test/cppCoroutine_thread_test
   ```

## Usage
Include the coroutine header in your C++ files:
```cpp
#include "coroutine.h"
```

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact
For any questions or suggestions, please open an issue or contact the project maintainer at [your.email@example.com](mailto:your.email@example.com).

Happy coding!