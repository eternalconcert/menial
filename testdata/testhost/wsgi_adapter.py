import sys
sys.path.append("src/python/")
sys.path.append("/home/christian/Projekte/pax-musica/")

from request import App, render, redirect, url_for, request
from paxmusica.wsgi import application


environ = {
    "REQUEST_METHOD": request.method,
    "SCRIPT_NAME": "",
    "PATH_INFO": request.target,
    "QUERY_STRING": request.query_string,
    "SERVER_NAME": request.host,
    "SERVER_PORT": request.port,
    "SERVER_PROTOCOL": "HTTP/1.0",
    "wsgi.input": None
}


def get_response_headers(*args):

    status = args[0]
    headers = "HTTP/1.0 {status}\n" \
              "Server: menial\n".format(status=status)

    for key, value in args[1]:
        headers += key + ": " + value + "\n"

    print(headers)


def dummy(*args):
    pass


try:
    content = application(environ, get_response_headers).content.decode("utf-8")
    print(content)
except AttributeError:
    content = application(environ, dummy).streaming_content
    for item in content:
        print(item.decode("utf-8"))

