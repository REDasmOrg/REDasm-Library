#include "functiongraph_impl.h"

namespace REDasm {
namespace Graphing {

FunctionBasicBlockImpl::FunctionBasicBlockImpl(): m_startidx(REDasm::npos), m_endidx(m_startidx) { }
FunctionBasicBlockImpl::FunctionBasicBlockImpl(size_t startidx): m_startidx(startidx), m_endidx(startidx) { }


FunctionGraphImpl::FunctionGraphImpl()
{

}

} // namespace Graphing
} // namespace REDasm
