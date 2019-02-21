import sys
sys.path.append("/home/xgwschk/projects/menial/src/python/")

from request import request, App


app = App()

@app.route("/")
def test():
    print("This is the index!")


@app.route("/test/")
def test():
    print("Test!")


app(request)
