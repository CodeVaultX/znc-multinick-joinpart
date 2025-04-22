---

🔁 ZNC Multi Join/Part All Module – Control All Nicks in One Command

A ZNC module that allows you to join or part all your connected nicks in a given network to/from a specific IRC channel with a single command.

This module is especially useful for bouncers managing multiple nicks across a network and needing quick access to a channel.

---

## 📦 Files Included

- `multijoin.cpp` – Source code for the module.
- `multijoin.so` – Precompiled shared object for users who cannot compile ZNC modules manually.

---

## 🔧 Installation

### Option 1: Use Precompiled `.so` (no compilation required)

1. Download the `multijoin.so` file.
2. Place it into your ZNC modules directory, typically located at:
   ```
   ~/.znc/modules/
   ```   
   
   or
   
   ```
   /usr/local/lib/znc
   ```   

---

### Option 2: Build from source

If you prefer to compile it manually:

  ```bash
  znc-buildmod multijoin.cpp
  ```

This will generate `multijoin.so` in your current directory. Move it to your ZNC module directory and reload ZNC.

---

## 🚀 Usage
- You must load the module from ZNC first:
  ```
  /znc LoadMod multijoin
   ```
  or
  ```
  /msg *status LoadMod multijoin
  ```

You can control the module via commands: 
   ```
   /msg *multijoin help
   ```
 
---

### Commands

- `/msg *multijoin join #channel [network]`
  > Makes **all your connected nicks** join the specified channel on the given network.

- `/msg *multijoin part #channel [network]`
  > Makes **all your connected nicks** leave the specified channel on the given network.

- `/msg *multijoin list [network]`
  > Lists all active nicks under the specified (or current) network.

---

## 💬 Example

```
/msg *multijoin join #Online Rizon
/msg *multijoin part #Online Rizon
/msg *multijoin list Rizon


/msg *multijoin join #Online (If you don't specify a network, it will only operate on the network you're currently connected to)
/msg *multijoin part #Online (If you don't specify a network, it will only operate on the network you're currently connected to)
```

---


## 📃 License

Licensed under the [Apache License 2.0](https://github.com/CodeVaultX/znc-multinick-joinpart/blob/main/LICENSE).

---
