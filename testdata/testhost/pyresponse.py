import sys
sys.path.append("../src/python/")

from request import request, App


app = App()

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

@app.route("/")
def index():
    response = template.format(title="Python test page: index", body="This is the index")
    return response


@app.route("/sub/")
def subpage():
    response = template.format(title="Python test page: sub", body="This is a random sub page")
    return response


@app.route("/params/?name=Christian")
def get_params():
    name = request.get['name']
    response = template.format(title="Python test page: " + name, body="")
    return response


@app.route("/items/<id>/edit/")
def parameterized(id):
    response = template.format(title="Item to edit: " + id, body="")
    return response

app(request)
