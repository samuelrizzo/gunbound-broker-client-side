# GunBound Broker Client

A modern C++23 implementation of a GunBound game server broker that works as a DLL inside GunBound client.

## Overview

This project creates a shared library (DLL) that implements a TCP server to handle GunBound protocol communication. The server listens for client connections and processes game server list requests.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    GunBound Client                          │
│                                                             │
│  ┌─────────────────┐    ┌─────────────────────────────────┐ │
│  │   Game Client   │◄──►│       Broker DLL (this)        │ │
│  │                 │    │                                 │ │
│  └─────────────────┘    └─────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                                    │
                                    │ TCP Protocol
                                    ▼
                         ┌─────────────────────┐
                         │   TCP Server        │
                         │   (Port 8625)       │
                         └─────────────────────┘
                                    │
                                    │ Packet Processing
                                    ▼
                         ┌─────────────────────┐
                         │ Protocol Handler    │
                         │ • Server List       │
                         │ • Packet Parser     │
                         └─────────────────────┘
```

## Project Structure

```
gunbound-broker-client-side/
├── include/
│   ├── broker/
│   │   └── control.hpp          # DLL module control
│   ├── helpers/
│   │   ├── endian.hpp          # Endianness utilities
│   │   └── logs.hpp            # Debug logging
│   ├── protocol/
│   │   ├── handler.hpp         # Packet handler
│   │   ├── ids.hpp             # Protocol IDs
│   │   ├── packet.hpp          # Packet structure
│   │   └── server_options.hpp  # Server list options
│   └── tcp_server.hpp          # TCP server interface
├── src/
│   ├── broker/
│   │   └── control.cpp
│   ├── helpers/
│   │   ├── endian.cpp
│   │   └── logs.cpp
│   ├── protocol/
│   │   ├── handler.cpp
│   │   ├── packet.cpp
│   │   └── server_options.cpp
│   ├── main.cpp               # DLL entry point
│   └── tcp_server.cpp
├── CMakeLists.txt
└── build.ps1                 # Build script
```

## Key Components

### Protocol Layer

- **Packet**: Basic protocol structure with length, sequence, ID, and payload
- **Handler**: Processes incoming packets and generates responses
- **Server Options**: Manages game server list serialization

### Network Layer

- **TCP Server**: Handles client connections on port 8625
- **Protocol Parser**: Parses and validates incoming packets

### Utilities

- **Endian Helpers**: Little/big endian byte order utilities
- **Logging**: Debug output for packet analysis

## Building

Requirements:

- CMake 3.20+
- Visual Studio 2022 (C++23 support)
- Windows SDK

```powershell
# Quick build
.\build.ps1

# Manual build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

Output: `build/Release/broker.dll`

## Usage

1. Build the project to generate `broker.dll`
2. Inject the DLL into the GunBound client process
3. The DLL automatically starts a TCP server on port 8625
4. The server processes GunBound protocol packets

## Protocol Details

The broker implements the GunBound network protocol:

- **Packet Format**: `len(2) | seq(2) | id(2) | payload(len-6)`
- **Endianness**: Little-endian for most fields, big-endian for ports
- **Server List**: Serializes available game servers with metadata

## Development

The codebase follows modern C++ practices:

- C++23 standard
- Doxygen documentation
- RAII resource management
- Exception-safe design

## Author

**Rizzo** - [GitHub Repository](https://github.com/samuelrizzo/gunbound-broker-client-side)
