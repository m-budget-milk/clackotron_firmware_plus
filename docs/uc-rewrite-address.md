---
title: "Rewrite Module Address"
permalink: /docs/uc-rewrite-address
parent: "Specific use cases"
nav_order: 5
---

# Rewrite module address
If you have two or more modules with the same address, you will most likely want to re-program them, so you can use them together. To do so:

1. Disconnect all but one modules with the same serial number.
2. Hold the mode button on the Clackotron 2000 for >5s until the LED above it lights up
3. Navigate to `/rewrite-address?from=...&to=...` in your browser where `from=...` is the current address of the module and `to=` is the new address you want to give the module.
4. Note the new address on the module with a pen

So for example, if you have multiple modules with the address `32`, connect one of them, enable config mode, and open `http://clackotronip:8080/rewrite-address?from=32&to=12` to rewrite its address to `12`.
