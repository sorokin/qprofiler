#ifndef UNDO_STACK_H
#define UNDO_STACK_H

#include <vector>
#include <type_traits>

template <typename T>
struct undo_stack
{
    static_assert(std::is_nothrow_move_constructible<T>::value
               && std::is_nothrow_move_assignable<T>::value, "T must be nothrow moveable");

    enum class action_type
    {
        undo,
        redo,
    };

    undo_stack()
    {}

    undo_stack(T current_item)
        : current_item(std::move(current_item))
    {}

    T& current()
    {
        return current_item;
    }

    T const& current() const
    {
        return current_item;
    }

    void reset(T item = T{})
    {
        undo_list.clear();
        redo_list.clear();
        current_item = std::move(item);
    }

    void push(T item)
    {
        undo_list.push_back(std::move(current_item));
        redo_list.clear();              // nothrow
        current_item = std::move(item); // nothrow
    }

    bool can_undo(action_type action) const
    {
        return !list_by_action(action).empty();
    }

    T const& peek_undo_item(action_type action) const
    {
        return list_by_action(action).back();
    }

    void undo(action_type action)
    {
        std::vector<T>& my_list = list_by_action(action);
        std::vector<T>& other_list = list_by_action(invert(action));

        other_list.push_back(std::move(current_item));
        current_item = std::move(my_list.back());
        my_list.pop_back();
    }

private:
    static action_type invert(action_type action)
    {
        return action == action_type::undo ? action_type::redo : action_type::undo;
    }

    std::vector<T>& list_by_action(action_type action)
    {
        return action == action_type::undo ? undo_list : redo_list;
    }

    std::vector<T> const& list_by_action(action_type action) const
    {
        return action == action_type::undo ? undo_list : redo_list;
    }

private:
    std::vector<T> undo_list;
    std::vector<T> redo_list;
    T current_item;
};

#endif
