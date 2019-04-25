from io import BytesIO
import StringIO
import sys

sys.path.append("/home/xgwschk/projects/demobase/")
sys.path.append("/home/xgwschk/projects/demobase/pythonenv/lib/python2.7/site-packages/")
sys.path.append("/home/xgwschk/projects/menial/src/python/")

from request import App, redirect, render, url_for, Request
from base.wsgi import application as app


def make_headers(status, headers):

    result = "HTTP/1.0 {status}\n" \
             "Server: menial\n".format(status=status)

    for key, value in headers:
        result += key + ": " + value + "\n"

    return result




def call_application(app, environ):
    global status, headers
    body = BytesIO()

    def start_response(rstatus, rheaders):
       global status, headers
       status, headers = rstatus, rheaders

    app_iter = app(environ, start_response)
    try:
        for data in app_iter:
            assert status is not None and headers is not None, "start_response was not called"
            body.write(data)
    except Exception as e:
        pass
    finally:
        if hasattr(app_iter, 'close'):
            app_iter.close()
    return status, headers, body.getvalue()


def application(*args):
    host = args[0].split(":")[0]
    port = None if len(args[0].split(":")) <= 1 else args[0].split(":")[1]
    target = args[1]
    header = args[2]
    body = args[3]
    request = Request(host, port, target, header, body)
    wsgi_input = BytesIO(request.body)
    environ = {
        "REQUEST_METHOD": request.method,
        "CONTENT_LENGTH": len(request.body),
        "CONTENT_TYPE": request.content_type,
        "SCRIPT_NAME": "",
        "PATH_INFO": request.target,
        "QUERY_STRING": request.query_string,
        "SERVER_NAME": request.host,
        "SERVER_PORT": request.port,
        "SERVER_PROTOCOL": "HTTP/1.0",
        "HTTP_REFERER": request.referer,
        "HTTP_COOKIE": request.cookies,
        "wsgi.input": wsgi_input,
        "wsgi.url_scheme": "",
        "wsgi.errors": ""
    }

    _status, _headers, _body = call_application(app, environ)
    headers = make_headers(_status, _headers)
    wsgi_input.close()

    return b"{0}\n{1}".format(headers, _body)
