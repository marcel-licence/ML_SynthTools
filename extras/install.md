# ML_SynthTools Installation Guide

This document outlines the steps required to install the **ML_SynthTools** library on different platforms, including **Windows**, **Ubuntu**, and **PlatformIO**.

---

## Table of Contents
1. [Installation on Arduino IDE (Windows)](#installation-on-arduino-ide-windows)
2. [Installation on Arduino IDE (Ubuntu)](#installation-on-arduino-ide-ubuntu)
3. [Installation via PlatformIO](#installation-via-platformio)
4. [Troubleshooting](#troubleshooting)

---

## Installation on Arduino IDE (Windows)

### Step 1: Download and Install Arduino IDE
- Visit the [Arduino website](https://www.arduino.cc/en/software) and download the latest version of the Arduino IDE.
- Follow the instructions to install it on your Windows system.

### Step 2: Download ML_SynthTools Library
- Navigate to the **ML_SynthTools** GitHub repository: [ML_SynthTools](https://github.com/marcel-licence/ML_SynthTools).
- Click the green **Code** button and select **Download ZIP**.

### Step 3: Install the Library in Arduino IDE
- Open the Arduino IDE.
- Go to `Sketch` -> `Include Library` -> `Add .ZIP Library...`.
- Browse and select the **ML_SynthTools** ZIP file you just downloaded.
- The library will be installed automatically.

### Step 4: Verify the Installation
- Go to `Sketch` -> `Include Library`. You should see **ML_SynthTools** listed under **Contributed Libraries**.

### Step 5: Install **ML_SynthTools_Lib**
- Navigate to the **ML_SynthTools_Lib** GitHub repository: [ML_SynthTools_Lib](https://github.com/marcel-licence/ML_SynthTools_Lib)
  and follow the instructions

---

## Installation on Arduino IDE (Ubuntu)

### Step 1: Install Arduino IDE
- Open a terminal and run the following commands to install Arduino IDE:

  ```bash
  sudo apt update
  sudo apt install arduino

### Step 2: Download ML_SynthTools Library

You can either clone the repository or download the ZIP file:

- To clone the **ML_SynthTools** repository using Git, open a terminal and run the following command:

```bash
git clone https://github.com/marcel-licence/ML_SynthTools.git
```

- Alternatively, download the ZIP from the GitHub page and extract it.

### Step 3: Install the Library in Arduino IDE

- Copy the **ML_SynthTools** folder to your Arduino libraries directory:

```bash
cp -r ML_SynthTools ~/Arduino/libraries/
```

### Step 4: Verify the Installation
- Go to `Sketch` -> `Include Library`. You should see **ML_SynthTools** listed under **Contributed Libraries**.

### Step 5: Install **ML_SynthTools_Lib**
- Navigate to the **ML_SynthTools_Lib** GitHub repository: [ML_SynthTools_Lib](https://github.com/marcel-licence/ML_SynthTools_Lib)
  and follow the instructions

---

## Installation via PlatformIO

todo

## Troubleshooting

todo

