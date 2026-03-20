---
title: "Configuration"
permalink: /docs/config
has_children: true
nav_order: 3
---

# Configuration

This section of the documentation contains information about the configuration files that are used to configure the firmware and webinterface.

Configuration files can either be edited on your computer and uploaded to the device via PlatformIO (`Upload filesystem image`) or directly on the machine. To edit a file directly on the Clackotron 2000, hold the mode button for >5s until the LED just above it lights up. Afterwards, you can access `http://clackotronip:8080/edit?file=...` where `ip` is the ip address of your device (e.g. `192.168.1.23)` and `...` is the file you want to edit (e.g. `modules`).

After editing and saving a file on the device, you must restart the device before changes are applied.
