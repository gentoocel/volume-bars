# Volume Bars

Volume Bars is a lightweight volume control program for Linux that integrates with PulseAudio or PipeWire. It provides a simple interface for adjusting audio volume and works well with various window managers.

Features

- Volume Control: Adjust the volume using +, -, or arrow keys.
- Mute/Unmute: Toggle mute with the m key.
- Exit: Close the program with the ESC key or by clicking outside the window.
- Auto Exit: Automatically exits when the window loses focus.

Dependencies

To build Volume Bars, you need:

- GTK+ 3: For the graphical user interface.
- PulseAudio: Required if you use PulseAudio for sound control.
- PipeWire: Required if you use PipeWire for sound control.

Install these dependencies using your package manager. For example:

- Debian/Ubuntu:
  sudo apt-get install libgtk-3-dev libpulse-dev libpipewire-0.3-dev

- Fedora/RHEL:
  sudo dnf install gtk3-devel pulseaudio-libs-devel pipewire-devel

Installation

1. Clone the Repository:
   git clone https://github.com/gentoocel/volume-bars.git

2. Navigate to the Project Directory:
   cd volume-bars

3. Compile the Source Code:
   gcc `pkg-config --cflags gtk+-3.0` -o volume-bars volume-bars.c `pkg-config --libs gtk+-3.0`

4. Run the Program:
   ./volume-bars

Usage

- Increase Volume: Press the + key or the right arrow key.
- Decrease Volume: Press the - key or the left arrow key.
- Mute/Unmute: Press the m key.
- Exit: Press the ESC key or click outside the window.
