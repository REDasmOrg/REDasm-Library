#ifndef DEX_H
#define DEX_H

// https://source.android.com/devices/tech/dalvik/dex-format

#include "../../plugins/plugins.h"
#include "../../assemblers/dalvik/dalvik_metadata.h"
#include "dex_header.h"

namespace REDasm {

class DEXFormat : public FormatPluginT<DEXHeader>
{
    DEFINE_FORMAT_PLUGIN_TEST(DEXHeader)

    public:
        DEXFormat(Buffer& buffer);
        virtual std::string name() const;
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual endianness_t endianness() const;
        virtual void load();

    public:
        bool getMethodOffset(u64 idx, offset_t &offset) const;
        bool getStringOffset(u64 idx, offset_t &offset) const;
        const std::string& getString(u64 idx);
        const std::string& getType(u64 idx);
        const std::string& getMethodName(u64 idx);
        const std::string& getMethodProto(u64 idx);
        const std::string& getField(u64 idx);
        const std::string& getReturnType(u64 methodidx);
        const std::string& getParameters(u64 methodidx);
        bool getMethodInfo(u64 methodidx, DEXEncodedMethod& dexmethod);
        bool getDebugInfo(u64 methodidx, DEXDebugInfo& debuginfo);
        u32 getMethodSize(u32 methodidx) const;
        address_t nextImport(address_t *res = NULL);

    private:
        bool getClassData(const DEXClassIdItem& dexclass, DEXClassData& dexclassdata);
        void loadMethod(const DEXEncodedMethod& dexmethod, u16 &idx);
        void loadClass(const DEXClassIdItem& dexclass);
        const std::string &getNormalizedString(u64 idx);
        const std::string &getTypeList(u64 typelistoff);

    private:
        static const std::string& cacheEntry(u64 idx, std::unordered_map<u64, std::string>& cache, const std::function<void(std::string&)>& cb);
        static std::string normalized(const std::string& type);

    public:
        static bool validateSignature(const DEXHeader *format);

    private:
        u64 m_importbase;
        std::unordered_map<u64, DEXCodeItem*> m_codeitems;
        std::unordered_map<u64, DEXEncodedMethod> m_encmethods;
        DEXTypeIdItem* m_types;
        DEXStringIdItem* m_strings;
        DEXMethodIdItem* m_methods;
        DEXFieldIdItem* m_fields;
        DEXProtoIdItem* m_protos;

    private: // Caching
        static const std::string m_invalidstring;
        std::unordered_map<u64, std::string> m_cachedstrings;
        std::unordered_map<u64, std::string> m_cachednstrings;
        std::unordered_map<u64, std::string> m_cachedfields;
        std::unordered_map<u64, std::string> m_cachedtypes;
        std::unordered_map<u64, std::string> m_cachedtypelist;
        std::unordered_map<u64, std::string> m_cachedparameters;
        std::unordered_map<u64, std::string> m_cachedmethodnames;
        std::unordered_map<u64, std::string> m_cachedmethodproto;
};

DECLARE_FORMAT_PLUGIN(DEXFormat, dex)

} // namespace REDasm

#endif // DEX_H
