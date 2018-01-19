import sys
import json


class Resquest(object):
    def __init__(self, host, url, header, body=None):
        self.host = host
        self.url = url
        self.header = header
        self.body = body
        self.method = self.header.split('/')[0].strip()
        self.get = self._get_get_params()
        self.post = self._get_post_params()

    def _get_get_params(self):
        params = {}
        if len(self.url.split('?')) > 1:
            query_string = self.url.split('?')[1]
            for item in query_string.split('#'):
                try:
                    key, value = item.split('=')
                    params[key] = value
                except ValueError:
                    params[item] = None
        return params

    def _get_post_params(self):
        params = {}
        if len(self.url.split('?')) > 1:
            query_string = self.url.split('?')[1]
            for item in query_string.split('#'):
                try:
                    key, value = item.split('=')
                    params[key] = value
                except ValueError:
                    params[item] = None
        return params


try:
    request = Resquest(sys.argv[1], sys.argv[2], sys.argv[3])

except Exception as e:
    print("<h1>An error occured during the request.</h1><h2>Traceback:</h2>{}".format(e))


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
            <p>
                <h2>Get paramters</h2>
                {0}
            </p>
            <form action='.' method='POST' >
                <input type='text' name='name' />
                <input type='password' name='password' />
                <button>Senden</button>
            </form>

            <div id=footer>
                Licensed under <a href="https://www.gnu.org/licenses/gpl.html">GPLv3</a>
            </div>
        </div>
    </body>
</html>

""".format(request._get_get_params())

print(template)
