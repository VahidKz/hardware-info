# Contributing

Thanks for helping improve Hardware Info.

## Development setup

Install CMake 3.16 or newer, a C++17 compiler, and either Qt 5 Core or Qt 6 Core.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Pull requests

- Keep changes focused and explain the user-visible behavior.
- Add or update tests when behavior changes.
- Run the build and test commands above before submitting.
- Do not add hardware identifiers, command output, or other sensitive machine
  data to issues or test fixtures.

By contributing, you agree that your contribution is licensed under the MIT
License.
