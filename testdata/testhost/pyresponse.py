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


@app.route("/params/")
def get_params():
    name = request.get['name']
    response = template.format(title="Python test page: " + name, body="")
    _id = request.get.get('id')
    if _id:
        response = template.format(title="Python test page: Name: " + name + ", ID: " + _id, body="")
    return response


@app.route("/items/<id>/edit/")
def parameterized(id):
    response = template.format(title="Item to edit: " + id, body="")
    return response


@app.route("/customers/<name>/orders/<id>/")
def multiparams(name, id):
    response = template.format(title="Customer: " + name + " - " + " Order: " + id, body="")
    return response

@app.route("/suppliers/<str:name>/offers/<int:id>/taxrate/<float:rate>/")
def typed_variables(name, id, rate):
    assert isinstance(name, str)
    assert isinstance(id, int)
    assert isinstance(rate, float)
    title = "Supplier: %s - Offers: %s (Tax rate: %s)" % (name, id, rate)
    response = template.format(title=title, body="")
    return response


@app.route("/post_form/")
def show_form():
    body = """
    <form action="/process_post/" method="POST">
        <input type="text" name="name" />
        <input type="password" name="password" />
        <button id="submit">Submit</button>
    </form>
    """
    response = template.format(title="Post form", body=body)
    return response


@app.route("/process_post/")
def process_post():
    if request.method == "POST":
        name = request.post['name']
        password = request.post['password']
        response = template.format(title="Name: " + name + ", Password: " + password, body="<h2>" + request.method + "</h2>")
    elif request.method == "GET":
        response = template.format(title="", body="<h2>" + request.method + "</h2>")
    return response


@app.route("/no/trailing/slash/")
def addition():
    return template.format(title="Matched it anyway", body="")


@app.route("/session/write/<name>/")
def write(name):
    request.session["name"] = name
    return ""


@app.route("/session/read/")
def write():
    response = template.format(title=request.session["name"], body="")
    return response


app()
