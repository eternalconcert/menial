import sys
import json


class ResquestDispatcher(object):
    def __init__(self, host, method, get_params):
        self.host = host
        self.url = host.lstrip('/')
        self.get_params = get_params
        self.method = method

get_params = None

if len(sys.argv) >= 4:
    get_params = sys.argv[3]

try:
    request_dispatcher = ResquestDispatcher(sys.argv[1], sys.argv[2], get_params)
    print(request_dispatcher.url, request_dispatcher.method, request_dispatcher.get_params)
except Exception as e:
    print("<h1>An error occured during the request.</h1><h2>Traceback:</h2>{}".format(e))



'''with open("testdata/menial.json") as f:
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

""".format(config['loglevel'], config['logger'], config['logfilepath'])'''

# sys.stdout.write(respsonse)
