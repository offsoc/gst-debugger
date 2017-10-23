# gst-debugger

gst-debugger toolset allows to introspect gst-pipeline remotely. It provides graphical client, and GStreamer's plugin. 

## Build
1. Download the sources:

  a) latest git-master version:
   ```shell
   $ git clone git://git.gnome.org.com/gst-debugger
   $ cd gst-debugger
   ```
  b) latest release: [sources](https://ftp.gnome.org/pub/gnome/sources/gst-debugger/) 

2. Compile the project
```shell
$ meson build
$ cd build
$ ninja
$ ninja install
```

## Documentation
TBD, I promisse. Until then, feel free to mail me with any question you have (you can find my e-mail in the commit log :)
You can also find "Running the debugger" section useful.

## Running the debugger
The toolset consists of the rich client, and debugserver. debugserver is implemented as a tracer plugin, and has to be loaded with your pipeline, e.g.:
```shell
$ GST_TRACERS="debugserver(port=8080)" gst-launch-1.0 videotestsrc ! autovideosink
```

Now you can use a debugging client to connect to the debugger and inspect your pipeline:
```shell
$ gst-debugger-1.0
```

Happy debugging!