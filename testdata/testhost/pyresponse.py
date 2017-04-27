import sys
import json

with open("testdata/menial.json") as f:
    config = json.load(f)

template = """
<!DOCTYPE html>
<html>
    <head>
        <title>menial: Always at your service!</title>
        <meta http-equiv="content-type" content="text/html;charset=utf-8" />
    </head>

    <body>
        <div id="main">
            <h1>menial</h1>
            <p id="claim">
                Always at your service!
            </p>
            <p>
                If you see this page, the menial webserver is up and running with the python extension.
            </p>

            <div id=conf>
            <h2>Configuration:</h2>
                <table width=200>
                    <tr>
                        <td>Port:</td>
                        <td>{}</td>
                    </tr>
                    <tr>
                        <td>Loglevel:</td>
                        <td>{}</td>
                    </tr>
                    <tr>
                        <td>Logger:</td>
                        <td>{}</td>
                    </tr>
                    <tr>
                        <td>Logfilepath:</td>
                        <td>{}</td>
                    </tr>
                </table>
            </div>
            <br />
            <div id=footer>
                Licensed under <a href="https://www.gnu.org/licenses/gpl.html">GPLv3</a>
            </div>
        </div>
    </body>
</html>

""".format(config['port'], config['loglevel'], config['logger'], config['logfilepath'])

sys.stdout.write(template)
