import re


RDAPI_REGEX = r"RD_API_EXPORT (.+) ((RD\w+)\((.+)\));"


class CParser:
    def __init__(self, filepath):
        self._functions = []

        with open(filepath, "r") as f:
            self._content = f.read()

        self.__find_functions()

    def __find_functions(self):
        for m in re.finditer(RDAPI_REGEX, self._content):
            f = {"ret": m[1], "def": m[2],
                 "name": m[3], "args": []}

            if m[4] != "void":
                args = [arg.strip() for arg in m[4].split(",")]

                for a in args:
                    parts = a.split(" ")
                    arg = {"name": parts[-1], "type": " ".join(parts[:-1])}
                    f["args"].append(arg)
            else:
                f["def"] = f["def"].replace("(void)", "()")

            self._functions.append(f)

    @property
    def functions(self):
        return self._functions
