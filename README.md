# Golioth + Memfault Example

## Overview

This application demonstrates how to use the Golioth Firmware SDK and Golioth
Pipelines to send observability data from the Memfault SDK to Memfault's cloud.

## Pipelines Setup

This documentation assumes you already have accounts with both Golioth and
Memfault.

## Application Setup

This is a Zephyr application managed and built using `west`. To setup your
environment for building Zephyr apps, follow the [Getting Started guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

Then you can setup your workspace to build this application by running one of
the following:

For vanilla Zephyr:
```
west init -m https://github.com/golioth/golioth-memfault-zephyr-example.git
```

For nRF Connect SDK (NCS):
```
west init -m https://github.com/golioth/golioth-memfault-zephyr-example.git --mf west-ncs.yml
```

To build the application (see below for a list of supported boards):

```
west build -p -b <board_name> app
```

You'll need to configure a pipeline in the Golioth Console in order to route
data to Memfault. See [the documentation](https://docs.golioth.io/data-routing/destinations/memfault/)
for instructions.

### Supported Boards

| Board Name     | OS     |
|----------------|--------|
| MIMXRT1024-EVK | Zephyr |

## Running the Application

First flash the application to the board:

```
west flash
```

Next, set Golioth credentials through the serial terminal:

```
settings set golioth/psk-id <your_psk_id>
settings set golioth/psk <your_psk>
```

The application is configured to capture Memfault heartbeat metrics once per
hour. Also once per hour, the application will upload that data to Golioth,
where it will be routed to Memfault.

### Memfault CLI

To test the application, you can use the `mflt` menu in the serial console. E.g.:

To trigger a coredump via a hardfault:
```
mflt test hardfault
```

To force an immediate heartbeat recording:
```
mflt test heartbeat
```

### Golioth RPCs

The application registers several RPCs with Golioth that can be used for
testing and debugging.

- `coredump` - This will remotely trigger an assert on the device to force
collection of a coredump.
- `memfault_upload` - This will remotely trigger an immediate upload of
Memfault data.
