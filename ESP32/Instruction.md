# Instructions.md

## Introduction
This guide provides detailed instructions for setting up and compiling code for the ESP32, specifically for the AZ-Delivery DevKit V4 board, using the Arduino framework.

## Requirements
- PlatformIO (or any suitable ESP32 development environment that can understand the provided `platformio.ini` details)
- ESP32 development board (specifically AZ-Delivery DevKit V4)
- A computer with a suitable operating system (Windows, macOS, or Linux)

## Setup Instructions
### Installing PlatformIO:
1. **PlatformIO IDE on VSCode (Recommended)**:
   - Download and install Visual Studio Code (VSCode) from [here](https://code.visualstudio.com/).
   - Launch VSCode.
   - Open Extensions from the side menu (or press `Ctrl+Shift+X`).
   - Search for "PlatformIO" and install the PlatformIO IDE extension.

2. **PlatformIO Core (Command Line)**:
   - Install PlatformIO Core using the [official installation guide](https://docs.platformio.org/en/latest/core/installation.html).

### Preparing Your Project:
1. **Create a New Folder** for your project:

   ```bash
   mkdir your_project_name
   cd your_project_name
   ```

   Replace `your_project_name` with your preferred project name.

2. **Create a platformio.ini File**:

   Inside your project folder, create a file named `platformio.ini` and add the following content:

   ```ini
   [env:az-delivery-devkit-v4]
   platform = espressif32
   board = az-delivery-devkit-v4
   framework = arduino
   ```

3. **Add Your Source Code**:

   - Create a `src` directory inside your project folder:

     ```bash
     mkdir src
     ```

   - Place the provided ESP32 code into a file inside the `src` directory, for instance `main.cpp`.

### Compiling and Uploading:
1. **Open the Project**:

   If using VSCode with PlatformIO IDE:
   - Launch VSCode.
   - Open your project folder using `File -> Open Folder`.
   - You should see your project structure on the left and the PlatformIO Home on the right.

   If using PlatformIO Core:
   - Navigate to your project folder in the terminal.

2. **Compile the Code**:

   - With VSCode: Click on the checkmark icon (✓) on the bottom toolbar (or use the PlatformIO: Build option from the PlatformIO menu).
   - With PlatformIO Core: Run the command:

     ```bash
     platformio run
     ```

3. **Upload to ESP32**:

   - Connect your ESP32 board to your computer via USB.
   - With VSCode: Click on the right arrow icon (➔) on the bottom toolbar (or use the PlatformIO: Upload option from the PlatformIO menu).
   - With PlatformIO Core: Run the command:

     ```bash
     platformio run --target upload
     ```

   Wait for the upload to complete. Once done, your ESP32 board should be running the uploaded code.

## Conclusion
You've successfully set up, compiled, and uploaded code to your ESP32 board using either the PlatformIO IDE on VSCode or the PlatformIO Core command line tool. You can now expand your project, add libraries, or develop more complex applications for your ESP32 board.