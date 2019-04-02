import re
import sys
import json
import os
from random import randint
from argparse import ArgumentParser

parser = ArgumentParser()

parser.add_argument('-s', dest='host')
parser.add_argument('-t', dest='target')
parser.add_argument('-p', dest='header')
parser.add_argument('-b', dest='body')

args = parser.parse_args()


class Session:

    base_path = "/tmp/menial/"
    if not os.path.isdir(base_path):
        os.mkdir(base_path)

    def __init__(self, _id, content):
        self._id = int(_id)
        self.content = content
        self.file_path = os.path.join(Session.base_path, str(_id))

    def __getitem__(self, key):
        return self.content.get(key)

    def __setitem__(self, key, value):
        self.content[key] = value

    def get(self, key, default=None):
        try:
            self.content[key]
        except KeyError:
            return default

    def save(self):
        with open(self.file_path, 'w') as f:
            f.write(json.dumps({'id': self._id, 'content': self.content}))

    @classmethod
    def create(cls):
        _id = randint(10000000000000, 9999999999999999)
        inst = cls(_id, {})
        session_dict = {'id': inst._id, 'content': inst.content}
        with open(inst.file_path, 'w') as f:
            f.write(json.dumps(session_dict))
        return inst

    @classmethod
    def get_by_id(cls, _id):
        file_path  = os.path.join(Session.base_path, str(_id))
        if os.path.isfile(file_path):
            with open(file_path, 'r') as f:
                session_dict = json.loads(f.read())
                return Session(_id, session_dict['content'])


class Request(object):
    def __init__(self, host, target, headers, body):
        self.host = host
        self.target = target.split('?')[0]
        self.uri = target
        self.headers = headers
        self.body = body
        self.method = self.headers.split('/')[0].strip()
        self.get = self._get_get_params()
        self.post = self._get_post_params()
        self.session_id = self.get_session_id_from_headers()
        if self.session_id:
            self.session = Session.get_by_id(self.session_id)
        else:
            self.session = Session.create()
            self.session_id = self.session._id

    def get_session_id_from_headers(self):
        for line in self.headers.splitlines():
            if line.startswith("Cookie: menial-session"):
                value  = line.split("=")[1]
                return value.split(";")[0]

    def _get_get_params(self):
        params = {}
        if len(self.uri.split('?')) > 1:
            query_string = self.uri.split('?')[1]
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


try:
    request = Request(args.host, args.target, args.header, args.body)

except Exception as e:
    print("<h1>An error occured during the request.</h1><h2>Traceback:</h2>{}".format(e))


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


class NotFoundError(Exception):

    def __init__(self, message):
        message = "Cannot find ressource: " +  message
        super(NotFoundError, self).__init__(message)


class Error:
    def __init__(self, ex, status):
        self.ex = ex
        self.status = status

    def __repr__(self):
        template = """
            <head>
                <title>{statustext}: menial python exception</title>
            </head>
            <h1>{statustext}: menial python exception</h1>
            <div style="color: red">
                {error}
            </div>
        """
        return template.format(error=self.ex, statustext=status_messages[self.status])


status_messages = {
    200: "200 OK",
    301: "301 Moved Permanently",
    302: "302 Moved Temporary",
    304: "304 Not Modified",
    401: "401 Unauthorized",
    404: "404 Not Found",
    405: "405 Method Not Allowed",
    500: "500 Internal Server Error"
}


class Response:

    def __init__(self, request, function, func_args):
        self.status = 200
        self.request = request
        self.function = function
        self.func_args = func_args
        self.body = self.make_body()
        self.headers = self.make_headers()
        self.request.session.save()

    def make_headers(self):
        headers = "HTTP/1.0 {status}\n" \
                  "Server: menial"
        headers += "\nSet-Cookie: menial-session={}".format(self.request.session._id)
        headers = headers.format(status=status_messages[self.status])
        return headers

    def make_body(self):
        try:
            return self.function(*self.func_args)
        except Exception as e:
            self.status = 500
            return Error(e, self.status)


class App:

    def __init__(self):
        self.url_patterns = {}

    def __call__(self):
        self.request = request
        self.send_response()

    def send_response(self):
        try:
            func, func_args = self._get_route_function(request.target)
            response = Response(request, func, func_args)
            print(response.headers)
            print("\n\n")
            print(response.body)
        except NotFoundError as e:
            print("HTTP/1.0 {status}\n" \
                  "Server: menial\n\n \
                  {body}".format(status=status_messages[404], body=Error(e, 404))
            )

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
        raise NotFoundError(url)

    def route(self, url):
        def function_wrapper(func):
            self.url_patterns[url] = func
        return function_wrapper

