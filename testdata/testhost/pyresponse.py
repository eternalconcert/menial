import sys
sys.path.append("/home/christian/Projekte/menial/src/python/")

from request import request

import os
import urllib2
import json

from jinja2 import Environment, FileSystemLoader
env = Environment(loader=FileSystemLoader(os.path.abspath(os.getcwd())))

up = request._get_post_params().get('up', False)
down = request._get_post_params().get('down', False)



template = env.get_template("testdata/testhost/templates/test.html")
context = {}
print(template.render(context))

import subprocess

if up:
    subprocess.call(['pactl', '--', 'set-sink-volume', '0', '+10%'])
if down:
    subprocess.call(['pactl', '--', 'set-sink-volume', '0', '-10%'])
