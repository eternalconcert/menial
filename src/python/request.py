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


class SessionBase(object):

    def __init__(self, _id, content):
        self._id = int(_id)
        self.content = content

    def __getitem__(self, key):
        return self.content.get(key)

    def __setitem__(self, key, value):
        self.content[key] = value

    def get(self, key, default=None):
        try:
            return self.content[key]
        except KeyError:
            return default

    def pop(self, key):
        return self.content.pop(key, None)

    def save(self):
        raise NotImplementedError("save method must be implemented by inherited class")

    @classmethod
    def create(cls):
        raise NotImplementedError("create method must be implemented by inherited class")

    @classmethod
    def get_by_id(cls, _id):
        raise NotImplementedError("get_by_id method must be implemented by inherited class")


class FileSystemSession(SessionBase):

    base_path = "/tmp/menial/"
    if not os.path.isdir(base_path):
        os.mkdir(base_path)

    def __init__(self, _id, content):
        super(FileSystemSession, self).__init__(_id, content)
        self.file_path = os.path.join(FileSystemSession.base_path, str(_id))

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
        file_path  = os.path.join(FileSystemSession.base_path, str(_id))
        if os.path.isfile(file_path):
            with open(file_path, 'r') as f:
                session_dict = json.loads(f.read())
                return FileSystemSession(_id, session_dict['content'])


session_adapter = FileSystemSession


class Request(object):
    def __init__(self, host, port, target, headers, body):
        self.host = host
        self.port = port
        self.target = target.split('?')[0]
        self.uri = target
        self.headers = headers
        self.body = body
        self.method = self.headers.split('/')[0].strip()
        self.query_string = ""
        self.get = self._get_get_params()
        self.post = self._get_post_params()

        current_session_id = self.get_session_id_from_headers()
        if current_session_id:
            current_session = session_adapter.get_by_id(current_session_id)
            if current_session:
                self.session = current_session
            else:
                self.session =  session_adapter.create()
        else:
            self.session = session_adapter.create()
        self.session_id = self.session._id

    def get_session_id_from_headers(self):
        for line in self.headers.splitlines():
            if line.startswith("Cookie: menial-session"):
                value  = line.split("=")[1]
                return value.split(";")[0]

    def _get_get_params(self):
        params = {}
        if len(self.uri.split('?')) > 1:
            self.query_string = self.uri.split('?')[1]
            for item in self.query_string.split('&'):
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
    host = args.host.split(":")[0]
    port = None if len (args.host.split(":")) <= 1 else args.host.split(":")[1]
    request = Request(host, port, args.target, args.header, args.body)

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


def render(content, status=200):
    return (None, content, status)


def redirect(url, permanent=False):
    headers = Response.header_base
    headers += "Location: {url}\n".format(url=url)
    status = 302 if not permanent else 301
    return (headers, None, status)


class Error:

    def __init__(self, ex, status):
        self.ex = ex
        self.status = status

    def __repr__(self):
        status = status_messages[self.status]

        header = Response.header_base.format(status=status)
        template = """{header}

            <head>
                <title>{status}</title>
            </head>
            <h1>{status}</h1>
            <div style="color: red">
                {error}
            </div>
        """
        return template.format(header=header, error=self.ex, status=status)


class Response:

    header_base = "HTTP/1.0 {status}\n" \
                  "Server: menial\n"

    def __init__(self, request, function, func_args):
        self.request = request
        self.function = function
        self.func_args = func_args

        headers, body, status = self.function(self.request, *self.func_args)

        if not headers:
            headers = self.make_headers()

        self.body = body
        self.headers = headers.format(status=status_messages[status])
        self.request.session.save()

    def make_headers(self):
        headers = Response.header_base
        if not self.request.get_session_id_from_headers():
            headers += "Set-Cookie: menial-session={}".format(self.request.session._id)
        return headers


class App:

    url_patterns = {}
    static_files_dir = None
    static_files_url = None

    def __call__(self):
        self.request = request
        try:
            self.send_response()
        except Exception as e:
            error = Error(e, 500)
            print(error)

    def send_response(self):
        func, func_args = self._get_route_function(request.target)
        response = Response(self.request, func, func_args)
        print(response.headers)
        print("")
        print(response.body)

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
        if App.static_files_url and url.startswith(App.static_files_url):
            return send_static_file, [url]

        if url[-1:] != "/" and not self.request.get:
            url += "/"

        arguments = []
        for pattern, func in App.url_patterns.items():
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
            App.url_patterns[url] = func
        return function_wrapper


def url_for(func_name, *args):
    for key, value in App.url_patterns.items():
        if value.__name__ == func_name:
            arg_idx = 0
            url = ""
            for index, part in enumerate(key.split('/')):
                if part:
                    url += '/'
                    if re.match(r"(<\S+?>)", part):
                        part = str(args[arg_idx])
                        arg_idx += 1
                    url += part
            return url + "/"


def send_static_file(reqest, file_path):
    if ".." in file_path:
        raise Exception("Intrusion try")
    full_path = App.static_files_dir + file_path
    full_path = os.path.normpath(full_path)
    full_path = full_path.replace(App.static_files_url, "/", 1)
    full_path = os.path.normpath(full_path)
    with open(full_path) as f:
        content = f.read()
        return render(content)


class Template(object):

    def __init__(self, file_path):
        with open(file_path, "r") as f:
            self.content = f.read()

    def render(self, **kwargs):
        return self.content.format(**kwargs)
