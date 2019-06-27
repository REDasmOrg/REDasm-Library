#include "loadrequest_impl.h"

namespace REDasm {

LoadRequestImpl::LoadRequestImpl(const String &filepath, AbstractBuffer *buffer): m_filepath(filepath), m_buffer(buffer), m_view(buffer->view()) { }
String LoadRequestImpl::filePath() const { return m_filepath; }
AbstractBuffer *LoadRequestImpl::buffer() const { return m_buffer; }
const BufferView &LoadRequestImpl::view() const { return m_view; }

} // namespace REDasm
