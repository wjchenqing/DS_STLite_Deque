#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {

    template<class T>
    class deque {
        static const int len = 600;

        class block;

        class node {
            friend class deque;
            friend class block;

            node *pre;
            T *val;
            node *nex;

            node() : pre(nullptr), val(nullptr), nex(nullptr) {};
            node(node *p, node *n) : pre(p), val(nullptr), nex(n) {};
            node(const T &v) : pre(nullptr), nex(nullptr) {
                val = new T(v);
            }
            ~node() {
                delete val;
            }
        };

        class block {
            friend class deque;

            friend class node;

            size_t siz;
            block *pre;
            block *nex;
            node *bh;
            node *bt;

            block() : siz(0), pre(nullptr), nex(nullptr) {
                bh = new node;
                bt = new node;
                bh->nex = bt;
                bt->pre = bh;
            }
            block(block *p, block *n, node *tmp) : siz(1), pre(p), nex(n) {
                bh = new node(nullptr, tmp);
                bt = new node(tmp, nullptr);
                tmp->pre = bh;
                tmp->nex = bt;
            }
            block(const block *other) {
                bh = new node;
                bt = new node;
                node *cur = bh;
                node *pos = other->bh->nex;
                siz = other->siz;
                for (size_t i = 1; i <= siz; ++i) {
                    node *p = new node;
                    p->val = new T(*pos->val);
                    p->pre = cur;
                    cur->nex = p;
                    cur = p;
                    pos = pos->nex;
                }
                cur->nex = bt;
                bt->pre = cur;
            }

            ~block() {
                node *tmp = bh;
                while (tmp != bt) {
                    tmp = tmp->nex;
                    delete tmp->pre;
                }
                delete tmp;
            }

            block &operator=(const block &other) {
                if (this == &other)
                    return *this;
                node *tmp = bh->nex;
                while (tmp != bt) {
                    tmp = tmp->nex;
                    delete tmp->pre;
                }
                pre = other.pre;
                nex = other.nex;
                node *cur = bh;
                node *pos = other->bh->nex;
                siz = other->siz;
                for (size_t i = 1; i <= siz; ++i) {
                    node *p = new node;
                    p->val = new T(pos->val);
                    p->pre = cur;
                    cur->nex = p;
                    cur = p;
                    pos = pos->nex;
                }
                cur->nex = bt;
                bt->pre = cur;
                return *this;
            }
        };

    private:
        size_t siz;
        size_t num;
        block *h;
        block *t;
    public:
        class const_iterator;

        class iterator {
            friend class deque;
            friend class node;
            friend class block;
            friend class const_iterator;

        private:
            deque *deque_;
            block *block_;
            size_t num_;
            node *node_;
            size_t pos_;

        public:
            iterator() : deque_(nullptr), block_(nullptr), num_(0), node_(nullptr), pos_(0) {};
            iterator(const iterator &o) : deque_(o.deque_), block_(o.block_),
                        num_(o.num_), node_(o.node_), pos_(o.pos_) {};
            iterator(const const_iterator &o) : deque_(o.deque_), block_(o.block_),
                        num_(o.num_), node_(o.node_), pos_(o.pos_) {};
            iterator(deque *d, block *b, size_t num, node *n, size_t p) :
                        deque_(d), block_(b), num_(num), node_(n), pos_(p) {}

            iterator operator+(const int &n) const {
                if (n == 0)
                    return *this;
                if (n < 0)
                    return *this - (-n);
                int step = n;
                block *tmp1 = block_;
                size_t num__ = num_;
                node *tmp2 = node_;
                size_t p = pos_;
                while (step > 0) {
                    if (p + step > tmp1->siz) {
                        if (tmp1->nex == deque_->t) {
                            return iterator(deque_, tmp1, num__, tmp1->bt, tmp1->siz + 1);
                        }
                        step -= (tmp1->siz - p);
                        tmp1 = tmp1->nex;
                        ++num__;
                        tmp2 = tmp1->bh;
                        p = 0;
                        continue;
                    }
                    p += step;
                    for (; step > 0; --step) {
                        tmp2 = tmp2->nex;
                    }
                    return iterator(deque_, tmp1, num__, tmp2, p);
                }
            }

            iterator operator-(const int &n) const {
                if (n == 0)
                    return *this;
                if (n < 0)
                    return *this + (-n);
                int step = n;
                block *tmp1 = block_;
                size_t num__ = num_;
                node *tmp2 = node_;
                size_t p = pos_;
                while (step > 0) {
                    if (step > p - 1) {
                        step -= p - 1;
                        tmp1 = tmp1->pre;
                        --num__;
                        tmp2 = tmp1->bt;
                        p = tmp1->siz + 1;
                        continue;
                    }
                    p -= step;
                    for (; step > 0; --step) {
                        tmp2 = tmp2->pre;
                    }
                    return iterator(deque_, tmp1, num__, tmp2, p);
                }
            }

            int operator-(const iterator &rhs) const {
                if (deque_ != rhs.deque_)
                    throw invalid_iterator();
                if (num_ == rhs.num_)
                    return pos_ - rhs.pos_;
                bool flag = num_ < rhs.num_;
                block *cur = flag ? block_ : rhs.block_;
                size_t n1 = flag ? num_ : rhs.num_;
                size_t n2 = flag ? rhs.num_ : num_;
                size_t p1 = flag ? pos_ : rhs.pos_;
                size_t p2 = flag ? rhs.pos_ : pos_;
                int tmp = 0;
                while (n1 < n2) {
                    tmp += cur->siz - p1;
                    cur = cur->nex;
                    ++n1;
                    p1 = 0;
                }
                tmp += p2;
                return flag ? -tmp : tmp;
            }

            iterator &operator+=(const int &n) {
                if (n == 0)
                    return *this;
                if (n < 0)
                    return *this -= -n;
                int step = n;
                while (step > 0) {
                    if (pos_ + step > block_->siz) {
                        if (block_->nex == deque_->t) {
                            node_ = block_->bt;
                            pos_ = block_->siz + 1;
                            return *this;
                        }
                        step -= (block_->siz - pos_);
                        block_ = block_->nex;
                        ++num_;
                        node_ = block_->bh;
                        pos_ = 0;
                        continue;
                    }
                    pos_ += step;
                    for (; step > 0; --step) {
                        node_ = node_->nex;
                    }
                    return *this;
                }
            }

            iterator &operator-=(const int &n) {
                if (n == 0)
                    return *this;
                if (n < 0)
                    return *this += -n;
                int step = n;
                while (step > 0) {
                    if (step > pos_ - 1) {
                        step -= pos_ - 1;
                        block_ = block_->pre;
                        --num_;
                        node_ = block_->bt;
                        pos_ = block_->siz + 1;
                        continue;
                    }
                    pos_ -= step;
                    for (; step > 0; --step) {
                        node_ = node_->pre;
                    }
                    return *this;
                }
            }

            iterator operator++(int) {
                if (block_->nex == deque_->t && pos_ == block_->siz + 1)
                    throw invalid_iterator();
                else {
                    iterator tmp(*this);
                    if (pos_ < block_->siz || block_->nex == deque_->t) {
                        node_ = node_->nex;
                        pos_++;
                    } else {
                        block_ = block_->nex;
                        num_++;
                        node_ = block_->bh->nex;
                        pos_ = 1;
                    }
                    return tmp;
                }
            }

            iterator &operator++() {
                if (block_->nex == deque_->t && pos_ == block_->siz + 1)
                    throw invalid_iterator();
                else {
                    if (pos_ < block_->siz || block_->nex == deque_->t) {
                        node_ = node_->nex;
                        pos_++;
                    } else {
                        block_ = block_->nex;
                        num_++;
                        node_ = block_->bh->nex;
                        pos_ = 1;
                    }
                    return *this;
                }
            }

            iterator operator--(int) {
                if (num_ == 1 && pos_ == 1)
                    throw invalid_iterator();
                else {
                    iterator tmp(*this);
                    if (pos_ > 1) {
                        node_ = node_->pre;
                        pos_--;
                    } else {
                        block_ = block_->pre;
                        num_--;
                        node_ = block_->bt->pre;
                        pos_ = block_->siz;
                    }
                    return tmp;
                }
            }

            iterator &operator--() {
                if (num_ == 1 && pos_ == 1)
                    throw invalid_iterator();
                else {
                    if (pos_ > 1) {
                        node_ = node_->pre;
                        pos_--;
                    } else {
                        block_ = block_->pre;
                        num_--;
                        node_ = block_->bt->pre;
                        pos_ = block_->siz;
                    }
                    return *this;
                }
            }

            T &operator*() const {
                if (block_->nex == deque_->t && pos_ == block_->siz + 1)
                    throw invalid_iterator();
                else return *node_->val;
            }
            T *operator->() const noexcept {
                if (block_->nex == deque_->t && pos_ == block_->siz + 1)
                    throw invalid_iterator();
                else return &*node_->val;
            }
            bool operator==(const iterator &rhs) const {
                return !((deque_ != rhs.deque_) || (num_ != rhs.num_) || (pos_ != rhs.pos_));
            }
            bool operator==(const const_iterator &rhs) const {
                return !((deque_ != rhs.deque_) || (num_ != rhs.num_) || (pos_ != rhs.pos_));
            }
            bool operator!=(const iterator &rhs) const {
                return ((deque_ != rhs.deque_) || (num_ != rhs.num_) || (pos_ != rhs.pos_));
            }
            bool operator!=(const const_iterator &rhs) const {
                return ((deque_ != rhs.deque_) || (num_ != rhs.num_) || (pos_ != rhs.pos_));
            }
        };

        class const_iterator {
            friend class deque;
            friend class iterator;

        private:
            const deque *deque_;
            const block *block_;
            size_t num_;
            const node *node_;
            size_t pos_;
        public:
            const_iterator() : deque_(nullptr), block_(nullptr), num_(0), node_(nullptr), pos_(0) {};
            const_iterator(const iterator &o) : deque_(o.deque_), block_(o.block_),
                                                num_(o.num_), node_(o.node_), pos_(o.pos_) {};
            const_iterator(const const_iterator &o) : deque_(o.deque_), block_(o.block_),
                                                      num_(o.num_), node_(o.node_), pos_(o.pos_) {};
            const_iterator(const deque *d, const block *b, size_t num, const node *n, size_t p) :
                    deque_(d), block_(b), num_(num), node_(n), pos_(p) {}

            const_iterator operator+(const int &n) const {
                if (n == 0)
                    return *this;
                if (n < 0)
                    return *this - (-n);
                int step = n;
                const block *tmp1 = block_;
                size_t num__ = num_;
                const node *tmp2 = node_;
                size_t p = pos_;
                while (step > 0) {
                    if (p + step > tmp1->siz) {
                        if (tmp1->nex == deque_->t) {
                            return const_iterator(deque_, tmp1, num__, tmp1->bt, tmp1->siz + 1);
                        }
                        step -= (tmp1->siz - p);
                        tmp1 = tmp1->nex;
                        ++num__;
                        tmp2 = tmp1->bh;
                        p = 0;
                        continue;
                    }
                    p += step;
                    for (; step > 0; --step) {
                        tmp2 = tmp2->nex;
                    }
                    return const_iterator(deque_, tmp1, num__, tmp2, p);
                }
            }

            const_iterator operator-(const int &n) const {
                if (n == 0)
                    return *this;
                if (n < 0)
                    return *this + (-n);
                int step = n;
                const block *tmp1 = block_;
                size_t num__ = num_;
                const node *tmp2 = node_;
                size_t p = pos_;
                while (step > 0) {
                    if (step > p - 1) {
                        step -= p - 1;
                        tmp1 = tmp1->pre;
                        --num__;
                        tmp2 = tmp1->bt;
                        p = tmp1->siz + 1;
                        continue;
                    }
                    p -= step;
                    for (; step > 0; --step) {
                        tmp2 = tmp2->pre;
                    }
                    return const_iterator(deque_, tmp1, num__, tmp2, p);
                }
            }

            int operator-(const const_iterator &rhs) const {
                if (deque_ != rhs.deque_)
                    throw invalid_iterator();
                if (num_ == rhs.num_)
                    return pos_ - rhs.pos_;
                bool flag = num_ < rhs.num_;
                const block *cur = flag ? block_ : rhs.block_;
                size_t n1 = flag ? num_ : rhs.num_;
                size_t n2 = flag ? rhs.num_ : num_;
                size_t p1 = flag ? pos_ : rhs.pos_;
                size_t p2 = flag ? rhs.pos_ : pos_;
                int tmp = 0;
                while (n1 < n2) {
                    tmp += cur->siz - p1;
                    cur = cur->nex;
                    ++n1;
                    p1 = 0;
                }
                tmp += p2;
                return flag ? -tmp : tmp;
            }

            const_iterator &operator+=(const int &n) {
                if (n == 0)
                    return *this;
                if (n < 0)
                    return *this -= (-n);
                int step = n;
                while (step > 0) {
                    if (pos_ + step > block_->siz) {
                        if (block_->nex == deque_->t) {
                            node_ = block_->bt;
                            pos_ = block_->siz + 1;
                            return *this;
                        }
                        step -= (block_->siz - pos_);
                        block_ = block_->nex;
                        ++num_;
                        node_ = block_->bh;
                        pos_ = 0;
                        continue;
                    }
                    pos_ += step;
                    for (; step > 0; --step) {
                        node_ = node_->nex;
                    }
                    return *this;
                }
            }

            const_iterator &operator-=(const int &n) {
                if (n == 0)
                    return *this;
                if (n < 0)
                    return *this += (-n);
                int step = n;
                while (step > 0) {
                    if (step > pos_ - 1) {
                        step -= pos_ - 1;
                        block_ = block_->pre;
                        --num_;
                        node_ = block_->bt;
                        pos_ = block_->siz + 1;
                        continue;
                    }
                    pos_ -= step;
                    for (; step > 0; --step) {
                        node_ = node_->pre;
                    }
                    return *this;
                }
            }

            const_iterator operator++(int) {
                if (block_->nex == deque_->t && pos_ == block_->siz + 1)
                    throw invalid_iterator();
                else {
                    const_iterator tmp(*this);
                    if (pos_ < block_->siz || block_->nex == deque_->t) {
                        node_ = node_->nex;
                        pos_++;
                    } else {
                        block_ = block_->nex;
                        num_++;
                        node_ = block_->bh->nex;
                        pos_ = 1;
                    }
                    return tmp;
                }
            }

            const_iterator &operator++() {
                if (block_->nex == deque_->t && pos_ == block_->siz + 1)
                    throw invalid_iterator();
                else {
                    if (pos_ < block_->siz || block_->nex == deque_->t) {
                        node_ = node_->nex;
                        pos_++;
                    } else {
                        block_ = block_->nex;
                        num_++;
                        node_ = block_->bh->nex;
                        pos_ = 1;
                    }
                    return *this;
                }
            }

            const_iterator operator--(int) {
                if (num_ == 1 && pos_ == 1)
                    throw invalid_iterator();
                else {
                    const_iterator tmp(*this);
                    if (pos_ > 1) {
                        node_ = node_->pre;
                        pos_--;
                    } else {
                        block_ = block_->pre;
                        num_--;
                        node_ = block_->bt->pre;
                        pos_ = block_->siz;
                    }
                    return tmp;
                }
            }

            const_iterator &operator--() {
                if (num_ == 1 && pos_ == 1)
                    throw invalid_iterator();
                else {
                    if (pos_ > 1) {
                        node_ = node_->pre;
                        pos_--;
                    } else {
                        block_ = block_->pre;
                        num_--;
                        node_ = block_->bt->pre;
                        pos_ = block_->siz;
                    }
                    return *this;
                }
            }

            const T &operator*() const {
                if (block_->nex == deque_->t && pos_ == block_->siz + 1)
                    throw invalid_iterator();
                else return *node_->val;
            }
            const T *operator->() const noexcept {
                if (block_->nex == deque_->t && pos_ == block_->siz + 1)
                    throw invalid_iterator();
                else return &*node_->val;
            }
            bool operator==(const iterator &rhs) const {
                return !((deque_ != rhs.deque_) || (num_ != rhs.num_) || (pos_ != rhs.pos_));
            }
            bool operator==(const const_iterator &rhs) const {
                return !((deque_ != rhs.deque_) || (num_ != rhs.num_) || (pos_ != rhs.pos_));
            }
            bool operator!=(const iterator &rhs) const {
                return ((deque_ != rhs.deque_) || (num_ != rhs.num_) || (pos_ != rhs.pos_));
            }
            bool operator!=(const const_iterator &rhs) const {
                return ((deque_ != rhs.deque_) || (num_ != rhs.num_) || (pos_ != rhs.pos_));
            }
        };

        deque() : num(0), siz(0) {
            h = new block;
            t = new block;
            h->nex = t;
            t->pre = h;
        }

        deque(const deque &other) {
            h = new block;
            t = new block;
            block *cur = h;
            const block *pos = other.h->nex;
            num = other.num;
            siz = other.siz;
            for (size_t i = 1; i <= num; ++i) {
                block *p = new block(pos);
                p->pre = cur;
                cur->nex = p;
                cur = p;
                pos = pos->nex;
            }
            cur->nex = t;
            t->pre = cur;
        }

        ~deque() {
            for (block *tmp = h->nex; tmp != nullptr; tmp = tmp->nex) {
                delete tmp->pre;
            }
            delete t;
        }

        deque &operator=(const deque &other) {
            if (this == &other)
                return *this;
            for (block *tmp = h->nex->nex; tmp != nullptr; tmp = tmp->nex) {
                delete tmp->pre;
            }
            block *cur = h;
            block *pos = other.h->nex;
            num = other.num;
            siz = other.siz;
            for (size_t i = 1; i <= num; ++i) {
                block *p = new block(pos);
                p->pre = cur;
                cur->nex = p;
                cur = p;
                pos = pos->nex;
            }
            cur->nex = t;
            t->pre = cur;
            return *this;
        }

        T &at(const size_t &pos) {
            if (pos >= siz)
                throw index_out_of_bound();
            int step = pos + 1;
            block *tmp1 = h->nex;
            node *tmp2 = tmp1->bh;
            while (step > 0) {
                if (step > tmp1->siz) {
                    step -= tmp1->siz;
                    tmp1 = tmp1->nex;
                    tmp2 = tmp1->bh;
                    continue;
                }
                for (; step > 0; --step) {
                    tmp2 = tmp2->nex;
                }
                return *tmp2->val;
            }
        }

        const T &at(const size_t &pos) const {
            if (pos >= siz)
                throw index_out_of_bound();
            int step = pos + 1;
            block *tmp1 = h->nex;
            node *tmp2 = tmp1->bh;
            while (step > 0) {
                if (step > tmp1->siz) {
                    step -= tmp1->siz;
                    tmp1 = tmp1->nex;
                    tmp2 = tmp1->bh;
                    continue;
                }
                for (; step > 0; --step) {
                    tmp2 = tmp2->nex;
                }
                return *tmp2->val;
            }
        }

        T &operator[](const size_t &pos) {
            return this->at(pos);
        }

        const T &operator[](const size_t &pos) const {
            return this->at(pos);
        }

        const T &front() const {
            if (siz == 0)
                throw container_is_empty();
            return *h->nex->bh->nex->val;
        }

        const T &back() const {
            if (siz == 0)
                throw container_is_empty();
            return *t->pre->bt->pre->val;
        }

        iterator begin() {
            if (siz != 0) return iterator(this, h->nex, 1, h->nex->bh->nex, 1);
            else return iterator(this, h, 0, h->bt, 1);
        }

        const_iterator cbegin() const {
            if (siz != 0) return const_iterator(this, h->nex, 1, h->nex->bh->nex, 1);
            else return const_iterator(this, h, 0, h->bt, 1);
        }

        iterator end() {
            return iterator(this, t->pre, num, t->pre->bt, t->pre->siz + 1);
        }

        const_iterator cend() const {
            return const_iterator(this, t->pre, num, t->pre->bt, t->pre->siz + 1);
        }

        bool empty() const {
            return siz == 0;
        }

        size_t size() const {
            return siz;
        }

        void clear() {
            for (block *tmp = h->nex->nex; tmp != nullptr; tmp = tmp->nex) {
                delete tmp->pre;
            }
            h->nex = t;
            t->pre = h;
            num = 0;
            siz = 0;
        }

        iterator insert(iterator pos, const T &value) {
            if (this != pos.deque_)
                throw invalid_iterator();
            ++siz;
            if (pos.num_ == 0) {
                ++num;
                node *tmp = new node(value);
                block *cur = new block(h, t, tmp);
                h->nex = cur;
                t->pre = cur;
                return iterator(this, cur, 1, tmp, 1);
            }
            node *cur = new node(pos.node_->pre, pos.node_);
            cur->val = new T(value);
            cur->pre->nex = cur;
            cur->nex->pre = cur;
            pos.node_ = cur;
            if (pos.block_->siz == len) {
                node *tmp = pos.block_->bt->pre;
                tmp->nex->pre = tmp->pre;
                tmp->pre->nex = tmp->nex;
                if (pos.block_->nex->siz == len || pos.block_->nex == t) {
                    ++num;
                    block *cur = new block(pos.block_, pos.block_->nex, tmp);
                    cur->pre->nex = cur;
                    cur->nex->pre = cur;
                } else {
                    block *cur = pos.block_->nex;
                    ++cur->siz;
                    tmp->pre = cur->bh;
                    tmp->nex = cur->bh->nex;
                    tmp->pre->nex = tmp;
                    tmp->nex->pre = tmp;
                }
            } else {
                ++pos.block_->siz;
            }
            return pos;
        }

        iterator erase(iterator pos) {
            if (pos.deque_->siz == 0 || this != pos.deque_)
                throw invalid_iterator();
            --siz;
            if (pos.block_->siz == 1) {
                --num;
                block *tmp = pos.block_;
                if (tmp->nex == t) {
                    pos.block_ = tmp->pre;
                    --pos.num_;
                    pos.node_ = pos.block_->bt;
                    pos.pos_ = pos.block_->siz + 1;
                } else {
                    pos.block_ = tmp->nex;
                    pos.node_ = pos.block_->bh->nex;
                    pos.pos_ = 1;
                }
                tmp->pre->nex = tmp->nex;
                tmp->nex->pre = tmp->pre;
                delete tmp;
                return pos;
            }
            if (pos.block_->nex != t && pos.block_->siz + pos.block_->nex->siz <= len) {
                --num;
                block *tmp1 = pos.block_;
                block *tmp2 = tmp1->nex;
                tmp1->siz += tmp2->siz;
                tmp1->bt->pre->nex = tmp2->bh->nex;
                tmp2->bh->nex->pre = tmp1->bt->pre;
                tmp1->bt->pre = tmp2->bt->pre;
                tmp1->bt->pre->nex = tmp1->bt;
                tmp2->bh->nex = tmp2->bt;
                tmp2->bt->pre = tmp2->bh;
                tmp1->nex = tmp2->nex;
                tmp2->nex->pre = tmp1;
                delete tmp2;
            } else if (pos.pos_ == pos.block_->siz) {
                node *tmp = pos.node_;
                --pos.block_->siz;
                tmp->pre->nex = tmp->nex;
                tmp->nex->pre = tmp->pre;
                delete tmp;
                if (pos.block_->nex == t) {
                    pos.node_ = pos.block_->bt;
                } else {
                    pos.block_ = pos.block_->nex;
                    ++pos.num_;
                    pos.node_ = pos.block_->bh->nex;
                    pos.pos_ = 1;
                }
                return pos;
            }
            node *tmp = pos.node_;
            --pos.block_->siz;
            tmp->pre->nex = tmp->nex;
            tmp->nex->pre = tmp->pre;
            pos.node_ = tmp->nex;
            delete tmp;
            return pos;
        }

        void push_back(const T &value) {
            ++siz;
            if (t->pre != h && t->pre->siz < len) {
                ++t->pre->siz;
                node *tmp = new node(t->pre->bt->pre, t->pre->bt);
                tmp->val = new T(value);
                tmp->pre->nex = tmp;
                tmp->nex->pre = tmp;
            } else {
                ++num;
                node *tmp = new node(value);
                block *cur = new block(t->pre, t, tmp);
                cur->pre->nex = cur;
                cur->nex->pre = cur;
            }
        }

        void pop_back() {
            if (siz == 0)
                throw container_is_empty();
            --siz;
            if (t->pre->siz > 1) {
                --t->pre->siz;
                node *tmp = t->pre->bt->pre;
                tmp->pre->nex = tmp->nex;
                tmp->nex->pre = tmp->pre;
                delete tmp;
            } else {
                --num;
                block *tmp = t->pre;
                tmp->pre->nex = tmp->nex;
                tmp->nex->pre = tmp->pre;
                delete tmp;
            }
        }

        void push_front(const T &value) {
            ++siz;
            if (h->nex != t && h->nex->siz < len) {
                ++h->nex->siz;
                node *tmp = new node(h->nex->bh, h->nex->bh->nex);
                tmp->val = new T(value);
                tmp->pre->nex = tmp;
                tmp->nex->pre = tmp;
            } else {
                ++num;
                node *tmp = new node(value);
                block *cur = new block(h, h->nex, tmp);
                cur->pre->nex = cur;
                cur->nex->pre = cur;
            }
        }

        void pop_front() {
            if (siz == 0)
                throw container_is_empty();
            --siz;
            if (h->nex->siz > 1) {
                --h->nex->siz;
                node *tmp = h->nex->bh->nex;
                tmp->pre->nex = tmp->nex;
                tmp->nex->pre = tmp->pre;
                delete tmp;
            } else {
                --num;
                block *tmp = h->nex;
                tmp->pre->nex = tmp->nex;
                tmp->nex->pre = tmp->pre;
                delete tmp;
            }
        }
    };

}

#endif
