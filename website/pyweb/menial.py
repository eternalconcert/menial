from request import App, redirect, render, url_for

App.static_files_dir = "website/pyweb/static/"
App.static_files_url = "/static/"
App.secret_key = "f894y56nfvsdjklgbnP*&Q#T45r35UTTTsdsdddpbgi;rdetg789"

app = App()

with open('website/pyweb/templates/base.html', 'r') as f:
    template = f.read()


@app.route("/")
def index(request):
    response = template.format(title="Python test page: index", body="This is the fdgdfgindex")
    return render(response)

application = app.run
