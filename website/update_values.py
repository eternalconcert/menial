from datetime import datetime
import json
from hashlib import sha256


with open('build/menial', 'r') as menial:
    menial_hash = sha256(menial.read()).hexdigest()

with open('website/hashvalues.json', 'w+') as f:
    timestamp = datetime.strftime(datetime.now(), "%Y-%m-%d %H:%M:%S")
    d = {
        "menial_hash": menial_hash,
        "timestamp": timestamp
    }
    f.write(json.dumps(d))
