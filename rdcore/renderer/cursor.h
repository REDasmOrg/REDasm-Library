#pragma once

#include "../object.h"
#include <rdapi/renderer/surface.h>
#include <stack>

class Cursor: public Object
{
    public:
        typedef std::stack<RDSurfacePos> PositionStack;

    public:
        Cursor(Context* ctx);
        void enable();
        void disable();
        void goBack();
        void goForward();
        void clearSelection();
        int currentRow() const;
        int currentColumn() const;
        int selectionLine() const;
        int selectionColumn() const;
        bool isRowSelected(int row) const;
        bool hasSelection() const;
        bool canGoBack() const;
        bool canGoForward() const;
        bool active() const;
        void set(int row, int col);
        virtual void moveTo(int row, int col);
        virtual void select(int row, int col);
        const RDSurfacePos* position() const;
        const RDSurfacePos* selection() const;
        const RDSurfacePos* startSelection() const;
        const RDSurfacePos* endSelection() const;

    protected:
        virtual void onStackChanged() = 0;
        virtual void onPositionChanged() = 0;

    private:
        void moveTo(int row, int col, bool save, bool notify);
        void select(int row, int col, bool notify);
        static bool equalPos(const RDSurfacePos* pos1, const RDSurfacePos* pos2);

    private:
        RDSurfacePos m_position{0, 0}, m_selection{0, 0};
        PositionStack m_backstack, m_forwardstack;
        bool m_active{false};
};

