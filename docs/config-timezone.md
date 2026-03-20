---
title: "Config: timezone.json"
permalink: /docs/config-timezone
parent: "Configuration"
nav_order: 3
---

# Configuration: `timezone.json`
The `timezone.json` file contains the configuration that informs Clackotron 2000 about the current timezone, daylight savings time changes, as well as the ntp server to sync time with and the day names to display.

The `timezone.json` file can be edited on your computer and uploaded as part of the filesystem via PlatformIO or directly on the device by enabling the config mode and opening `http://clackotronip:8080/edit?file=timezone`. When editing directly on the device, you must manually reboot the device after editing.

## File format
The `timezone.json` file contains a JSON object with multiple keys:
* `time_normal` - Timezone configuration array without daylight savings time.
* `time_dst` - Timezone configuration array with daylight savings time.
* `days_short` - Array of 2-letter day name strings.
* `days_long` - Array of 3-letter day name strings.
* `ntp_server` - The ntp server to sync time with.

## Default
```json
{
    "time_normal": [0, 1, 3, 2, 120],
    "time_dst": [0, 1, 10, 3, 60],
    "days_short": [ "  ", "SO", "MO", "DI", "MI", "DO", "FR", "SA" ],
    "days_long": [ "   ", "SON", "MON", "DIE", "MIT", "DON", "FRE", "SAM" ],
    "ntp_server": "ch.pool.ntp.org"
}
```

## Examples
### Time configurations
The `time_normal` and `time_dst` settings expect raw configuration values for the [JChristensen/Timezone](https://github.com/JChristensen/Timezone) library. The format of each settings array is `[weekday_type, day_of_week, month, hour, offset]`.

Parameter | Description
--- | ---
`weekday_type` | `Last=0, First=1, Second=2, Third=3, Fourth=4`
`day_of_week` | `Sun=1, Mon=2, Tue=3, Wed=4, Thu=5, Fri=6, Sat=7`
`month` | `Jan=1, Feb=2, Mar=3, Apr=4, May=5, Jun=6, Jul=7, Aug=8, Sep=9, Oct=10, Nov=11, Dec=12`
`hour` | `0 - 23`
`offset` | Offset from UTC in minutes

So the default configurations mean:
```
[0, 1, 3, 2, 120] -> [Last, Sun, Mar, 2, 120]
Last Sunday of March change the offset to UTC+120min at 2:00

[0, 1, 10, 3, 60] -> [Last, Sun, Oct, 3, 60]
Last Sunday of October change the offset to UTC+60min at 3:00
```

Which are the UTC+1/UTC+2 rules for daylight savings time in Switzerland. For more explanation on the rules, check out the [JChristensen/Timezone](https://github.com/JChristensen/Timezone) library.

### Day name configurations
The `days_short` and `days_long` arrays are used for writing the current weekday on the hardware. Index zero of the arrays is unused and Sunday is the first day at index one.

The default configuration uses german names:
```
"days_short": [ "  ", "SO", "MO", "DI", "MI", "DO", "FR", "SA" ],
"days_long": [ "   ", "SON", "MON", "DIE", "MIT", "DON", "FRE", "SAM"]
```

An example of using english names:

```
"days_short": [ "  ", "SU", "MO", "TU", "WE", "TH", "FR", "SA" ],
"days_long": [ "   ", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"] 
```
