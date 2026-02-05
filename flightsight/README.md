# flightsight

This repository contains a minimal Docker image and setup to run `readsb` (ADS-B receiver software) with RTL-SDR support.

Important files
- `dockerfile` — builds a Debian-slim image, installs `readsb`, builds `rtl-sdr` packages and configures `supervisord` to run `readsb`.

Quick goals
- Build an image that installs `readsb` and `rtl-sdr`.
- Run `readsb` inside a container supervised by `supervisord`.
- Provide instructions for passing RTL-SDR USB hardware into the container.

Build
1. Build the Docker image (run from repository root):

```bash
docker build -t flightsight-readsb -f dockerfile .
```

Run (recommended for hardware access)
- If you want the container to access host USB RTL-SDR devices, run with either `--privileged` or map the specific `/dev/bus/usb` device node.

Examples
- Run with full USB access (easiest):

```bash
docker run --rm -it --privileged --network host flightsight-readsb
```

- Run with a single device node (more secure):

1. On the host find the device with `lsusb` and list nodes:

```bash
lsusb
ls -l /dev/bus/usb/$(printf "%03d" $BUS)/$(printf "%03d" $DEVICE)
```

2. Start container mapping that device (example path):

```bash
docker run --rm -it --device /dev/bus/usb/001/004 --network host flightsight-readsb
```

Notes on hardware and permissions
- Accessing USB RTL-SDR requires host udev rules or running the container with `--privileged`. Mapping a specific `/dev/bus/usb/<bus>/<dev>` node works when you have the exact device node.
- On macOS hosts you cannot pass-through USB devices directly into Linux containers; use a Linux host or a VM.

Service and logs
- `readsb` is run under `supervisord` in the container; the `supervisord` configuration is created during image build in the `dockerfile` (see the `readsb --net --lat ...` invocation).
- Logs are written to `/var/log/readsb/readsb.log` and `/var/log/readsb/readsb.err` inside the container. To view them from the host:

```bash
docker ps                              # get container id
docker exec -it <container> tail -f /var/log/readsb/readsb.log
```

Setting/Changing Location
- The Dockerfile attempts to run `readsb-set-location` at build-time. To change location at runtime:

```bash
docker exec -it <container> readsb-set-location 50.12344 10.23429
```