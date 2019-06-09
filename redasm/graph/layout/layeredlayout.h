#pragma once

// Sugiyama Layout Theory
// - https://drive.google.com/file/d/1uAAch1SxLLVBJ53ZX-zX4AnwzwhcXcEM/view
// - http://publications.lib.chalmers.se/records/fulltext/161388.pdf
// - http://www.graphviz.org/Documentation/TSE93.pdf
// - https://blog.disy.net/sugiyama-method

// Layout based on x64dbg's DisassemblerGraphView
// - https://github.com/x64dbg/x64dbg/blob/development/src/gui/Src/Gui/DisassemblerGraphView.h
// - https://github.com/x64dbg/x64dbg/blob/development/src/gui/Src/Gui/DisassemblerGraphView.cpp

#include "abstractlayout.h"
#include "../../pimpl.h"

namespace REDasm {
namespace Graphing {

class LayeredLayoutImpl;

enum class LayeredLayoutType { Wide, Medium, Narrow };

class LIBREDASM_API LayeredLayout: public AbstractLayout
{
    PIMPL_DECLARE_P(LayeredLayout)
    PIMPL_DECLARE_PRIVATE(LayeredLayout)

    public:
        LayeredLayout(Graph* graph);
        void setLayoutType(LayeredLayoutType lt);
        bool execute() override;
};

} // namespace Graphing
} // namespace REDasm
