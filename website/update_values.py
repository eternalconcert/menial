from datetime import datetime
import json
from hashlib import sha256


with open('build/menial.bin', 'rb') as menial:
    menial_hash = sha256(menial.read()).hexdigest()
    menial_hash = menial_hash[:32] + "<wbr>" + menial_hash[32:]


with open('menial.tar.gz', 'rb') as menial_tar_gz:
    menial_tar_gz_hash = sha256(menial_tar_gz.read()).hexdigest()
    menial_tar_gz_hash = menial_tar_gz_hash[:32] + "<wbr>" + menial_tar_gz_hash[32:]

with open('website/hashvalues.json', 'w+') as f:
    timestamp = datetime.strftime(datetime.now(), "%Y-%m-%d %H:%M:%S")
    d = {
        "menial_hash": menial_hash,
        "menial_tar_gz_hash": menial_tar_gz_hash,
        "timestamp": timestamp
    }
    f.write(json.dumps(d))
