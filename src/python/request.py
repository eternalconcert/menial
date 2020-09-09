import json
import re
import os
import traceback
import base64
from hashlib import sha256


class ClientSession:

    def __init__(self, cookies, secret_key):
        self.secret_key = secret_key

        self.session_cookie = None
        for line in cookies.split(';'):
            line = line.strip()
            if line.startswith('session'):
                self.session_cookie = line.split('=')[1].strip()
        self.data = self.get_payload()

    def get_payload(self):
        if not self.session_cookie:
            return {}

        hashed_payload = self.session_cookie.split('.')[0]
        checksum = self.session_cookie.split('.')[-1]
        if not checksum or not hashed_payload:
            break_here_2
            return {}
        payload = base64.urlsafe_b64decode(hashed_payload + "==").decode()

        if sha256((hashed_payload + self.secret_key).encode()).hexdigest() == checksum:
            return json.loads(payload)
        return {}

    def cookie_header(self):
        data = json.dumps(self.data).encode()
        hashed_payload = base64.urlsafe_b64encode(data).decode().replace('=', '')
        checksum = sha256((hashed_payload + self.secret_key).encode()).hexdigest()
        return ("Set-Cookie", "session={0}.{1}; Path=/".format(hashed_payload, checksum))

    def __getitem__(self, key, default=None):
        try:
            return self.data[key]
        except KeyError:
            return default

    def __setitem__(self, key, value):
        self.data[key] = value


class Request(object):
    def __init__(self, environ, secret_key):
        assert (secret_key is not None and secret_key != ""), "Please provide a secret key"
        self.secret_key = secret_key
        self.host = environ['SERVER_NAME']
        self.port = environ['SERVER_PORT']
        self.method = environ['REQUEST_METHOD']
        self.target = environ['PATH_INFO'].split("?")[0]
        self.uri = environ['PATH_INFO']
        self.body = environ['wsgi.input'].read()
        self.query_string = environ['QUERY_STRING']
        self.content_type = environ.get('CONTENT_TYPE')
        self.cookies = environ.get('HTTP_COOKIE', '')
        self.referer = environ.get('HTTP_REFERER')
        self.get = self._get_get_params()
        self.post = self._get_post_params()
        self.session = ClientSession(self.cookies, secret_key)

    def _get_get_params(self):
        params = {}

        for item in self.query_string.split('&'):
            try:
                key, value = item.split('=')
                params[key] = value
            except ValueError:
                continue
        return params

    def _get_post_params(self):
        params = {}
        _body = self.body.decode()
        if len(_body) > 3:
            for item in _body.split("&"):
                params[item.split("=")[0]] = item.split("=")[1]
        return params


template = "<h1>Internal Server Error</h1>"
if os.getenv("DEBUG"):
    template += """<h2>{exc}</h2><pre>{traceback}</pre>"""


class NotFoundError(Exception):

    def __init__(self, message):
        message = "Cannot find ressource: " + message
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
    status = 302 if not permanent else 301
    headers = [('status_line', status_messages[status]), ('Location', url)]
    return (headers, None, status)


def get_mimetype(target=None):
    mimetypes = {
        'bin': 'application/octet-stream',
        'class': 'application/octet-stream',
        'com': 'application/octet-stream',
        'css': 'text/css',
        'extension': 'application/octet-stream',
        'file': 'application/octet-stream',
        'flac': 'audio/flac',
        'gzip': 'application/gzip',
        'jpg': 'image/jpg',
        'json': 'application/json',
        'log': 'text/plain',
        'mp3': 'audio/mpeg',
        'pdf': 'application/pdf',
        'png': 'image/png',
        'tar': 'application/x-tar',
        'ttf': 'font/ttf',
        'woff': 'font/woff',
        'woff2': 'font/woff2',
        'zip': 'application/zip',
    }
    extenstion = target.split("/")[-1].split('.')[-1]
    return mimetypes.get(extenstion, 'text/html; charset=utf-8;')


class Response:

    def __init__(self, request, function, func_args):
        self.request = request
        self.function = function
        self.func_args = func_args

        self.headers, body, status = self.function(self.request, *self.func_args)
        self.body = body if body else ""
        self.status = status_messages[status]

        if not self.headers:
            self.headers = [
                ('Content-Length', str(len(self.body))),
                ('Content-Type', get_mimetype(self.request.target))
            ]
        self.headers.append(self.request.session.cookie_header())


class App:
    url_patterns = {}
    context_preprocessors = []
    static_files_dir = None
    static_files_url = None
    secret_key = None

    def run(self, environ, start_response):
        self.request = Request(environ, App.secret_key)

        #return self.send_response(start_response)

        try:
            return self.send_response(start_response)
        except Exception as e:
            exc = traceback.format_exc()
            return render(template.format(exc=e, traceback=exc).encode(), status=500)

    def send_response(self, start_response):
        func, func_args = self._get_route_function(self.request.target)
        response = Response(self.request, func, func_args)
        start_response(response.status, response.headers)
        content = response.body
        if isinstance(content, str):
            content = content.encode()
        return [content]

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

    def context_preprocessor(self, function):
        def function_wrapper(func):
            App.context_preprocessors.append(func)
        return function_wrapper(function)


def url_for(func_name, *args):
    if func_name == 'static':
        if App.static_files_url:
            return App.static_files_url + args[0]
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


def send_static_file(request, file_path):
    if ".." in file_path:
        raise Exception("Intrusion try")
    full_path = App.static_files_dir + file_path
    full_path = os.path.normpath(full_path)
    full_path = full_path.replace(App.static_files_url, "/", 1)
    full_path = os.path.normpath(full_path)

    with open(full_path, mode='rb') as f:
        content = f.read()
        return render(content)
