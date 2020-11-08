#pragma once

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
            std::deque<RDSurfaceCell> cells;
            RDDocumentItem item;
        };

        typedef std::scoped_lock<std::mutex> SurfaceLock;

    public:
        Surface(Context* ctx, rd_flag flags, uintptr_t userdata);
        ~Surface();
        size_t getPath(const RDPathItem** path) const;
        int row(int row, const RDSurfaceCell** cells) const;
        const std::string* currentWord() const;
        const std::string* wordAt(int row, int col) const;
        const std::string& selectedText() const;
        const RDDocumentItem* firstItem() const;
        const RDDocumentItem* lastItem() const;
        uintptr_t userData() const;
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
        void selectAt(int row, int col);
        void update(const RDDocumentItem* currentitem = nullptr);
        void activate();
        void deactivate();
        bool active() const;

    protected:
        void handleEvents(const RDEventArgs* event);
        void onStackChanged() override;
        void onPositionChanged() override;

    private:
        const SurfaceRow* currentSurfaceRow() const;
        RDSurfaceCell& cell(size_t row, size_t col);
        SafeDocument& document() const;
        const ItemContainer* items() const;
        int lastColumn() const;
        void notifyPositionChanged();
        void checkColumn(int row, int& col) const;
        void drawRow(SurfaceRow& sfrow, const Renderer& st);
        void drawCursor();
        void highlightCurrentRow();
        void highlightWords();
        void checkSelection();
        bool hasFlag(rd_flag flag) const;
        void scrollRows(int nrows);

    private:
        mutable std::vector<RDSurfaceCell> m_reqrows;
        mutable std::mutex m_mutex;

    private:
        SurfacePath m_path;
        std::unordered_map<int, SurfaceRow> m_surface;
        std::pair<RDDocumentItem, RDDocumentItem> m_items{ };
        int m_rows{0}, m_cols{0}, m_commentcolumn{0}, m_lastcolumn{0};
        std::string m_selectedtext;
        uintptr_t m_userdata;
        bool m_active{false};
        rd_flag m_flags;
};

