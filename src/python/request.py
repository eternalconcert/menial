import sys
from argparse import ArgumentParser

parser = ArgumentParser()

parser.add_argument('-s', dest='host')
parser.add_argument('-t', dest='target')
parser.add_argument('-p', dest='header')
parser.add_argument('-b', dest='body')

args = parser.parse_args()


class Request(object):
    def __init__(self, host, target, header, body):
        self.host = host
        self.target = target
        self.header = header
        self.body = body
        self.method = self.header.split('/')[0].strip()
        self.get = self._get_get_params()
        self.post = self._get_post_params()

    def _get_get_params(self):
        params = {}
        if len(self.target.split('?')) > 1:
            query_string = self.target.split('?')[1]
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



def notFound():
    print(404)


class App:

    def __init__(self):
        self.url_mapping = {}

    def __call__(self, request):
        self.request = request
        self.url_mapping.get(request.target, notFound)()

    def route(self, url):
        def function_wrapper(func):
            self.url_mapping[url] = func
        return function_wrapper


try:
    request = Request(args.host, args.target, args.header, args.body)

except Exception as e:
    print("<h1>An error occured during the request.</h1><h2>Traceback:</h2>{}".format(e))
