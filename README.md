# RPG Log Parser

A piece of software created to parse MediaWiki RPG logs, and convert them to JSON, or do the conversion in reverse. Future plans also include an editor, as well as the ability to consolidate date and time formats.

The program can run as both a GUI application and a command-line application: when all three arguments are given, it runs as a console application, and if any of the three is missing, it will open a GUI prompt for selecting an option.

It is run as `./RplogParser --mode <MODE> --in <INPUT FILE/DIRECTORY PATH> --out <OUTPUT FILE/DIRECTORY PATH>`. Supported mods include:
* `textlog2json`: parses a single MediaWiki-formatted RPG log file, and outputs a JSON
* `textlog2jsonDIR`: parses a directory full of MediaWiki-formatted RPG log files, and outputs JSON files into a given directory
* `json2textlog`: parses a single JSON file, and outputs a MediaWiki-formatted RPG log file
* `json2textlogDIR`: parses a directory full of JSON files, and outputs MediaWiki-formatted RPG log files into a given directory
* `template2jsonDIR`: parses a directory full of RPG post templates, and outputs a JSON file containing said post templates.
* `dateparsetest`: simply tests, if there are any malformed dates within the RPG session that cannot be parsed to proper dates.
* `discord2textlog`: Converts Discord JSON logs into MediaWiki-formatted RPG logs
* `discord2jsonlog`: Converts Discord JSON logs into JSON RPG logs
* `textlog2agnaistic`: Converts MediaWiki-formatted RPG logs and converts them into Agnaistic JSON logs, so you can fool the AI into thinking it wrote your RPG logs and continue it. Needs the additional arguments `--playername` and `--playeruid` to work.
* `json2agnaistic`: Converts JSON RPG logs and converts them into Agnaistic JSON logs, so you can fool the AI into thinking it wrote your RPG logs and continue it. Needs the additional arguments `--playername` and `--playeruid` to work.
* `agnaistic2textlog`: Takes Agnaistic JSON logs and converts them into MediaWiki RPG logs.
* `agnaistic2json`: Takes Agnaistic JSON logs and converts them into JSON RPG logs.
* `textlog2tavernai`: Converts MediaWiki-formatted RPG logs and converts them into TavernAI JSON logs, so you can fool the AI into thinking it wrote your RPG logs and continue it.  Needs the additional arguments `--playername` and `--chid` to work.
* `json2tavernai`: Converts JSON RPG logs and converts them into TavernAI JSON logs, so you can fool the AI into thinking it wrote your RPG logs and continue it. Needs the additional arguments `--playername` and `--chid` to work.
* `tavernai2textlog`: Takes TavernAI JSON logs and converts them into MediaWiki RPG logs.
* `tavernai2json`: Takes TavernAI JSON logs and converts them into JSON RPG logs.
* `textlog2sillytavern`: Converts MediaWiki-formatted RPG logs and converts them into SillyTavern JSON logs, so you can fool the AI into thinking it wrote your RPG logs and continue it.  Needs the additional arguments `--playername` to work.
* `json2sillytavern`: Converts JSON RPG logs and converts them into SillyTavern JSON logs, so you can fool the AI into thinking it wrote your RPG logs and continue it. Needs the additional arguments `--playername`  to work.
* `sillytavern2textlog`: Takes SillyTavern JSON logs and converts them into MediaWiki RPG logs.
* `sillytavern2json`: Takes SillyTavern JSON logs and converts them into JSON RPG logs.
* `textlog2aitemplate`: Takes a MediaWiki-formatted RPG session and converts into an example conversation for giving an AI more presonality. Needs the extra argument `--playername`.
* `json2aitemplate`: Takes a JSON-formatted RPG session and converts into an example conversation for giving an AI more presonality. Needs the extra argument `--playername`.

## What is this MediaWiki log format that's being mentioned?

it is the format for storing RPG logs on [my wiki](https://waysofdarkness.miraheze.org/wiki/Main_Page).

```
{{RPG Post/<poster's character name>
|date=<post date>
|post=<post content>
}}
```

The alternate JSON format is:

```json
[
    {
        "posts": [
            {
                "content": "<post content>",
                "date": "<post date>",
                "hun": false,
                "streamlinedDate": <true if we have a unix timestamp, false otherwise>,
                "unixTimestamp": <post date converted to a Unix timestamp, optional>,
                "user": "<character name>"
            },
            ...
        ],
        "sectionName": "[[Fort Redlight]] ''(near [[Grandfolk]])'', after a timeskip"
    }
]
```
