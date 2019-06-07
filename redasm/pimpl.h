#pragma once

// https://www.modernescpp.com/index.php/c-core-guidelines-interfaces-ii
// https://wiki.qt.io/D-Pointer#The_d-pointer

#include <memory>

namespace PImplDetail {

template <typename T> static inline T *pimpl_get_ptr_helper(T *ptr) { return ptr; }
template <typename Wrapper> static inline typename Wrapper::element_type* pimpl_get_ptr_helper(const Wrapper &ptr) { return ptr.get(); }

} // namespace Detail

#define PIMPL_DECLARE_P(classname) std::shared_ptr<classname##Impl> m_pimpl_p;
#define PIMPL_DECLARE_Q(classname) classname* m_pimpl_q;

#define PIMPL_DECLARE_PRIVATE(classname) \
    PIMPL_DECLARE_P(classname) \
    inline classname##Impl* pimpl_p() { \
        return reinterpret_cast<classname##Impl*>(PImplDetail::pimpl_get_ptr_helper(m_pimpl_p)); \
    } \
    inline const classname##Impl* pimpl_p() const { \
        return reinterpret_cast<const classname##Impl *>(PImplDetail::pimpl_get_ptr_helper(m_pimpl_p)); \
    } \
    friend class classname##Impl;

#define PIMPL_DECLARE_PUBLIC(classname) \
    PIMPL_DECLARE_Q(classname) \
    inline classname* pimpl_q() { return static_cast<classname*>(m_pimpl_q); } \
    inline const classname* pimpl_q() const { return static_cast<const classname*>(m_pimpl_q); } \
    friend class classname;

#define PIMPL_P(classname) classname##Impl* const p = pimpl_p()
#define PIMPL_Q(classname) classname* const q = pimpl_q()
