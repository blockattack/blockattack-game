# Contributing

## Introduction
This is an Open Source project and contributions are welcome.

Both the current graphics and the original sharedir support was provided by kind individuals.
Patches can be attached to a github ticket or sent to poul@poulsander.com directly. You can also create pull requests on Github.

## Coding
The project must compile on the olders support LTS version of Ubuntu. It should also be tested on Fedora.

If any dependecies are not provided they must be embedded.

Except for official repositories no internet access should be needed.

Code formatting. The code formatting is handled by astyle. The script `source/misc/astyle/runAstyle.sh`

Windows version is compiled using MXE and docker.

## Translations
Want to translate to a new language?

This project uses po-files for translations.

The template file can be found in `source/misc/translation/template/blockattack_roftb.pot`. Programs like Poedit (https://poedit.net/) can be used to translate the files.

The resulting po files should be in: `source/misc/translation/po/`

Or just open a ticket and attach the po-file.
