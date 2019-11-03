#pragma once

#include <algorithm>
#include <memory>
#include <deque>
#include <list>

namespace REDasm {

template<typename T> class TreeNode
{
    public:
        static constexpr size_t npos = size_t(-1);
        typedef TreeNode<T> NodeType;

    private:
        typedef std::shared_ptr<NodeType> NodeTypePtr;

    public:
        TreeNode() = default;
        NodeType* parent() const { return m_parent; }
        bool hasParent() const { return m_parent; }
        bool empty() const { return m_children.empty(); }
        NodeType* at(size_t idx) const { return m_children[idx].get(); }
        size_t size() const { return m_children.size(); }

        size_t indexOf(const T& data) const {
            auto it = this->find(data);
            return (it != m_children.end()) ? std::distance(m_children.begin(), it) : npos;
        }

        size_t indexOf(const NodeType* node) const { return this->indexOf(node->data); }
        size_t parentIndexOf(const T& data) const { return m_parent ? m_parent->indexOf(data) : npos; }
        size_t parentIndexOf(const NodeType* node) const { return this->parentIndexOf(node->data); }
        size_t index() const { return m_parent ? m_parent->indexOf(this) : npos; }

        template<typename N = NodeType> N* add(const T& data) {
            N* node = this->add<N>();
            node->data = data;
            return node;
        }

        template<typename N = NodeType> N* add() {
            auto node = std::make_shared<N>();
            node->m_parent = this;
            m_children.push_back(node);
            return static_cast<N*>(m_children.back().get());
        }

        void remove(const T& data) { auto it = this->find(data); m_children.erase(it); }
        void remove(const NodeType* node) { this->remove(node->data); }
        void clear() { m_children.clear(); }

    private:
        typename std::deque<NodeTypePtr>::const_iterator find(const T& data) const {
            return std::find_if(m_children.begin(), m_children.end(), [&](const NodeTypePtr& item) -> bool {
                return item->data == data;
            });
        }

    protected:
        std::deque<NodeTypePtr> m_children;
        NodeType* m_parent{nullptr};

    public:
        T data{ };
};

template<typename T> using Tree = TreeNode<T>;

} // namespace REDasm
