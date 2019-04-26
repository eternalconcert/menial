from io import BytesIO
import StringIO
import sys

sys.path.append("/home/xgwschk/projects/demobase/")
sys.path.append("/home/xgwschk/projects/demobase/pythonenv/lib/python2.7/site-packages/")



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


def wsgi(app, *args):
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
        "PATH_INFO": target,
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

    return b"{0}\n{1}".format(headers, _body)
