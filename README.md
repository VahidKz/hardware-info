# Hardware Info

[![CI](https://github.com/VahidKz/hardware-info/actions/workflows/ci.yml/badge.svg)](https://github.com/VahidKz/hardware-info/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C.svg)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-5%20%7C%206-41CD52.svg)](https://www.qt.io/)

A small Qt library and command-line tool for reading hardware identifiers on
Linux. It has no shell-command dependency, supports Qt 5 and Qt 6, and provides
a deterministic SHA-256 machine fingerprint for application-level device
identification.

## Features

- Reads ARM CPU serials from `/proc/cpuinfo`
- Reads x86 processor signature data through CPUID
- Discovers physical disk serials and WWIDs through `/sys/class/block`
- Ignores loop devices, RAM disks, optical drives, and partitions
- Produces a fixed-length machine fingerprint without returning the raw
  identifiers
- Supports human-readable and JSON command-line output
- Exposes a reusable CMake target: `HardwareInfo::HardwareInfo`

## Requirements

- Linux
- CMake 3.16+
- A C++17 compiler
- Qt 5 Core or Qt 6 Core

On Ubuntu, install the build dependencies with:

```sh
sudo apt-get install build-essential cmake qtbase5-dev
```

Use `qt6-base-dev` instead to build with Qt 6.

## Build

```sh
git clone https://github.com/VahidKz/hardware-info.git
cd hardware-info
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Run the CLI:

```sh
./build/hardware-info
./build/hardware-info --json
```

Example JSON shape:

```json
{
  "cpuId": "000806F8BFEBFBFF",
  "diskId": "example-disk-serial",
  "machineId": "64-character-sha256-hex-digest"
}
```

Availability depends on the hardware, kernel, virtualization layer, and
permissions. An unavailable identifier is returned as an empty string and is
shown as `unavailable` by the text CLI.

## Library usage

Add the project with CMake:

```cmake
add_subdirectory(hardware-info)
target_link_libraries(your-app PRIVATE HardwareInfo::HardwareInfo)
```

Then call the static API:

```cpp
#include <HardwareInfo.h>

const QString cpuId = HardwareInfo::getCpuId();
const QString diskId = HardwareInfo::getDiskId();
const QString machineId = HardwareInfo::getMachineId();
```

The project also installs a CMake package, so installed copies can be consumed
with `find_package(HardwareInfo CONFIG REQUIRED)`.

## Identifier semantics

These values are useful for diagnostics and best-effort device recognition, not
for authentication or licensing enforcement. CPU signatures are not guaranteed
to be unique, disk serials may be hidden or changed by virtual machines, and
hardware replacement changes the machine fingerprint.

Treat all hardware identifiers as sensitive data. Collect them only with user
consent and according to the privacy laws that apply to your application.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for the local development workflow and
pull request guidelines.

## License

MIT
