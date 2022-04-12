#pragma once

#include "surfacerenderer.h"
#include "surfacepath.h"
#include "cursor.h"

class BlockContainer;
class Renderer;

class Surface: public SurfaceRenderer
{
    private:

    public:
        Surface(Context* ctx, rd_flag flags, uintptr_t userdata);
        ~Surface();
        const Cursor* cursor() const;
        Cursor* cursor();
        size_t getPath(const RDPathItem** path) const;
        const std::string* currentWord() const;
        const std::string* wordAt(int row, int col) const;
        const std::string& selectedText() const;
        rd_address currentAddress() const;
        const std::string* currentLabel(rd_address* resaddress) const;
        uintptr_t userData() const;
        rd_address getAddress(int row) const;
        void getScrollRange(rd_address* start, rd_address* end) const;
        bool contains(rd_address address) const;
        bool labelAt(int row, int col, rd_address* resaddress) const;
        bool goTo(rd_address address, bool updatehistory = true);
        bool goTo(const RDBlock* block, bool updatehistory = true);
        void setUserData(uintptr_t userdata);
        void scroll(int nx, int ny);
        void moveTo(int row, int col);
        void select(int row, int col);
        void selectAt(int row, int col);
        void activate();
        void deactivate();
        bool active() const;

    public: // Used by Cursor class
        void notifyHistoryChanged();
        void notifyAddressChanged();

    protected:
        void updateCompleted() override;

    private:
        void handleEvents(const RDEventArgs* event);
        void checkColumn(int row, int& col) const;
        bool ensureVisible(const BlockContainer* blocks, rd_address address);
        void scrollAddress(rd_address address, int n);
        void drawCursor();
        void highlightCurrentRow();
        void highlightWords();
        void checkSelection();

    private:
        Cursor m_cursor;
        SurfacePath m_path;
        std::string m_selectedtext;
        uintptr_t m_userdata;
        bool m_active{false}, m_updatecursor{false};
};
