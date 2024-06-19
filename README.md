# Golioth + Memfault Example

## Overview

This application demonstrates how to use the [Golioth Firmware
SDK](https://github.com/golioth/golioth-firmware-sdk) and [Golioth
Pipelines](https://docs.golioth.io/data-routing) to send observability data from
the [Memfault SDK](https://github.com/memfault/memfault-firmware-sdk) to
Memfault's cloud.

## Pipelines Setup

This documentation assumes you already have accounts with both
[Golioth](https://console.golioth.io/) and
[Memfault](https://app.memfault.com/). The [`pipelne.yml`](pipipeline.yml) file
in this repository contains the source for a pipeline that routes data on the
`/mflt` path to Memfault. Click the link below to configure it into your Golioth
project.

<a href='https://console.golioth.io/pipeline?name=Memfault&pipeline=ZmlsdGVyOgogIHBhdGg6ICIvbWZsdCIKICBjb250ZW50X3R5cGU6IGFwcGxpY2F0aW9uL29jdGV0LXN0cmVhbQpzdGVwczoKICAtIG5hbWU6IHN0ZXAwCiAgICBkZXN0aW5hdGlvbjoKICAgICAgdHlwZTogbWVtZmF1bHQKICAgICAgdmVyc2lvbjogdjEKICAgICAgcGFyYW1ldGVyczoKICAgICAgICBwcm9qZWN0X2tleTogJE1FTUZBVUxUX1BST0pFQ1RfS0VZ'>Create Pipeline</a>

> [!NOTE]  
> Make sure to also create a
> [secret](https://docs.golioth.io/data-routing/secrets/) named
> `MEMFAULT_PROJECT_KEY` with your Memfault [project
> key](https://docs.memfault.com/docs/platform/data-routes/)  to ensure that
> your pipeline is able to authenticate.

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

### Supported Boards

| Board Name     | OS     |
|----------------|--------|
| MIMXRT1024-EVK | Zephyr |
| nRF9160DK      | NCS    |

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
