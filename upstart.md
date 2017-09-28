### The pain of carrying Pi peripherals.

I had to, on multiple occassions carry a VGA / HD screen alongside a keyboard that can be attached to the Pi. Demos required at the workplace required me to carry the entire working prototype to locations at my work-campus. This started becoming even more cumbersome when the number of discrete electronics on the bread board started piling up. On each of occassions I have had to connect the monitor , keyboard and manually start the program on the Pi and then speak about it.

I kept wishing for a better way to do this. While I knew there had to be a professional way to get around this , just did not know how. What's funnier , I was always aware of upstarting Linux services on `systemd` , I never thought of applying the same.

> When you are into an idea , one tends to miss out on the width of the subject matter. Only when you have satisfactory results would you then try experiments with other moving parts.

### Upstarting services

Getting the desired program run at the start as a `systemd` service instantly gets rid of the need to carry around the peripherals. One can turn on the Pi and expect things to crank up just as desired.
Pi would mostly fire systems that run infinite loops of sensing at regular intervals. -Atleast mojority of the projects that I worked on have this chracteristic. Upstarting services on `initd` can be tricky if they are blocking and are spawned at the wrong time. While `systemd` does not have sequential nature of upstarting, you can breathe a sigh of relief knowing the Raspbian Jessie uses `systemd`.

But for an one-off case here is how you can still mess up your Pi
**Don't try the steps below, this may lead to Pi being inaccessible over the network and hungup on the boot !!**
```bash
$ cat /etc/systemd/system/some.service
[Unit]
Description=This is to test a service that runs an infinite loop but also has restart interrupt
[Service]
Type=forking
ExecStart=path/to/exectuable
[Install]
WantedBy=multi-user.target

$ sudo systemctl enable some.service
$ sudo systemctl daemon-reload
$ sudo shutdown -r now
```
If you working from your laboratory and have setup ssh connections to your Pi, you have messed up you Pi. You will find you are unable to get into ssh - as it keeps timing out. You can try connecting a monitor, keyboard, mouse to the Pi. and see if you can access the terminal. But if not, time to flash the SD card and reload raspbian.

#### Making forking services with infinite loops
