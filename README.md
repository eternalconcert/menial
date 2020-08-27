![Tests](https://github.com/eternalconcert/menial/workflows/Tests/badge.svg?branch=master)

# menial
Always at your service!

# Documentation
## Compiling
To compile menial, you can use the following commands:

```bash
g++ $(find src/  -name "*.cpp") -o menial.bin -std=c++11 -pthread -Wall -I /usr/include/python3.8m/ -l python3.8m -I src/include/ -lssl -lcrypto
```
## Prerequisites

- g++/gcc
- OpenSSL headers
- Python3

## Configuration
You can find the default config file in the root folder of your menial installation. It contains all configuration directives you need to customize menial.

The content is JSON formated and has the following structure:
```json
{
    "hosts": {
        "*:80": {
            "handler": "file",
            "root": "default/",
            "staticdir": "default/static/",
            "defaultdocument": "index.html",
            "additionalheaders": "Access-Control-Allow-Origin: *\n"
        },
        "*:8080": {
            "handler": "redirect",
            "root": "",
            "permanent": true,
            "target": "http://localhost/",
            "keeptarget": true
        },
        "*:8080": {
            "handler": "file",
            "root": "/restricted/",
            "authfile": "auth.db",
            "dirlisting": true
        }
    },
    "logger": "console",
    "loglevel": "info",
    "iplogging": false,
    "logfilepath": "/tmp/",
    "staticdir": "/etc/menial/static/",
    "resources": "/etc/menial/resources/",
    "debug": false,
    "ssl": {
        "443": {
            "on": true,
            "cert": "/path/to/the/cert.pem",
            "key": "/path/to/the/key.pem"
        }
    }
}
```
Detailed information about the particular directives can be found on the [project website](https://menial.softcreate.de).

## Custom config
When you run menial you need to pass the location to the config file as first parameter.
You can create your own config file (e.g. copying the exsting one) and save it at any accessible locatation. Handover the new file to menial on startup.