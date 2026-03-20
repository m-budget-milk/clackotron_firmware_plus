---
title: "Template Syntax"
permalink: /docs/misc-templates
parent: "Additional documentation"
nav_order: 4
---

# Template Syntax
Behind the scenes, when a user selects a specific mode, its template is stored and used. This allows a very complex and dynamic configuration while keeping things simple for the end user. The default configuration provided in this repository should be sufficient for most use cases.

Templates use the notation `{parameter}` or `{parameter:size}`
The following template parameters are possible:

* `{YYYY}` - The current year in 4 digits (e.g. `2022`)
* `{MM}` - The current month in 2 digits (e.g. `08`)
* `{DD}` - The current day in 2 digits (e.g. `15`)
* `{WD}` - The current week day in 2 letters (e.g. `MO`)
* `{WDD}` - The current week day in 3 letters (e.g. `MON`)
* `{hh}` - The current hour in 2 digits (e.g. `10`)
* `{mm}` - The current minute in 2 digits (e.g. `12`)
* `{ss}` - The current second in 2 digits (e.g. `12`)
* For variants of these for hour and minute modules, see [Module Suffixes](#module-suffixes)

{: .watchout}
The `{ss}` parameter is supported but not recommended. The split-flap modules are not fast enough to update every second, especially not on rollover.

To allow for configurable modes, the following parameters are also supported:
* `{tx:n}` - A custom text input, where `x` is a number from 0-9 and `n` is the length of the text.
  * For example: `t1:4` will show the user a text input where he can enter 4 characters.
* `{ix:n}` - A custom number input, where `x` is a number from 0-9 and `n` is how often it is repeated.
  * For example: `i1:2` will show the user a number input where he can enter a raw value that is sent to the two modules at this position. This can be used for controlling modules that have fixed station names on them by using the index of the desired sheet.

Every character in a template that is not in curly braces is simply printed as-is. The following examples should make the use of the template system more clear:
* `{DD}{MM}` - Will show the current day and the current month, e.g. `1208`
* `{hh}{mm}` - Will show the current hour and minute, e.g. `1012`
* `{hh}.{mm}` - Will show the current hour and minute, separated by a dot, e.g. `10.12`
* `{t1:4}` - Will give the user one text field to enter 4 letters, e.g `HOME`
* `LOVE`  Will simply be printed as-is, e.g. `LOVE`
* `{WD} {DD}.{MM}.{YYYY} {hh}.{mm}` - Will show e.g. `MO 01.01.2000 01:23`
* `{t1:6}{t2:6}{i1:1}` - This complex example will give the user two text inputs that each take 6 characters and one number input that is repeated once. This could be used on a device that has two rows of 6 letters and a module with different stations below to show: 

```
HELLO
WORLD
Zürich Hauptbahnhof
```

If a template results in a value that is longer than the amount of aviable modues, the overlapping part is simply cut off. For example, attempting to display `{hh}.{mm}` on a device with 4 letter modules, will show `12.2` instead of `12.23`.

## Module Suffixes
The generic templates listed above such as `{DD}` or `{YYYY}` work with alphanumeric modules. This means, `{YYYY}` will be expanded into four digits (e.g. `2023`) which will be sent to four different modules.

The original set of moduels includes special modules for hours and minutes which have special flaps for `00-23` and `00-59` respectively. To use these, there is a set of special suffix templates that are listed below. Using these allows sending information as one value directly to an hour or minute module. (E.g. `{DD}` will not be sent as `12` to two modules but will be sent as `<the flap value for 12>` directly to one module).

For hour modules (can only display `0.` - `23.`):
* `{YY_h}` - The current year in 2 digits (e.g. `22`)
* `{MM_h}` - The current month in 2 digits (e.g. `08`)
* `{DD_h}` - The current day in 2 digits (e.g. `15`)
* `{hh_h}` - The current hour in 2 digits (e.g. `01`)
* `{mm_h}` - The current minute in 2 digits (e.g. `12`)
* `{ss_h}` - The current seconds in 2 digits (e.g. `21`)

For minute modules (can only display `0` - `59`):
* `{YY_m}` - The current year in 2 digits (e.g. `22`)
* `{MM_m}` - The current month in 2 digits (e.g. `08`)
* `{DD_m}` - The current day in 2 digits (e.g. `31`)
* `{hh_m}` - The current hour in 2 digits (e.g. `20`)
* `{mm_m}` - The current minute in 2 digits (e.g. `47`)
* `{ss_m}` - The current seconds in 2 digits (e.g. `53`)

{: .watchout}
Due to the nature of these modules, not all possible values can be properly displayed!
