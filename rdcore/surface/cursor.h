#pragma once

#include <rdapi/renderer/surface.h>
#include <unordered_set>
#include <stack>
#include "../object.h"

class Surface;

bool operator==(RDSurfacePos pos1, RDSurfacePos pos2);
bool operator!=(RDSurfacePos pos1, RDSurfacePos pos2);

class CursorHistory
{
    private:
        using History = std::stack<rd_address>;

    public:
        CursorHistory() = default;
        CursorHistory(const History& backstack, const History& forwardstack);
        History& backStack();
        History& forwardStack();
        bool canGoForward() const;
        bool canGoBack() const;

    private:
        History m_hback, m_hforward;
};

class Cursor: public Object
{

    public:
        Cursor(Surface* surface, Context* ctx);
        RDSurfacePos position() const;
        RDSurfacePos selection() const;
        RDSurfacePos startSelection() const;
        RDSurfacePos endSelection() const;
        rd_address currentAddress() const;
        const CursorHistory* history() const;
        int currentRow() const;
        int currentColumn() const;
        bool hasSelection() const;
        bool canGoForward() const;
        bool canGoBack() const;
        void clearSelection();
        void goForward();
        void goBack();
        void setCurrentAddress(rd_address address);
        void set(int row, int col);
        void moveTo(int row, int col);
        void select(int row, int col);
        void updateHistory();

    private:
        void moveTo(int row, int col, bool notify);
        void select(int row, int col, bool notify);

    private:
        CursorHistory m_history;
        Surface* m_surface;
        rd_address m_currentaddress{RD_NVAL};
        RDSurfacePos m_position{0, 0}, m_selection{0, 0};
};

typedef std::shared_ptr<Cursor> CursorPtr;
