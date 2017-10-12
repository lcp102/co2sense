### Simple application configuration from files.
***

When you are done building the first milestone of your embedded application on Pi , you ought to take a hard look at those Macros in the `#define` section to come out with the list of variables that can be then set from the user interface. Loops for typical programs on the embedded are continuous with an eye on the configuration. A external stimulus to change the configuration can trigger the loop to pause and reload the configuration.

Here are a few things you may want to consider before letting your program open to external config changes.
1. Type of the files you may think are suitable. This choice could largely be of convenience too. I have seen a lot of upvotes for YAML files but for now atleast I would like to go for a simple text file with identifiable delimiters.
2. A non interrupting service that keeps a watch on the user inputs. This service would either notify the loop of restart or minimally about the configuration changes. Here is a suggestion of what you can do [My other blog](https://pifarm.github.io/co2sense/upstart)
3. Hardware peripherals attached to the processor , would inturn help the user to key the inputs.

### System calls and why should they be used minimally
****

Analogous to the windows API - you have to make system calls to open, read, write, close the files & directories. But there exsists a whole range of library functions that also make this possible , or rather more efficient.
In Linux (or Unix) almost everything is a file - so programs can use disk files, serial ports, and other devices exactly the same way they would use a file. - there are some exceptions like the network connections, but for rest everything is under the five operations :

1. open
2. close
3. read
4. write
5. ioctl

- Even directories are files, special files. Even the super user will not be able to write to them directly. All the users may be able to do is `readdir / opendir`, without knowing the system level details. 

### file descriptors


### `Open( )` and new file descriptors
