#include "dex.h"
#include "dex_statemachine.h"
#include "dex_constants.h"
#include "dex_utils.h"
#include <cctype>

#define IMPORT_SECTION_ADDRESS        0x10000000
#define IMPORT_SECTION_SIZE           0x1000000

namespace REDasm {

LOADER_PLUGIN_TEST(DEXLoader, DEXHeader)
{
    if(!DEXLoader::validateSignature(header) || (!header->data_off || !header->data_size))
        return false;

    if((!header->type_ids_off || !header->type_ids_size) || (!header->string_ids_off || !header->string_ids_size))
        return false;

    if((!header->method_ids_off || !header->method_ids_size) || (!header->proto_ids_off || !header->proto_ids_size))
        return false;

    return true;
}

const std::string DEXLoader::m_invalidstring;

DEXLoader::DEXLoader(AbstractBuffer *buffer): LoaderPluginT<DEXHeader>(buffer), m_types(nullptr), m_strings(nullptr), m_methods(nullptr), m_fields(nullptr), m_protos(nullptr)
{
    m_importbase = IMPORT_SECTION_ADDRESS;
}

std::string DEXLoader::assembler() const { return "dalvik"; }

void DEXLoader::load()
{
    REDasm::log("Loading DEX Version " + std::string(m_header->version, 3));

    m_types = pointer<DEXTypeIdItem>(m_header->type_ids_off);
    m_strings = pointer<DEXStringIdItem>(m_header->string_ids_off);
    m_methods = pointer<DEXMethodIdItem>(m_header->method_ids_off);
    m_protos = pointer<DEXProtoIdItem>(m_header->proto_ids_off);

    if(m_header->field_ids_off && m_header->field_ids_size)
        m_fields = pointer<DEXFieldIdItem>(m_header->field_ids_off);

    m_document->segment("CODE", m_header->data_off, m_header->data_off, m_header->data_size, SegmentType::Code);
    m_document->segment("IMPORT", 0, IMPORT_SECTION_ADDRESS, IMPORT_SECTION_SIZE, SegmentType::Bss);

    DEXClassIdItem* dexclasses = pointer<DEXClassIdItem>(m_header->class_defs_off);
    this->filterClasses(dexclasses);
}

bool DEXLoader::getMethodOffset(u64 idx, offset_t &offset) const
{
    auto it = m_codeitems.find(idx);

    if(it == m_codeitems.end())
        return false;

    DEXCodeItem* dexcode = it->second;
    offset = fileoffset(&dexcode->insns);
    return true;
}

bool DEXLoader::getStringOffset(u64 idx, offset_t& offset) const
{
    if(!m_strings || (idx >= m_header->string_ids_size))
        return false;

    u8* pstringdata = pointer<u8>(m_strings[idx].string_data_off);
    DEXUtils::getULeb128(&pstringdata);
    offset = fileoffset(pstringdata);
    return true;
}

const std::string &DEXLoader::getString(u64 idx)
{
    if(!m_strings)
        return m_invalidstring;

    return cacheEntry(idx, m_cachedstrings, [=](std::string& s) {
        u8* pstringdata = pointer<u8>(m_strings[idx].string_data_off);
        u32 len = DEXUtils::getULeb128(&pstringdata);
        s = std::string(reinterpret_cast<const char*>(pstringdata), len);
    });
}

const std::string& DEXLoader::getType(u64 idx, bool full)
{
    return cacheEntry(idx, m_cachedtypes, [&](std::string& s) {
        if(idx >= m_header->type_ids_size) {
            s = "type_" + std::to_string(idx);
            return;
        }

        const DEXTypeIdItem& dextype = m_types[idx];
        s = this->getNormalizedString(dextype.descriptor_idx);

        if(full)
            return;

        // Strip full qualified name
        size_t idx = s.find_last_of(".");

        if(idx != std::string::npos)
            s = s.substr(idx + 1);
    });
}

const std::string& DEXLoader::getMethodName(u64 idx)
{
    return cacheEntry(idx, m_cachedmethodnames, [&](std::string& s) {
        if(idx >= m_header->method_ids_size) {
            s = "method_" + std::to_string(idx);
            return;
        }

        const DEXMethodIdItem& dexmethod = m_methods[idx];
        s = this->getType(dexmethod.class_idx) + "." + this->getNormalizedString(dexmethod.name_idx);
    });
}

const std::string& DEXLoader::getMethodProto(u64 idx)
{
    return cacheEntry(idx, m_cachedmethodproto, [&](std::string& s) {
        s = this->getMethodName(idx) + this->getParameters(idx) + ":" + this->getReturnType(idx);
    });
}

const std::string& DEXLoader::getField(u64 idx)
{
    return cacheEntry(idx, m_cachedfields, [&](std::string& s) {
        if(!m_fields || (idx >= m_header->field_ids_size)) {
            s = "field_" + std::to_string(idx);
            return;
        }

        const DEXFieldIdItem& dexfield = m_fields[idx];
        s = this->getType(dexfield.class_idx) + "." + this->getNormalizedString(dexfield.name_idx) + ":" + this->getType(dexfield.type_idx);
    });
}

const std::string& DEXLoader::getReturnType(u64 methodidx)
{
    if(methodidx >= m_header->method_ids_size)
        return m_invalidstring;

    const DEXMethodIdItem& dexmethod = m_methods[methodidx];
    const DEXProtoIdItem& dexproto = m_protos[dexmethod.proto_idx];

    return this->getNormalizedString(m_types[dexproto.return_type_idx].descriptor_idx);
}

const std::string& DEXLoader::getParameters(u64 methodidx)
{
    if(methodidx >= m_header->method_ids_size)
        return m_invalidstring;

    return this->cacheEntry(methodidx, m_cachedparameters, [&](std::string& s) {
        const DEXMethodIdItem& dexmethod = m_methods[methodidx];
        const DEXProtoIdItem& dexproto = m_protos[dexmethod.proto_idx];

        if(!dexproto.parameters_off)
            s = "()";
        else
            s = "(" + this->getTypeList(dexproto.parameters_off) + ")";
    });
}

bool DEXLoader::getMethodInfo(u64 methodidx, DEXEncodedMethod &dexmethod)
{
    auto it = m_encmethods.find(methodidx);

    if(it == m_encmethods.end())
        return false;

    dexmethod = it->second;
    return true;
}

bool DEXLoader::getDebugInfo(u64 methodidx, DEXDebugInfo &debuginfo)
{
    auto it = m_codeitems.find(methodidx);

    if(it == m_codeitems.end())
        return false;

    DEXCodeItem* dexcode = it->second;

    if(!dexcode->debug_info_off)
        return false;

    u8* pdebuginfo = pointer<u8>(dexcode->debug_info_off);
    debuginfo.line_start = DEXUtils::getULeb128(&pdebuginfo);
    debuginfo.parameters_size = DEXUtils::getULeb128(&pdebuginfo);

    for(u32 i = 0; i < debuginfo.parameters_size; i++)
    {
        s32 idx = DEXUtils::getULeb128p1(&pdebuginfo);

        if(idx == DEX_NO_INDEX)
            debuginfo.parameter_names.push_back(std::string());
        else
            debuginfo.parameter_names.push_back(this->getNormalizedString(idx));
    }

    DEXStateMachine dexstatemachine(fileoffset(&dexcode->insns), debuginfo);
    dexstatemachine.execute(pdebuginfo);
    return true;
}

u32 DEXLoader::getMethodSize(u32 methodidx) const { return m_codeitems.at(methodidx)->insn_size * sizeof(u16); }

address_t DEXLoader::nextImport(address_t *res)
{
    address_t importbase = m_importbase;
    m_importbase += sizeof(u16);

    if(res)
        *res = importbase;

    return importbase;
}

bool DEXLoader::getClassData(const DEXClassIdItem &dexclass, DEXClassData &dexclassdata)
{
    if(!dexclass.class_data_off)
        return false;

    DEXEncodedField dexfield;
    DEXEncodedMethod dexmethod;
    u8* pclassdata = pointer<u8>(dexclass.class_data_off);

    dexclassdata.static_fields_size = DEXUtils::getULeb128(&pclassdata);
    dexclassdata.instance_fields_size = DEXUtils::getULeb128(&pclassdata);
    dexclassdata.direct_methods_size = DEXUtils::getULeb128(&pclassdata);
    dexclassdata.virtual_methods_size = DEXUtils::getULeb128(&pclassdata);

    for(u32 i = 0; i < dexclassdata.static_fields_size; i++)
    {
        dexfield.field_idx_diff = DEXUtils::getULeb128(&pclassdata);
        dexfield.access_flags = DEXUtils::getULeb128(&pclassdata);
        dexclassdata.static_fields.push_back(dexfield);
    }

    for(u32 i = 0; i < dexclassdata.instance_fields_size; i++)
    {
        dexfield.field_idx_diff = DEXUtils::getULeb128(&pclassdata);
        dexfield.access_flags = DEXUtils::getULeb128(&pclassdata);
        dexclassdata.instance_fields.push_back(dexfield);
    }

    for(u32 i = 0; i < dexclassdata.direct_methods_size; i++)
    {
        dexmethod.method_idx_diff = DEXUtils::getULeb128(&pclassdata);
        dexmethod.access_flags = DEXUtils::getULeb128(&pclassdata);
        dexmethod.code_off = DEXUtils::getULeb128(&pclassdata);
        dexclassdata.direct_methods.push_back(dexmethod);
    }

    for(u32 i = 0; i < dexclassdata.virtual_methods_size; i++)
    {
        dexmethod.method_idx_diff = DEXUtils::getULeb128(&pclassdata);
        dexmethod.access_flags = DEXUtils::getULeb128(&pclassdata);
        dexmethod.code_off = DEXUtils::getULeb128(&pclassdata);
        dexclassdata.virtual_methods.push_back(dexmethod);
    }

    return true;
}

void DEXLoader::loadMethod(const DEXEncodedMethod &dexmethod, u16& idx, bool filter)
{
    if(!dexmethod.code_off)
        return;

    if(!idx)
        idx = dexmethod.method_idx_diff;
    else
        idx += dexmethod.method_idx_diff;

    DEXCodeItem* dexcode = pointer<DEXCodeItem>(dexmethod.code_off);

    m_encmethods[idx] = dexmethod;
    m_codeitems[idx] = dexcode;

    const std::string& methodname = this->getMethodName(idx);

    if(filter)
        m_document->lock(fileoffset(&dexcode->insns), methodname, SymbolType::Import, idx);
    else
        m_document->symbol(fileoffset(&dexcode->insns), methodname, SymbolType::ExportFunction, idx);
}

void DEXLoader::loadClass(const DEXClassIdItem &dexclass, bool filter)
{
    DEXClassData dexclassdata;

    if(!this->getClassData(dexclass, dexclassdata))
        return;

    u16 idx = 0;

    std::for_each(dexclassdata.direct_methods.begin(), dexclassdata.direct_methods.end(), [&](const DEXEncodedMethod& dexmethod) {
        this->loadMethod(dexmethod, idx, filter);
    });

    idx = 0;

    std::for_each(dexclassdata.virtual_methods.begin(), dexclassdata.virtual_methods.end(), [&](const DEXEncodedMethod& dexmethod) {
        this->loadMethod(dexmethod, idx, filter);
    });
}

void DEXLoader::filterClasses(const DEXClassIdItem *dexclasses)
{
    UI::CheckList items;

    for(u32 i = 0; i < m_header->class_defs_size; i++)
    {
        const std::string& classtype = this->getType(dexclasses[i].class_idx, true);
        bool precheck = true;

        if(!classtype.find("android.") || !classtype.find("com.google.")) // Apply prefiltering
            precheck = false;

        items.push_back({ classtype , precheck });
    }

    r_ui->checkList("Class Loader", "Select one or more classes from the list below", items);

    for(u32 i = 0; i < m_header->class_defs_size; i++)
        this->loadClass(dexclasses[i], !items[i].second);
}

const std::string& DEXLoader::getNormalizedString(u64 idx)
{
    return cacheEntry(idx, m_cachednstrings, [&](std::string& s) {
        s = this->normalized(this->getString(idx));
    });
}

const std::string& DEXLoader::getTypeList(u64 typelistoff)
{
    return cacheEntry(typelistoff, m_cachedtypelist, [=](std::string& s) {
        u32 size = *pointer<u32>(typelistoff);
        DEXTypeItem* dextypeitem = pointer<DEXTypeItem>(typelistoff + sizeof(u32));

        for(u32 i = 0; i < size; i++) {
            if(i)
                s += ", ";

            s += this->getType(dextypeitem[i].type_idx);
        }
    });
}

const std::string &DEXLoader::cacheEntry(u64 idx, std::unordered_map<u64, std::string> &cache, const std::function<void(std::string&)> &cb)
{
    auto it = cache.find(idx);

    if(it != cache.end())
        return it->second;

    std::string s;
    cb(s);

    auto iit = cache.emplace(idx, std::move(s));
    return iit.first->second;
}

bool DEXLoader::validateSignature(const REDasm::DEXHeader *header)
{
    if(strncmp(header->dex, DEX_FILE_MAGIC, 3))
        return false;

    if(header->newline != '\n')
        return false;

    for(u32 i = 0; i < 3; i++)
    {
        if(!std::isdigit(header->version[i]))
            return false;
    }

    if(header->zero != '\0')
        return false;

    return true;
}

std::string DEXLoader::normalized(const std::string &type)
{
    if(type[0] == '[')
        return DEXLoader::normalized(type.substr(1)) + "[]";

    if(type == "V")
        return "void";
    if(type == "Z")
        return "boolean";
    if(type == "B")
        return "byte";
    if(type == "S")
        return "short";
    if(type == "C")
        return "char";
    if(type == "I")
        return "int";
    if(type == "J")
        return "long";
    if(type == "F")
        return "float";
    if(type == "D")
        return "double";

    std::string s = type;

    if(s.front() == 'L')
       s.erase(s.begin());

    if(s.back() == ';')
        s.pop_back();

    std::replace(s.begin(), s.end(), '/', '.');
    return s;
}

} // namespace REDasm
