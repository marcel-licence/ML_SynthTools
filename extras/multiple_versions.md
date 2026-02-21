# Using Multiple ESP32 Board Versions in Arduino IDE (Windows)

Tested with: **Arduino IDE 2.3.5**\
Target: Use multiple versions of **Espressif's ESP32 Arduino core** side-by-side in the Arduino IDE.

---

## ⚒️ Step-by-step Instructions

### 1. 📂 Prepare Your Folder Structure

Create a `hardware` folder inside your Arduino sketchbook:

```text
C:\Users\%USERNAME%\Documents\Arduino\hardware
```

Inside that, create a subfolder for your desired version:

```text
C:\Users\%USERNAME%\Documents\Arduino\hardware\espressif_2.0.17
```

Clone the ESP32 Arduino core into the `esp32` subfolder:

```bash
cd %USERPROFILE%\Documents\Arduino\hardware\espressif_2.0.17

git clone https://github.com/espressif/arduino-esp32.git esp32

cd esp32

git checkout tags/2.0.17
```

This will download version 2.0.17 into the path:

```text
...\Arduino\hardware\espressif_2.0.17\esp32
```

### 2. 🔄 Restart Arduino IDE

After restarting the Arduino IDE, you will now see both board versions:

- The default one from Boards Manager
- Your custom version from the Sketchbook

Look for entries like:

- `ESP32 Dev Module`
- `ESP32 Dev Module (in Sketchbook) (espressif_2.0.17)`

![grafik](https://github.com/user-attachments/assets/5fa68133-cdfc-461a-acec-9a76942a4a9d)


---

### 3. ⚠️ Compatibility with Arduino IDE < 2.x

Older IDE versions may **not display version labels** in the board selection menu.

To make your custom version distinguishable:

1. Edit this file:

   ```text
   ...\Arduino\hardware\espressif_2.0.17\esp32\platform.txt
   ```

2. Add or modify the `name` field at the top:

   ```ini
   name=ESP32 Arduino (in Sketchbook) (espressif_2.0.17)
   ```

This ensures proper identification in older IDEs.

---

## ✅ Benefits

- Use different versions of ESP32 platform for different projects
- Test compatibility with older/newer SDKs
- Avoid breaking changes from updates

---

## 💻 Optional: One-liner Script for Setup

```bash
cd %USERPROFILE%\Documents\Arduino\hardware

mkdir espressif_2.0.17 && cd espressif_2.0.17

git clone https://github.com/espressif/arduino-esp32.git esp32

cd esp32 && git checkout tags/2.0.17
```
