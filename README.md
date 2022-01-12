# Windows-Internals
This repository will serve as an exploratory environment where I will rely primarily on C++ and Windows APIs to delve into the deeper aspects of Windows security management along with the varying components and architectures that come within the Windows environment.

## Repository Breakdown

### Internals-DLL
All cpp *implant* side code that will, upon executing, open up a port on target and listen for commands sent by the user via Python CLI.

### Internals-Py
All py *client* side code that will, upon executing, open up a port on client and send commands to the implant via user input in the CLI.
