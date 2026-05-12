# 25+5 Clock 🍅

A lightweight, production-grade Pomodoro timer written in C using GTK3. It features an automated full-screen transition overlay that requests attention when your work session ends, helping you commit to regular health-conscious screen breaks.

## Key Features

- **Automated Work-Break States:** Seamlessly alternates between custom productive slots and rest boundaries.
- **Attention Forcing Alerts:** Automatically scales to hardware layout bounds (Fullscreen Mode) and stays on top when timers trigger.
- **Embedded Static Resources:** Uses GResource caching layers to package the application CSS layout themes and vector SVG assets directly into a single portable binary block.
- **Desktop Environment Integration:** Fully features structured Freedesktop system launcher configurations (.desktop entries) and taskbar caching protocols under X11 & Wayland.
- **Native Hardware Audio:** Dispatches alarms straight into local desktop mixer engines via the libcanberra notification framework.

---

## Installation (Debian / Ubuntu and derivatives)

You can download the pre-compiled `.deb` architecture bundle straight from the [Releases](https://github.com) tab.

Install it directly via your terminal using your preferred system manager package utilities:

```bash
# Install via apt package utility (recommended - handles backend dependencies)
sudo apt install ./25plus5clock-1.0.0-alpha1-Linux.deb

# Alternative manual layout installation via dpkg
sudo dpkg -i 25plus5clock-1.0.0-alpha1-Linux.deb
sudo apt-get install -f # Fix potential missing system runtime prerequisites
```

Once installed, hit your system **Super / Windows Key** and search for **"25+5 Clock"** to launch the timer with its native system desktop application launcher.

---

## Building From Source

### Prerequisites

To compile or test this project locally on your machine, install the mandatory compiler toolchains and development core header packages:

```bash
sudo apt-get update
sudo apt-get install -y cmake build-essential ninja-build \
                        libgtk-3-dev libcanberra-gtk3-dev \
                        glib-2.0-dev
```

### Compilation Workflow

Execute a standard release configuration compilation directly within your terminal workspace:

```bash
# 1. Clone the project code repositories
git clone github.com
cd 25Plus5Clock

# 2. Establish isolated compilation scopes
mkdir build && cd build

# 3. Configure CMake with an optimized release optimization layer
cmake -DCMAKE_BUILD_TYPE=Release ..

# 4. Compile the source binaries and bind resource arrays
make -j$(nproc)
```

### Running Test Assertions

Validate calculations, countdown ticks, and transition states through the integrated automated testing suites:

```bash
# Trigger assertions via CTest engine
ctest --output-on-failure
```

### Packaging Target Distributions Local Assemble

To construct a standalone, distributed `.deb` package archive directly inside your current environment, invoke CPack:

```bash
cpack
```

---

## Project Structure

```text
├── CMakeLists.txt        # Comprehensive compilation targets & packaging properties
├── main.c                # GtkApplication entry initialization pipelines
├── icon.svg              # Scalable app vector logo asset
├── style.css             # Main stylesheet customization provider
├── resources.xml         # Asset packaging blueprint maps
├── include/              # Modular shared application headers
│   ├── app.h
│   ├── ui.h
│   └── callbacks.h
├── src/                  # Execution code architectures
│   ├── app.c             # Internal data structure generation
│   ├── ui.c              # Layout engine widgets positioning configurations
│   └── callbacks.c       # Timer loops handlers and system transitions behaviors
└── tests/                # Automated logic unit verification suites
    └── test_logic.c
```

---

## License

This software utility is maintained open source. Contribution pull requests, feature feedback issues, or bug tracking notes can be logged right in the repository tracking dashboards.
