from pathlib import Path
from cparser import CParser
import json
import sys
import os


categories = {}


def generate_categories(inputdir):
    global categories

    headers = Path(inputdir).glob("**/*.h")
    categories = {h.stem.lower().capitalize(): h for h in headers}


def generate_json(outputdir):
    global categories
    docjson = {"categories": {}}

    for name, file in categories.items():
        p = CParser(file)

        doc = {}
        doc["handles"] = p.handles
        doc["enums"] = p.enums
        doc["callbacks"] = p.callbacks
        doc["structs"] = p.structs
        doc["functions"] = p.functions

        rpath = str(file)
        i = rpath.index("rdapi" + os.path.sep)
        doc["path"] = rpath[i:]

        docjson["categories"][name] = doc

    pathout = Path(outputdir)
    pathout.mkdir(parents=True, exist_ok=True)

    with open(Path(pathout, "doc.json"), "w") as f:
        json.dump(docjson, f, indent=2)


def generate_doc(inputdir, outputdir):
    global categories
    generate_categories(inputdir)
    generate_json(outputdir)


if __name__ == "__main__":
    generate_doc(sys.argv[1], sys.argv[2])
