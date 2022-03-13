# RPG Log Parser

A piece of software created to parse MediaWiki RPG logs, and convert them to JSON, or do the conversion in reverse. Future plans also include an editor, as well as the ability to consolidate date and time formats.

The program can run as both a GUI application and a command-line application: when all three arguments are given, it runs as a console application, and if any of the three is missing, it will open a GUI prompt for selecting an option.

It is run as `./RplogParser --mode <MODE> --in <INPUT FILE/DIRECTORY PATH> --out <OUTPUT FILE/DIRECTORY PATH>`. Supported mods include:
* `textlog2json`: parses a single MediaWiki-formatted RPG log file, and outputs a JSON
* `textlog2jsonDIR`: parses a directory full of MediaWiki-formatted RPG log files, and outputs JSON files into a given directory
* `json2textlog`: parses a single JSON file, and outputs a MediaWiki-formatted RPG log file
* `json2textlogDIR`: parses a directory full of JSON files, and outputs MediaWiki-formatted RPG log files into a given directory
* `template2jsonDIR`: parses a directory full of RPG post templates, and outputs a JSON file containing said post templates.
