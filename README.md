# FastBin 🗑️

**FastBin** is a super-lightweight and ultra-fast Windows system tray utility written in **pure C (WinAPI)** that allows you to:

- ✅ Open the Recycle Bin
- 🧹 Empty the Recycle Bin (with confirmation)
- ⚡ Sit silently in the system tray with **under 1 MB of RAM usage**

No .NET, no external libraries, no dependencies. Just native Win32 performance.

---

## ✨ Features

- 🧠 **Ultra-low memory usage** (~400–800 KB)
- ⚙️ Written entirely in **C** using **WinAPI**
- 📁 Open Recycle Bin with a single click
- ❌ Empty Recycle Bin with confirmation
- 💻 Tray icon menu with clean structure
- 🔒 No background services, no bloat
- 🧩 Easy to modify and extend

---

## 📸 Screenshot

![screenshot](screenshot.png)

---

## 🛠️ Building FastBin

### 🔧 Requirements:
- A C compiler (e.g. [MinGW](https://www.mingw-w64.org/), MSVC, TDM-GCC)
- Windows 7 or newer

### 📦 Compile with GCC (MinGW):
```bash
gcc fastbin.c -o fastbin.exe -mwindows -Os -s
```

### 🔧 Usage
Once compiled:

- Run fastbin.exe

- A tray icon will appear.

- Right-click the icon to:

- Open the Recycle Bin

- Empty the Recycle Bin (confirmation prompt)

- Exit the app

FastBin does not stay in memory unnecessarily and exits cleanly.

### 🙏 Credits
FastBin was inspired by SimpleBin by @exalaolir, a lightweight .NET tool with similar goals.
FastBin is a full rewrite in C focused on native performance and minimalism.