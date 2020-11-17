#pragma once

#include "../object.h"
#include <rdapi/renderer/surface.h>
#include <unordered_set>
#include <stack>

class Surface;

class Cursor: public Object
{
    private:
        typedef std::stack<RDDocumentItem> History;

    public:
        Cursor(Context* ctx);
        const RDSurfacePos* position() const;
        const RDSurfacePos* selection() const;
        const RDSurfacePos* startSelection() const;
        const RDSurfacePos* endSelection() const;
        const RDDocumentItem& currentItem() const;
        int currentRow() const;
        int currentColumn() const;
        bool hasSelection() const;
        bool canGoForward() const;
        bool canGoBack() const;
        void clearSelection();
        void goForward();
        void goBack();
        void setCurrentItem(const RDDocumentItem& item);
        void set(int row, int col);
        void moveTo(int row, int col);
        void select(int row, int col);
        void updateHistory();
        void attach(Surface* s);
        void detach(Surface* s);

    private:
        void moveSurfaces(const RDDocumentItem* item);
        void notifyHistoryChanged();
        void notifyPositionChanged();

    private:
        static bool equalPos(const RDSurfacePos* pos1, const RDSurfacePos* pos2);
        void moveTo(int row, int col, bool notify);
        void select(int row, int col, bool notify);

    private:
        RDDocumentItem m_currentitem{ };
        std::unordered_set<Surface*> m_surfaces;
        RDSurfacePos m_position{0, 0}, m_selection{0, 0};
        History m_hback, m_hforward;
};

typedef std::shared_ptr<Cursor> CursorPtr;
