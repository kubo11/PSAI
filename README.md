# PSAI
## PSeudoAssembly Interpreter

PSAI is a simple pseudoassembly interpreter that helps with learning 
a simplified version of assembly. It was initially written as a project
for CS studies and then redeveloped for better performance and functionality.

## Features

- Input files in regular and hexadecimal code
- Output in hexadecimal code before and after execution
- Error messages in command line
- Debug feature to find bugs in your code

## Installation

PSAI was developed as a [Visual Studio](https://visualstudio.microsoft.com/) project and won't work with any 
other compiler (because of VS safe functions).

It also requires [PDCurses](https://pdcurses.org/) library for the debug feature.
You can get PDCurses for Visual Studio using [vcpkg](https://vcpkg.io/en/getting-started.html).
Simply install it by following instructions listed on it's website and then
copy the following commands into command line:

```sh
vcpkg install pdcurses
vcpkg integrate install
```

## User manual

Run program by executing the following command:
```sh
PSAI.exe [input path] [psa_code/mcsk_code] [debug]
```
Program arguments:
- input path, if a wrong one or none is supplied program will ask for a correct path
- input file format, psa_code - regular, mcsk_code - machine code (hexadecimal)
- debug, enables debug mode

 Skipping former arguments or changing their order will produce an error.

For more information about pseudoassembly see [DOCUMENTATION.md](https://github.com/kubo11/PSAI/blob/master/DOCUMENTATION.md).

## Project status: _finished_

## License

MIT
