# Pseudoassembly documentation

## What is pseudoassembly?

Pseudoassembly is a simplified version of assembly language intended to help learn basics of assembly. It covers less data types and less instructions than regular assembly.

## Regular structure
Pseudoassembly code is divided into two sections separated by an empty line:

### Memory section
Here you can declare variables. A typical declaration looks like this:
```sh
[label] [DS/DC] [number * ]INTEGER[(value)]
```
- keywords in declaration should be separated by a space or tab
- label is used to reference the variable, if you don't want to use label make sure to start line with a space or tab
- command DS is responsible for declaration of memory without initial value
- command DC is used to declare a variable and assign it a starting value, if no value is supplied interpreter will use random value
- preceding the variable type with number and '*' allows to declare an array of desired size
- the only suppported variable type at the moment is integer but it still needs to be specified in declaration
- if DC command was used initial value of variable can be specified inside brackets after variable type (variable type and opening bracket should't be separated by any characters)

#### Examples
 - declaration of integer named ONE of value 1
 ```sh
 ONE    DC INTEGER(1)
 ```
 - declaration of unnamed space
 ```sh
        DS INTEGER
 ```
 - declaration of array of 10 random integers named RAND (only first variable in array will have this label)
```sh
RAND    DC 10 * INTEGER
```

### Instruction section

In this section you specify the operations your program will perform. They should follow this pattern:
```sh
[label] [command] [first argument] [,second argument]
```
- keywords in declaration should be separated by a space or tab
- label is used to reference the instruction, if you don't want to use label make sure to start line with a space or tab
- command is used to specify what given instruction does, you can find full list of avaliable commands below
- jump instructions take only one argument in form of instruction label or instruction address 
- non-jump instructions take two arguments, first is a register number and second could be second register address, memory label or memory address
- memory/instruction address takes this form: "offset(register)" where register contains some  memory/instruction address and offset specifies how far from this address is the desired one 

#### Examples

- add variable ONE to second register
```sh
        A 2,ONE
```
- load address of second variable in memory stack to first register, with label LOAD
```sh
LOAD    LA 1,4(15)
```
- jump to instruction with label LOAD
```sh
        J LOAD
```

## Machine code (hexadecimal) structure

Here instead of keywords and labels we use hexadecimal numbers.
As in regular version code is divided into two sections:

### Memory section

- constants are defined by writing its value in hex U2 with 8 digits and a space after every even digit
- space is defined by writing eight tildes with a space after every even tilde
- labels are not allowed in this version
- every variable of an array has to be declared individually

#### Examples

- declare a variable of value 24
```sh
00 00 00 18
```
- declare a variable of value -1
```sh
FF FF FF FF
```
- declare empty space
```sh
~~ ~~ ~~ ~~
```

### Instruction section

- labels are not allowed in this version
- instead of command codes use their hexadecimal counterparts
- after command specify hexadecimal address where first two digits specify two registers and the rest is used to express offset in hex
- register instructions don't have offset in address
- put space after every even character in instruction

#### Examples

- add second register to first register
```sh
D1 1F
```

- load fourth variable to third register
```sh
F0 3F 00 10
```

- jump to first instruction
```sh
E0 0E 00 00
```

## Table of all instruction codes

|Instruction    |CMD    |HEX    |
|---------------|:-----:|:-----:|
|Add registers|AR|10|
|Add|A|D1|
|Subtract registers|SR|12|
|Subtract|S|D3|
|Multiply registers|MR|14|
|Multiply|M|D5|
|Divide registers|DR|16|
|Divide|D|D7|
|Compare registers|CR|18|
|Compare|C|D9|
|Jump|J|E0|
|Jump if zero|JZ|E1|
|Jump if positive|JP|E2|
|Jump if negative|JN|E3|
|Load|L|F0|
|Load register|LR|F1|
|Load address|LA|F3|
|Store in memory|ST|F3|

## Comments
Comments should start with a '/' character and are ignored by the interpreter.