import sys
from argparse import ArgumentParser

parser = ArgumentParser()

parser.add_argument('-s', dest='host')
parser.add_argument('-t', dest='target')
parser.add_argument('-p', dest='header')
parser.add_argument('-b', dest='body')

args = parser.parse_args()


class Request(object):
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
            for item in query_string.split('&'):
                try:
                    key, value = item.split('=')
                    params[key] = value
                except ValueError:
                    params[item] = None
        return params

    def _get_post_params(self):
        params = {}
        if len(self.body) > 3:
            body = self.body[3:]
            for item in body.split("&"):
                params[item.split("=")[0]] = item.split("=")[1]
        return params


try:
    body = ""

    if len(sys.argv) > 4:
        body = sys.argv[4]
    request = Request(sys.argv[1], sys.argv[2], sys.argv[3], body)


except Exception as e:
    print("<h1>An error occured during the request.</h1><h2>Traceback:</h2>{}".format(e))
