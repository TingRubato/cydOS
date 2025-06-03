# 🌟 cydOS

cydOS is a versatile and user-friendly graphical user interface (GUI) designed for the ESP32-2432S028R device (CYD). It aggregates various applications and standalone functionalities, offering a comprehensive environment for managing and running apps directly from an SD card.

---

## 📑 Table of Contents

- [🌟 cydOS](#-cydos)
  - [📑 Table of Contents](#-table-of-contents)
  - [🚀 Features](#-features)
  - [🖼️ Screenshots](#️-screenshots)
  - [🛠️ Installation](#️-installation)
    - [Prerequisites](#prerequisites)
    - [Steps](#steps)
  - [▶️ Usage](#️-usage)
  - [📁 Directory Structure](#-directory-structure)
  - [🤝 Contributing](#-contributing)
  - [📄 License](#-license)
  - [📬 Contact](#-contact)

---

## 🚀 Features

- **🖱️ Touchscreen Interface:**  
  Responsive, finger-friendly UI using the XPT2046 touchscreen controller.

- **🏠 Home Screen:**  
  Central hub for quick access to settings and app launcher, with an intuitive navigation experience.

- **💾 Application Flasher:**  
  Scans the SD card for binaries in the `apps` directory and allows easy flashing.

- **⚙ Settings:**  
  Adjustable system configurations, designed for easy extensibility.

- **📂 File Browser:**  
  Browse the SD card and create new directories directly from the UI.

---

## 🖼️ Screenshots

<!-- Add screenshots or GIFs here if available -->
<!-- ![Home Screen](diagrams/home_screen.png) -->

---

## 🛠️ Installation

> **Note:** cydOS is currently in early development and may be unstable.

### Prerequisites

- ESP32-2432S028R device (CYD)
- [VSCode](https://code.visualstudio.com/) with [PlatformIO](https://platformio.org/) extension
- SD card (formatted as FAT32)

### Steps

1. **Clone the repository:**
   ```sh
   git clone https://github.com/yourusername/cydOS.git
   cd cydOS
   ```

2. **Open in VSCode:**  
   Open the project folder in VSCode.

3. **Configure PlatformIO:**  
   Ensure your `platformio.ini` is set for the ESP32-2432S028R.

4. **Connect your device:**  
   Plug in the ESP32-2432S028R via USB.

5. **Build and upload:**  
   Use PlatformIO to build and upload the firmware.

6. **Prepare SD card:**  
   - Create an `apps` directory on the SD card.
   - Place your application binaries in `apps/`.
   - Insert the SD card into the device.

---

## ▶️ Usage

- On boot, cydOS will display the home screen.
- Use the touchscreen to navigate between the home screen, settings, and file browser.
- Select and flash applications from the SD card via the Application Flasher.

---

## 📁 Directory Structure

```
cydOS/
├── src/                # Source code
├── include/            # Header files
├── data/               # Configuration and data files
├── diagrams/           # UI diagrams and images
├── boards/             # Board definitions
├── test/               # Test code
├── platformio.ini      # PlatformIO project configuration
├── sdkconfig           # ESP-IDF configuration
├── partitions.csv      # Partition table
└── README.md           # Project documentation
```

---

## 🤝 Contributing

Contributions are welcome! To get started:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature/your-feature`).
3. Commit your changes.
4. Push to your fork and submit a pull request.

Please see [CONTRIBUTING.md](CONTRIBUTING.md) for more details (create this file if needed).

---

## 📄 License

This project is licensed under the terms of the [MIT License](LICENSE).

---

## 📬 Contact

For questions, suggestions, or support, please open an issue or contact [your.email@example.com](mailto:your.email@example.com).

---
