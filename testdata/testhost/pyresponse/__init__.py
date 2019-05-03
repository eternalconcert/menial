import re
import json
import os
from random import randint

import sys
from request import App, redirect, render, url_for


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
def index(request):
    response = template.format(title="Python test page: index", body="This is the index")
    return render(response)


@app.route("/sub/")
def subpage(request):
    response = template.format(title="Python test page: sub", body="This is a random sub page")
    return render(response)


@app.route("/params/")
def get_params(request):
    name = request.get['name']
    response = template.format(title="Python test page: " + name, body="")
    _id = request.get.get('id')
    if _id:
        response = template.format(title="Python test page: Name: " + name + ", ID: " + _id, body="")
    return render(response)


@app.route("/items/<id>/edit/")
def parameterized(request, id):
    response = template.format(title="Item to edit: " + id, body="")
    return render(response)


@app.route("/customers/<name>/orders/<id>/")
def multiparams(request, name, id):
    response = template.format(title="Customer: " + name + " - " + " Order: " + id, body="")
    return render(response)

@app.route("/suppliers/<str:name>/offers/<int:id>/taxrate/<float:rate>/")
def typed_variables(request, name, id, rate):
    assert isinstance(name, str)
    assert isinstance(id, int)
    assert isinstance(rate, float)
    title = "Supplier: %s - Offers: %s (Tax rate: %s)" % (name, id, rate)
    response = template.format(title=title, body="")
    return render(response)


@app.route("/post_form/")
def show_form(request):
    body = """
    <form action="/process_post/" method="POST">
        <input type="text" name="name" />
        <input type="password" name="password" />
        <button id="submit">Submit</button>
    </form>
    """
    response = template.format(title="Post form", body=body)
    return render(response)


@app.route("/process_post/")
def process_post(request):
    print("5ytretertertert")
    if request.method == "POST":
        name = request.post['name']
        password = request.post['password']
        response = template.format(title="Name: " + name + ", Password: " + password, body="<h2>" + request.method + "</h2>")
    elif request.method == "GET":
        response = template.format(title="", body="<h2>" + request.method + "</h2>")
    return render(response)


@app.route("/no/trailing/slash/")
def addition(request):
    return render(template.format(title="Matched it anyway", body=""))


# @app.route("/session/write/<name>/")
# def write(request, name):
#     request.session["name"] = name
#     return render("")
#
#
# @app.route("/session/read/")
# def write(request):
#     response = template.format(title=request.session["name"], body="")
#     return render(response)


@app.route("/redirect/")
def redirect_to_another_location(request):
    return redirect(url_for('new_location'))


@app.route("/new_location/")
def new_location(request):
    response = template.format(title="Redirected from another location", body="")
    return render(response)


@app.route("/redirect/with/variable/part/")
def redirect_to_another_location_with_variable_part(request):
    return redirect(url_for('new_location_with_variable_part', "christian"))


@app.route("/new_location/<str:name>/")
def new_location_with_variable_part(request, name):
    response = template.format(title="Redirected from another location: " + name, body="")
    return render(response)


application = app.run
