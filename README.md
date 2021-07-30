Installation
------------

## Introduction
Warning: The build system has been only tested on Windows 10 with WSL1, using the Windows file system. Other environments might work as compatibility measures were copied from [pokemon_dp](https://gitgud.io/gainax/pokemon_dp), but none have been tested and there is no guarantee that they will work. 

## Windows
First, create an environment variable called `LM_LICENSE_FILE` with its value as the planned path to the license file in the repository, at `/sdk/cw/license.dat`. For example, if you plan to store the repository at **C:\\Users\\_\<user>_\\Desktop\\retsam_00jupc**, where _\<user>_ is your Windows username, then the value of `LM_LICENSE_FILE` should be `C:\Users\<user>\Desktop\retsam_00jupc\sdk\cw\license.dat`.

To add an environment variable:
1. Search for "environment variables" in Windows 10's Start Search, and click the option that says "Edit the system environment variables".
2. In the window that opens, click the button that says "Environment Variables..." on the bottom right.
3. In the window that opens, click "New..." on the bottom right.
4. Input in the environment variable name and value, then click "OK".

You will need to restart your computer for the changes to take effect, but if you plan to use Windows Subsystem for Linux (WSL), then you can restart later as part of the required restart when installing WSL.

### Windows 10 (Windows Subsystem for Linux)

1. Open [Windows Powershell **as Administrator**](https://i.imgur.com/QKmVbP9.png), and run the following command (Right Click or Shift+Insert is paste in the Powershell).

    ```powershell
    dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
    ```

2. Once the process finishes, restart your machine.

3. The next step is to choose and install a Linux distribution from the Microsoft Store. The following instructions will assume Ubuntu as the Linux distribution of choice.

4. Open the [Microsoft Store Linux Selection](https://aka.ms/wslstore), click Ubuntu, then click Get, which will install the Ubuntu distribution.
    <details>
        <summary><i>Notes...</i></summary>

    >   Note 1: If a dialog pops up asking for you to sign into a Microsoft Account, then just close the dialog.  
    >   Note 2: If the link does not work, then open the Microsoft Store manually, and search for the Ubuntu app (choose the one with no version number).
    </details>

### Setting up WSL1
Some tips before proceeding:
- In WSL, Copy and Paste is either done via
    - **right-click** (selection + right click to Copy, right click with no selection to Paste)
    - **Ctrl+Shift+C/Ctrl+Shift+V** (enabled by right-clicking the title bar, going to Properties, then checking the checkbox next to "Use Ctrl+Shift+C/V as Copy/Paste").
- Some of the commands that you'll run will ask for your WSL password and/or confirmation to perform the stated action. This is to be expected, just enter your WSL password and/or the yes action when necessary.

1. Open **Ubuntu** (e.g. using Search).
2. WSL/Ubuntu will set up its own installation when it runs for the first time. Once WSL/Ubuntu finishes installing, it will ask for a username and password (to be input in).
    <details>
        <summary><i>Note...</i></summary>

    >   When typing in the password, there will be no visible response, but the terminal will still read in input.
    </details>

3. Update WSL/Ubuntu before continuing. Do this by running the following command. These commands will likely take a long time to finish:

    ```bash
    sudo apt update && sudo apt upgrade
    ```

4. Certain packages are required to build the repository. Install these packages by running the following command:

    ```bash
    sudo apt-get install git gcc make bison flex bzip2
    ```

5. Change to a directory accessible from Windows where you'll store the files, for example:
    ```
    cd /mnt/c/Users/$USER/Desktop
    ```

Continue with the [building instructions](#building)

### Windows (Cygwin)
Cygwin is not recommended. While not tested, it is believed to run extremely slowly compared to WSL1.

Get the installer ([64 bit](https://www.cygwin.com/setup-x86_64.exe) or [32 bit](https://www.cygwin.com/setup-x86.exe)). Grab the setup script ([64 bit](tools/cygwin_setup-x86.bat) or [32 bit](tools/cygwin_setup-x86_64.bat)).  
Place these files next to each other and run the batch file. Follow through the installation, there's no need to select any additional packages.

Change to a directory accessible from Windows where you'll store the files, for example:
```
cd /cygdrive/c/Users/$USER/Desktop
```

Continue with the [building instructions](#building)


## Linux

You need to install the following software:
* git
* gcc
* make
* bison
* flex
* wine (with 32-bit support!)

On Debian or Ubuntu you can do this by running:
```
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install git gcc make bison flex wine32
```

Continue with the [building instructions](#building)


## Building

Clone the repository:
```
git clone <repo url> retsam_00jupc
cd retsam_00jupc
```

To build everything:
```
./build_all.sh -j
```
Passing `-j` will cause `make` to run with multiple jobs (faster with multiple CPU cores), but is entirely optional.

If everything works, then the following ROM should be built:
- [bin/ARM9-TS/Rom/main.srl](https://datomatic.no-intro.org/index.php?page=show_record&s=28&n=3541) `sha1: ce81046eda7d232513069519cb2085349896dec7`

**Note for Windows users**: Consider disabling real-time protection using [these instructions](https://support.microsoft.com/en-us/windows/turn-off-defender-antivirus-protection-in-windows-security-99e6004f-c54c-8509-773c-a4d776b77960) to speed up builds. [Adding an exception](https://support.microsoft.com/help/4028485) for the repository folder does not seem to help as much; one hypothesis is that the exceptions do not affect how various files are accessed via Windows (not WSL1) absolute paths.

## Other build scripts
To build only the Platinum specific source files (i.e. don't build tools or SDK source files), run:
```
./build_src.sh -j
```

To remove everything used in the build, run `./clean_all.sh`. To remove only code objects, run `./tidy_all.sh`. 

# Current status
TODO.
