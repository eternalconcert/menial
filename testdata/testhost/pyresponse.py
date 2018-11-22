import sys
from argparse import ArgumentParser

parser = ArgumentParser()

parser.add_argument('-s', dest='host')
parser.add_argument('-t', dest='target')
parser.add_argument('-p', dest='header')
parser.add_argument('-b', dest='body')

args = parser.parse_args()


class Resquest(object):
    def __init__(self, host, url, header, body):
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
    body = ""

    if len(sys.argv) > 4:
        body = sys.argv[4]
    request = Resquest(sys.argv[1], sys.argv[2], sys.argv[3], body)


except Exception as e:
    print("<h1>An error occured during the request.</h1><h2>Traceback:</h2>{}".format(e))


import urllib2
import os
import json

message = request._get_get_params().get('message', '').replace('+', ' ')
message = urllib2.unquote(message).decode('utf-8')
read = request._get_get_params().get('read', False)


template = """
<!DOCTYPE html>
<html>
    <head>
        <title>Post2Pi</title>
    </head>
    <body>
        <h1>Post2Pi</h1>
        {confirmation}
        <form>
            <input type="text" name="message" placeholder="Deine Nachricht" />
            <button>Senden</button>
        </form>
        <br>
        <div style="font-size: 0.7em; margin-top: 50px;">
            Powered by <a href="http://menial.softcreate.de" style="color: green;">menial</a>
        </div>
    </body>
</html>
"""

if read:
    if not os.path.isfile('index.db'):
        open('index.db', 'w')
    if not os.path.isfile('messages.db'):
        open('messages.db', 'w')

    with open('messages.db', 'r') as f:
        lines = f.readlines()
        maxindex = len(lines) - 1

    with open('index.db', 'r') as f:
        index = int(f.read() or "-1")
        index += 1
        if index <= maxindex:
            with open('index.db', 'w') as f:
                f.write(str(index))
                message = {'message': lines[index]}
            print(json.dumps(message))

elif not message:
    print(template.format(confirmation=''))


else:
    with open('messages.db', 'a+') as f:
        f.write(message + '\n')

    confirmation = '<div style="color: green; margin: 10px 0 30px 0">Vielen Dank! Deine Nachricht wird geposted:<br>{message}</div>'.format(message=message)

    print(template.format(confirmation=confirmation))
