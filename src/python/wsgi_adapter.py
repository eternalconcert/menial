import os
from io import BytesIO
from base64 import b64encode
import traceback


def make_headers(status, headers):
    result = "HTTP/1.0 {status}\n" \
             "Server: menial\n".format(status=status)

    for key, value in headers:
        result += key + ": " + value + "\n"

    return result


template = "<h1>Internal Server Error</h1>"
if os.getenv("DEBUG"):
    template += """<h2>{exc}</h2><pre>{traceback}</pre>"""


def call_application(application, environ):
    global status, headers
    status = ""
    headers = []
    body = BytesIO()

    def start_response(rstatus, rheaders):
        global status, headers
        status, headers = rstatus, rheaders

    try:
        app_iter = application(environ, start_response)
    except Exception as e:
        exc = traceback.format_exc()
        status = 500
        return status, headers, template.format(exc=e, traceback=exc).encode()
    try:
        for data in app_iter:
            assert (status is not None and headers is not None), "start_response was not called"
            if data:
                body.write(data)

    except Exception as e:
        exc = traceback.format_exc()
        status = 500
        body.write(template.format(exc=e, traceback=exc).encode())
    finally:
        if hasattr(app_iter, 'close'):
            app_iter.close()
    return status, headers, body.getvalue()


def get_content_type_from_headers(headers):
    value = ""
    for line in headers.splitlines():
        if line.startswith("Content-Type: "):
            value = line.split(": ")[1]
    return value


def get_referer_from_headers(headers):
    value = ""
    for line in headers.splitlines():
        if line.startswith("Referer: "):
            value = line.split(": ")[1]
    return value


def get_cookies_from_headers(headers):
    value = ""
    for line in headers.splitlines():
        if line.startswith("Cookie: "):
            value = line.split(": ")[1]
    return value


def get_query_string(target):
    query_string = ""
    if len(target.split('?')) > 1:
        query_string = target.split('?')[1]
    return query_string


def wsgi_interface(app, *args):
    host = args[0].split(":")[0]
    port = None if len(args[0].split(":")) <= 1 else args[0].split(":")[1]
    target = args[1]
    header = args[2]
    body = args[3]
    wsgi_input = BytesIO(body)

    environ = {
        "REQUEST_METHOD": header.split('/')[0].strip(),
        "CONTENT_LENGTH": len(body),
        "CONTENT_TYPE": get_content_type_from_headers(header),
        "SCRIPT_NAME": "",
        "PATH_INFO": target.split('?')[0],
        "QUERY_STRING": get_query_string(target),
        "SERVER_NAME": host,
        "SERVER_PORT": port,
        "SERVER_PROTOCOL": "HTTP/1.0",
        "HTTP_REFERER": get_referer_from_headers(header),
        "HTTP_COOKIE": get_cookies_from_headers(header),
        "wsgi.input": wsgi_input,
        "wsgi.url_scheme": "",
        "wsgi.errors": ""
    }

    _status, _headers, _body = call_application(app, environ)
    headers = make_headers(_status, _headers)
    wsgi_input.close()
    return headers.encode(), b64encode(_body)
