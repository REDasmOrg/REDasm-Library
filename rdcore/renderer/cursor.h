#pragma once

#include "../object.h"
#include <rdapi/document/cursor.h>
#include <stack>

class Cursor: public Object
{
    public:
        typedef std::stack<RDCursorPos> PositionStack;

    public:
        Cursor(Context* ctx);
        void toggle();
        void enable();
        void disable();
        void goBack();
        void goForward();
        void clearSelection();
        void set(int row, int col);
        virtual void moveTo(int row, int col);
        virtual void select(int row, int col);

    public:
        const RDCursorPos* position() const;
        const RDCursorPos* selection() const;
        const RDCursorPos* startSelection() const;
        const RDCursorPos* endSelection() const;
        int currentRow() const;
        int currentColumn() const;
        int selectionLine() const;
        int selectionColumn() const;
        bool isRowSelected(int row) const;
        bool hasSelection() const;
        bool canGoBack() const;
        bool canGoForward() const;
        bool active() const;

    protected:
        virtual void onCursorChanged();

    private:
        void moveTo(int row, int column, bool save);
        static bool equalPos(const RDCursorPos* pos1, const RDCursorPos* pos2);

    private:
        RDCursorPos m_position{0, 0}, m_selection{0, 0};
        PositionStack m_backstack, m_forwardstack;
        bool m_active{false};
};

