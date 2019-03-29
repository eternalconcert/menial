import re
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
            body = self.body
            for item in body.split("&"):
                params[item.split("=")[0]] = item.split("=")[1]
        return params


template = """
<html>
    <head>
        <title>{title}</title>
    </head>
    <body>
        {body}
    </body>
</html>
"""


def notFound(*args):
    message = "404 Not Found"
    return template.format(title=message, body=message)


class App:

    def __init__(self):
        self.url_patterns = {}

    def __call__(self, request):
        self.request = request
        func, args = self._get_route_function(request.target)
        print(func(*args))

    @staticmethod
    def _get_type(pattern):
        if len(pattern.split(":")) == 1:
            return
        typename = pattern.split(":")[0].replace("<", "")
        if typename == "int":
            return int
        if typename == "str":
            return str
        if typename == "float":
            return float
        raise TypeError("Type %s cannot be used for type casting." % typename)

    def _get_route_function(self, url):
        if url[-1:] != "/" and not self.request.get:
            url += "/"
        arguments = []
        for pattern, func in self.url_patterns.items():
            if pattern == url:
                return func, arguments
            url_parts = url.split("/")
            pattern_parts = pattern.split("/")
            if len(pattern_parts) != len(url_parts):
                continue
            match = True
            for index, part in enumerate(pattern_parts):
                if re.match(r"(<\S+?>)", part):
                    type_to_cast = self._get_type(part)
                    if type_to_cast:
                        url_parts[index] = type_to_cast(url_parts[index])
                    arguments.append(url_parts[index])
                elif part != url_parts[index]:
                    match = False
                    break
            if match:
                return func, arguments
        return notFound, arguments

    def route(self, url):
        def function_wrapper(func):
            self.url_patterns[url] = func
        return function_wrapper


try:
    request = Request(args.host, args.target, args.header, args.body)

except Exception as e:
    print("<h1>An error occured during the request.</h1><h2>Traceback:</h2>{}".format(e))
