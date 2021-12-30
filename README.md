netrate
=======

`netrate` is a simple program that displays real-time byte and packet
count rate of network interfaces in Linux systems.

The program relies on the `/proc/net/dev` kernel interface to retrieve
the network interface list and corresponding counters. For that reason,
it has very little overhead compared to other programs that capture the
traffic. Furthermore, it does not require special privileges and can run
as a regular user.

Interface counters are read and the display is updated every one second.
A backlog of three snapshots is kept, and the rates are calculated over
a moving window of three snapshots. Each snapshot is timestamped with
millisecond precision, since system load and process scheduling can add
variation to the one second delay between snapshots.

The measurement units are:
* kbytes/s for the byte rate (1 kbyte = 1024 bytes)
* kpackets/s for the packet rate (1 kpacket = 1000 packets)

The output format is text-only in a terminal. Unlike other programs that
display statistics in the terminal, `netrate` does not put the terminal
in any special mode. Only two special control characters are used, to
clear the contents and move the cursor back to the top-left corner.

The program does not trap signals or interpret keyboard input in any
way. Use `Ctrl-C` to exit the program.

## Author
Written by Radu Rendec.

## Copyright
Copyright 2010 Mindbit SRL  
Copyright 2021 Radu Rendec  
License:
[GPLv2: GNU GPL version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html).  
This is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.

## Reporting bugs
Open an issue in the
[official GitHub project](https://github.com/mindbit/netrate).
