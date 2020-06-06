import re


HANDLE_REGEX = r"DECLARE_HANDLE\((.+)\)"
ENUM_REGEX = r"enum (\w+) \{([\s\S]*?)\};"
STRUCT_REGEX = r"typedef struct \w+ \{([\s\S]*?)\} (\w+);"
FUNCTION_REGEX = r"RD_API_EXPORT (.+) ((RD\w+)\((.+)\));"
CALLBACK_REGEX = r"typedef (.+) \(\*(Callback_.+)\)\((.+)\);"

class CParser:
    def __init__(self, filepath):
        self._handles = []
        self._enums = []
        self._callbacks = {}
        self._structs = []
        self._functions = []

        with open(filepath, "r") as f:
            self._content = f.read()

        self.__find_handles()
        self.__find_enums()
        self.__find_callbacks()
        self.__find_structs()
        self.__find_functions()

    def __find_handles(self):
        for m in re.finditer(HANDLE_REGEX, self._content):
            self._handles.append(m[1])

    def __find_enums(self):
        r = re.compile(ENUM_REGEX, re.MULTILINE)

        for m in re.finditer(r, self._content):
            obj = {"name": m[1],
                   "states": []}

            statelines = m[2].strip().splitlines(False)

            for s in statelines:
                s = s.strip()
                if s.startswith("//"):
                    continue

                m = re.search(r"\w+", s)

                if m:
                    obj["states"].append(m[0])

            self._enums.append(obj)

    def __find_callbacks(self):
        for m in re.finditer(CALLBACK_REGEX, self._content):
            obj = {"ret": m[1], "args": []}

            if m[3] != "void":
                obj["args"] = self.__split_args(m[3])

            self._callbacks[m[2]] = obj

    def __find_structs(self):
        r = re.compile(STRUCT_REGEX, re.MULTILINE)
        rgxfields = re.compile(r"(\w+)[ ]+(\w+)(\[([^\]]+)\])?;")

        for m in re.finditer(r, self._content):
            obj = {"name": m[2],
                   "fields": []}

            fieldlines = m[1].strip().splitlines(False)

            for f in fieldlines:
                f = f.strip()
                if f.startswith("//"):
                    continue

                if f == "RD_USERDATA_FIELD":
                    self.__add_userdata_fields(obj)
                    continue
                elif f == "RD_PLUGIN_HEADER":
                    self.__add_pluginheader_fields(obj)
                    continue
                elif f == "RD_EVENTARGS_BASE":
                    self.__add_eventargs_fields(obj)
                    continue

                fm = re.search(rgxfields, f)

                if not fm:
                    continue

                fieldobj = {"type": fm[1],
                            "name": fm[2],
                            "arraysize": None,
                            "callback": fm[1].startswith("Callback_")}

                if fm[4]:
                    fieldobj["arraysize"] = fm[4]

                obj["fields"].append(fieldobj)

            self._structs.append(obj)

    def __find_functions(self):
        for m in re.finditer(FUNCTION_REGEX, self._content):
            f = {"ret": m[1], "def": m[2],
                 "name": m[3], "args": []}

            if m[4] != "void":
                f["args"] = self.__split_args(m[4])
            else:
                f["def"] = f["def"].replace("(void)", "()")

            self._functions.append(f)

    def __add_userdata_fields(self, obj):
        obj["fields"].append({"type": "void*", "name": "userdata", "arraysize": None, "callback": False})
        obj["fields"].append({"type": "void*", "name": "p_data", "arraysize": None, "arraysize": None, "callback": False})
        obj["fields"].append({"type": "intptr_t", "name": "i_data", "arraysize": None, "callback": False})
        obj["fields"].append({"type": "uintptr_t", "name": "u_data", "arraysize": None, "arraysize": None, "callback": False})
        obj["fields"].append({"type": "const char*", "name": "s_data", "arraysize": None, "callback": False})

    def __add_pluginheader_fields(self, obj):
        obj["fields"].append({"type": "apilevel_t", "name": "apilevel", "arraysize": None, "callback": False})
        obj["fields"].append({"type": "u32", "name": "apibits", "arraysize": None, "callback": False})
        obj["fields"].append({"type": "const char*", "name": "id", "arraysize": None, "callback": False})
        obj["fields"].append({"type": "const char*", "name": "name", "arraysize": None, "callback": False})
        obj["fields"].append({"type": "size_t", "name": "state", "arraysize": None, "callback": False})
        obj["fields"].append({"type": "Callback_PluginInit", "name": "init", "arraysize": None, "callback": True})
        obj["fields"].append({"type": "Callback_PluginFree", "name": "free", "arraysize": None, "callback": True})

    def __add_eventargs_fields(self, obj):
        obj["fields"].append({"type": "eventid_t", "name": "eventid", "arraysize": None, "callback": False})
        obj["fields"].append({"type": "void*", "name": "sender", "arraysize": None, "callback": False})
        obj["fields"].append({"type": "void*", "name": "owner", "arraysize": None, "callback": False})

    def __split_args(self, args):
        res = []
        arglist = [arg.strip() for arg in args.split(",")]

        for a in arglist:
            parts = a.split(" ")
            arg = {"name": parts[-1], "type": " ".join(parts[:-1])}
            res.append(arg)

        return res

    @property
    def handles(self):
        return self._handles

    @property
    def enums(self):
        return self._enums

    @property
    def callbacks(self):
        return self._callbacks

    @property
    def structs(self):
        return self._structs

    @property
    def functions(self):
        return self._functions
