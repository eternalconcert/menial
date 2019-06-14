from request import App, redirect, render, url_for

App.static_files_dir = ""
App.static_files_url = ""
App.secret_key = "f894y56nfvsdjklgbnP*&Q#T45r35UTTTsdsdddpbgi;rdetg789"

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

application = app.run
