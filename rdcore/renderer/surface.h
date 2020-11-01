#pragma once

#include <optional>
#include <vector>
#include <mutex>
#include "../document/document_fwd.h"
#include "surfacepath.h"
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
        size_t getPath(const RDPathItem** path) const;
        int lastColumn() const;
        int row(int row, RDSurfaceCell* cells) const;
        const std::string* currentWord() const;
        const std::string* wordAt(int row, int col) const;
        const RDDocumentItem* firstItem() const;
        const RDDocumentItem* lastItem() const;
        int findRow(const RDDocumentItem* item) const;
        bool getItem(int row, RDDocumentItem* item) const;
        bool contains(const RDDocumentItem* item) const;
        bool currentItem(RDDocumentItem* item) const;
        bool currentSymbol(RDSymbol* symbol) const;
        bool symbolAt(int row, int col, RDSymbol* symbol) const;
        bool goTo(const RDDocumentItem* item);
        bool goToAddress(rd_address address);
        void getSize(int* rows, int* cols) const;
        void scroll(int nrows, int ncols);
        void resize(int rows, int cols);
        void moveTo(int row, int col) override;
        void select(int row, int col) override;
        void update();

    protected:
        void handleEvents(const RDEventArgs* event);
        void onCursorStackChanged() override;
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
        void highlightCurrentRow();
        void highlightWords();
        void highlightSelection();
        bool hasFlag(rd_flag flag) const;
        void scrollRows(int nrows);

    private:
        mutable std::mutex m_mutex;
        SurfacePath m_path;
        std::vector<RDSurfaceCell> m_surface;
        std::unordered_map<int, SurfaceRow> m_surfacerows;
        std::pair<RDDocumentItem, RDDocumentItem> m_items{ };
        int m_rows{0}, m_cols{0}, m_commentcolumn{0}, m_lastcolumn{0};
        rd_flag m_flags;
};

