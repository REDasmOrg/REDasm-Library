import re


HANDLE_REGEX = r"DECLARE_HANDLE\((.+)\)"
ENUM_REGEX = r"enum (\w+) \{([\s\S]*?)\};"
FUNCTION_REGEX = r"RD_API_EXPORT (.+) ((RD\w+)\((.+)\));"


class CParser:
    def __init__(self, filepath):
        self._handles = []
        self._enums = []
        self._functions = []

        with open(filepath, "r") as f:
            self._content = f.read()

        self.__find_handles()
        self.__find_enums()
        self.__find_functions()

    def __find_handles(self):
        for m in re.finditer(HANDLE_REGEX, self._content):
            self._handles.append(m[1])

    def __find_enums(self):
        r = re.compile(ENUM_REGEX, re.MULTILINE)
        rgxstate = re.compile(r"([A-Za-z_]\w+)(?=[ ]+=|,)")

        for m in re.finditer(r, self._content):
            obj = {"name": m[1],
                   "states": []}

            statelines = m[2].strip().splitlines(False)

            for s in statelines:
                if s.strip().startswith("//"):
                    continue

                m = re.search(r"\w+", s)

                if m:
                    obj["states"].append(m[0])

            self._enums.append(obj)

    def __find_functions(self):
        for m in re.finditer(FUNCTION_REGEX, self._content):
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
    def handles(self):
        return self._handles

    @property
    def enums(self):
        return self._enums

    @property
    def functions(self):
        return self._functions
