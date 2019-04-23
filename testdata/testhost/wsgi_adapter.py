import sys
sys.path.append("/home/xgwschk/projects/pax-musica/")
sys.path.append("/home/xgwschk/projects/pax-musica/pythonenv/lib/python3.6/site-packages/")
sys.path.append("/home/xgwschk/projects/menial/src/python/")
from request import App, redirect, render, url_for, Request
from paxmusica.wsgi import application as app


result = []


def get_response_headers(*args):

    status = args[0]
    headers = "HTTP/1.0 {status}\n" \
              "Server: menial\n".format(status=status)

    for key, value in args[1]:
        headers += key + ": " + value + "\n"

    result.append(headers)


def dummy(*args):
    pass


#try:
#    content = application(environ, get_response_headers).content.decode("utf-8")
#    print(content)
#except AttributeError:
#    content = application(environ, dummy).streaming_content
#    for item in content:
#        print(item.decode("utf-8"))


def application(*args):
    host = args[0].split(":")[0]
    port = None if len(args[0].split(":")) <= 1 else args[0].split(":")[1]
    target = args[1]
    header = args[2]
    body = args[3]
    request = Request(host, port, target, header, body)


    environ = {
        "REQUEST_METHOD": request.method,
        "SCRIPT_NAME": "",
        "PATH_INFO": request.target,
        "QUERY_STRING": request.query_string,
        "SERVER_NAME": request.host,
        "SERVER_PORT": request.port,
        "SERVER_PROTOCOL": "HTTP/1.0",
        "wsgi.input": None,
        "wsgi.url_scheme": "",
        "wsgi.errors": ""
    }
    #content = app(environ, get_response_headers)
    content = app(environ, get_response_headers).content.decode("utf-8")
    result.append(content)
    return result[0] + "\n" + result[1]
