#pragma once

#include <optional>
#include <vector>
#include <mutex>
#include <rdapi/renderer/surface.h>
#include "../document/document_fwd.h"
#include "cursor.h"

class ItemContainer;
class Renderer;

class Surface: public Cursor
{
    private:
        struct SurfaceRow {
            std::deque<std::string> chunks;
            RDDocumentItem item;
            int length;
        };

        typedef std::scoped_lock<std::mutex> SurfaceLock;

    public:
        Surface(Context* ctx, rd_flag flags);
        ~Surface();
        int row(int row, RDSurfaceCell* cells) const;
        const std::string* currentWord() const;
        bool getCurrentItem(RDDocumentItem* item) const;
        bool getSelectedSymbol(RDSymbol* symbol) const;
        bool goTo(const RDDocumentItem* item);
        bool goToAddress(rd_address address);
        void getSize(int* rows, int* cols) const;
        void scroll(int nrows, int ncols);
        void resize(int rows, int cols);
        void moveTo(int row, int col) override;
        void select(int row, int col) override;

    protected:
        void handleEvents(const RDEventArgs* event);
        void onCursorChanged() override;

    private:
        const SurfaceRow* currentSurfaceRow() const;
        RDSurfaceCell* cell(size_t row, size_t col);
        SafeDocument& document() const;
        const ItemContainer* items() const;
        void notifyChanged();
        void checkColumn(int row, int& column) const;
        void drawRow(int row, const Renderer& st, SurfaceRow& sfrow);
        void drawCursor();
        void highlightWords();
        void checkSelection(int row, int col, u8* bg, u8* fg);
        bool hasFlag(rd_flag flag) const;
        bool inRange(const RDDocumentItem* item) const;
        void scrollRows(int nrows);
        void draw();

    private:
        mutable std::mutex m_mutex;
        std::vector<RDSurfaceCell> m_surface;
        std::unordered_map<int, SurfaceRow> m_visiblerows;
        std::pair<RDDocumentItem, RDDocumentItem> m_items{ };
        int m_rows{0}, m_cols{0}, m_commentcolumn{0};
        rd_flag m_flags;
};

