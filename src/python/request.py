import re
import os
import traceback


class ClientSession:

    def __init__(self, cookies):
        self.session_cookie = cookies

    def __getitem__(self, key, default=None):
        return "TEST"

    def __setitem__(self, key, value):
        return


class Request(object):
    def __init__(self, environ):
        self.host = environ['SERVER_NAME']
        self.port = environ['SERVER_PORT']
        self.method = environ['REQUEST_METHOD']
        self.target = environ['PATH_INFO'].split("?")[0]
        self.uri = environ['PATH_INFO']
        self.body = environ['wsgi.input'].read()
        self.query_string = environ['QUERY_STRING']
        self.content_type = environ.get('CONTENT_TYPE')
        self.cookies = environ.get('HTTP_COOKIE')
        self.referer = environ.get('HTTP_REFERER')
        self.get = self._get_get_params()
        self.post = self._get_post_params()
        self.session = ClientSession(self.cookies)

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


class MimeTypeCache:
    __instance = None

    def __new__(cls):
        if not cls.__instance:
            cls.__instance = super().__new__(cls)
            cls.__instance.data = {}
            with open('resources/mimetypes.tray') as f:  # TODO: Confirgurable
                for line in f.readlines():
                    cls.__instance.data[line.split(" ")[0].strip()] = line.split(" ")[-1].strip()
        return cls.__instance

    def __setitem__(self, key, value):
        self.data[key] = value

    def __getitem__(self, key):
        return self.data[key]

    def get(self, key, default=None):
        key = key.lower()
        try:
            return self[key]
        except KeyError:
            return default


def get_mimetype(target=None):
    extenstion = target.split("/")[-1].split('.')[-1]
    mimetypes = MimeTypeCache()
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
        self.headers.append(('HeregoestheCookie', 'SettheCookie!'))


class App:
    url_patterns = {}
    context_preprocessors = []
    static_files_dir = None
    static_files_url = None

    def run(self, environ, start_response):
        self.request = Request(environ)

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
