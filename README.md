# Windows-RAT
This repository will serve as an exploratory environment where I will rely primarily on C++ and Windows APIs to delve into the deeper aspects of Windows security management along with the varying components and architectures that come within the Windows environment.

## Repository Breakdown

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
### Perform a *get file*


## TODO
- Finish getfile functionality
  - Eventually add flags for offsets into file and chunksizes to send back to C2?
- Implement putfile functionality
- Implement password hash stealing via registry reads 
- Implement registry key read/write add/delete
- Implement screenshot
