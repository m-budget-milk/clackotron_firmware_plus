---
title: "Webinterface & API"
permalink: /docs/misc-api-docs
parent: "Additional documentation"
nav_order: 4
---

# Webinterface & API
## Normal mode
By default, whenever the device is connected to a WiFi network, it will be reachable via its IP address, as it runs a webserver on the default port `:80`. Accessing the device using a browser, (e.g. via `http://192.168.1.123`) will redirect the user to `/ui/index.html` which will serve him the graphical webinterface.

The routes published on the normal webserver are:

Path | Description
--- | ---
`/` | Redirects to `/ui/index.html`
`/config` | Get the current config as JSON.
`/ui/index.html`| Get the graphical webinterface.
`/ui/config/modules.json` | Get read-only view of the module config.
`/ui/config/timezone.json` | Get read-only view of the timezone config.
`/ui/config/webinterface.json` | Get read-only view of the webinterface config.

## Config mode
Whenever the mode button is pressed for >5 seconds, the LED above the button lights up and an additional webserver is started on port `:8080`. This mode allows for more advanced configuration of the device.

The routes published on this webserver are:

Path | Description
--- | ---
`/edit?file=...` | A read-write editor view of a config file. E.g. `/edit?file=modules` allows the user to edit the `modules.json` config file. After editing config files, a restart of the device is necessary.
`/rewrite-address?from=...&to=...` | Sends a protocol to all connected modules to rewrite the module with address `from` to `to`. E.g. `/rewrite-address?from=1&to=2` will rewrite the address of all connected modules with address `1` to `2`.
`/reset` | Clear all settings and reset configs to factory default.
`/reboot` | Reboot the device.
