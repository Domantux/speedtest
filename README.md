# speedtest

A command-line internet speed test tool written in C. Measures download speed, upload speed, and latency using the Speedtest.net server network.

## Features

- Auto-detects your location and selects the nearest server
- Measures download and upload speed with a live progress bar
- Measures latency (ping)
- Large bundled server list with worldwide coverage

## Dependencies

- [libcurl](https://curl.se/libcurl/)
- [cJSON](https://github.com/DaveGamble/cJSON)

On Debian/Ubuntu:
```sh
sudo apt install libcurl4-openssl-dev libcjson-dev
```

## Build

```sh
make
```

Install system-wide:
```sh
sudo make install
```

Uninstall:
```sh
sudo make uninstall
```

## Usage

```
speedtest [-s <id>] [-d] [-u] [-b] [-l] [-h] [-v]
```

| Flag | Description |
|------|-------------|
| (none) | Auto test: detect location, pick best server, run download and upload |
| `-s <id>` | Select a specific server by ID |
| `-d` | Download speed test (requires `-s`) |
| `-u` | Upload speed test (requires `-s`) |
| `-b` | Print the best server for your location |
| `-l` | Print your current location |
| `-h` | Show help |
| `-v` | Show version |

## Examples

Run a full auto test:
```sh
./speedtest
```

Find the best server for your location:
```sh
./speedtest -b
```

Run download and upload test on a specific server:
```sh
./speedtest -s 1234 -d -u
```

## Sample Output

```
Location: Kaunas, Lithuania
Server: Litnet (speedtest.litnet.lt:8080)
Latency: 39.34 ms
Download test has started. Please wait
[====================] 15/15s  342.1 Mbps
Download speed: 342.10 Mbps
Upload test has started. Please wait
[====================] 15s  95.3 Mbps
Upload speed: 95.30 Mbps
```
